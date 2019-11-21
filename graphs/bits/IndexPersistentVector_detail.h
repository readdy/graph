//
// Created by mho on 11/6/19.
//

#pragma once

#include <vector>
#include <stack>
#include <algorithm>
#include <fmt/format.h>

namespace graphs::detail {
template<typename T, typename = void>
struct can_be_deactivated : std::false_type {
};
template<typename T>
struct can_be_deactivated<T, std::void_t<decltype(std::declval<T>().deactivate())>> : std::true_type {
};

template<typename T, typename = void>
struct can_be_ptr_deactivated : std::false_type {
};
template<typename T>
struct can_be_ptr_deactivated<T, std::void_t<decltype(std::declval<T>()->deactivate())>> : std::true_type {
};

template<template<typename...> class BackingVector, typename T, typename... Rest>
class IndexPersistentContainer {
    static_assert(detail::can_be_deactivated<T>::value || detail::can_be_ptr_deactivated<T>::value,
                  "IndexPersistentVector can only work with (ptr) element types which have a deactivate() method");
public:
    /**
     * the size type of this, inherited from the backing vector
     */
    using size_type = typename BackingVector<T, Rest...>::size_type;

    /**
     * stack of blanks (indices) type
     */
    using BlanksList = std::vector<size_type>;

    /**
     * the difference type of this, inherited from the backing vector
     */
    using difference_type = typename BackingVector<T, Rest...>::difference_type;
    /**
     * the allocator type of this, inherited from the backing vector
     */
    using allocator_type = typename BackingVector<T, Rest...>::allocator_type;
    /**
     * the value type of this, inherited from the backing vector
     */
    using value_type = typename BackingVector<T, Rest...>::value_type;

    /**
     * the iterator type, same as backing vector's iterator
     */
    using iterator = typename BackingVector<T, Rest...>::iterator;
    /**
     * the const iterator type, same as backing vector's const iterator
     */
    using const_iterator = typename BackingVector<T, Rest...>::const_iterator;
    /**
     * the reverse iterator type, same as backing vector's reverse iterator
     */
    using reverse_iterator = typename BackingVector<T, Rest...>::reverse_iterator;
    /**
     * the const reverse iterator type, same as backing vector's const reverse iterator
     */
    using const_reverse_iterator = typename BackingVector<T, Rest...>::const_reverse_iterator;

    class active_iterator {
    public:
        using difference_type = typename iterator::difference_type;
        using value_type = typename iterator::value_type;
        using reference = typename iterator::reference;
        using pointer = typename iterator::pointer;
        using iterator_category = typename iterator::iterator_category;

        active_iterator() : parent(), begin(), end(), blanksPtr() {}
        active_iterator(iterator parent, iterator begin, iterator end, BlanksList* blanksPtr)
            : parent(parent), begin(begin), end(end), blanksPtr(blanksPtr) {
            skipBlanks();
        }

        bool operator==(const active_iterator& other) const { return parent == other.parent; }
        bool operator!=(const active_iterator& other) const { return parent != other.parent; }
        bool operator<(const active_iterator& other) const { return parent < other.parent; }
        bool operator>(const active_iterator& other) const { return parent > other.parent; }
        bool operator<=(const active_iterator& other) const { return parent <= other.parent; }
        bool operator>=(const active_iterator& other) const { return parent >= other.parent; }

        reference operator*() const { return *parent; }
        pointer operator->() const { return parent.operator->(); }
        reference operator[](size_type i) const {
            return *(operator+(i));
        }

        active_iterator& operator++() {
            if(parent != end) {
                ++parent;
                skipBlanks();
            }
            return *this;
        }

        active_iterator operator++(int) {
            iterator copy(*this);
            ++(*this);
            return copy;
        }
        active_iterator& operator--() {
            if(parent != begin) {
                --parent;
                auto pos = std::distance(begin, parent);
                auto it = std::lower_bound(blanksPtr->begin(), blanksPtr->end(), pos);
                while(it != blanksPtr->begin() && parent != begin && pos == *it) {
                    --parent;
                    --pos;
                    --it;
                }
            }
            return *this;
        }
        active_iterator operator--(int) {
            iterator copy(*this);
            --(*this);
            return copy;
        }

        active_iterator& operator+=(size_type n) {
            auto pos = std::distance(begin, parent);
            auto targetPos = pos + n;
            auto it = std::lower_bound(blanksPtr->begin(), blanksPtr->end(), pos);
            while(it != blanksPtr->end() && *it <= targetPos) {
                ++targetPos;
                ++it;
            }
            parent += targetPos - pos;
            return *this;
        }

        active_iterator operator+(size_type n) const {
            active_iterator copy(*this);
            copy += n;
            return copy;
        }

        friend active_iterator operator+(size_type n, const active_iterator& it) {
            return it + n;
        }

        active_iterator& operator-=(size_type n) {
            auto pos = std::distance(begin, parent);
            auto targetPos = pos - n;
            auto it = std::upper_bound(blanksPtr->begin(), blanksPtr->end(), pos);
            while (it != blanksPtr->begin() && *it >= targetPos) {
                --targetPos;
                --it;
            }
            parent -= pos - targetPos;
            return *this;
        }

        active_iterator operator-(size_type n) const {
            iterator copy (*this);
            copy -= n;
            return copy;
        }

        difference_type operator-(active_iterator rhs) const {
            auto dist = parent - rhs.parent;
            // find number of blanks in that range
            auto pos = std::distance(begin, parent);
            auto rhsPos = std::distance(begin, rhs.parent);
            auto nBlanksThis = std::distance(blanksPtr->begin(), std::lower_bound(blanksPtr->begin(), blanksPtr->end(), pos));
            auto nBlanksThat = std::distance(blanksPtr->begin(), std::lower_bound(blanksPtr->begin(), blanksPtr->end(), rhsPos));
            return dist - (nBlanksThis - nBlanksThat);
        }

    private:
        void skipBlanks() {
            auto pos = std::distance(begin, parent);
            auto it = std::lower_bound(blanksPtr->begin(), blanksPtr->end(), pos);
            while(it != blanksPtr->end() && parent != end && pos == *it) {
                ++parent;
                ++pos;
                ++it;
            }
        }

        iterator parent;
        iterator begin;
        iterator end;
        const BlanksList *blanksPtr;
    };

    /**
     * gives access to the backing vector
     * @return a reference to the backing vector
     */
    value_type* data() {
        return _backingVector.data();
    };

    /**
     * gives const access to the backing vector
     * @return a const reference to the backing vector
     */
    const value_type* data() const {
        return _backingVector.data();
    }

    /**
     * the size of this container, including blanks
     * @return the size
     */
    [[nodiscard]] size_type size() const {
        return _backingVector.size();
    }

    /**
     * returns whether the container is empty
     * @return true if it is empty, false otherwise
     */
    [[nodiscard]] bool empty() const {
        return _backingVector.empty();
    }

    [[nodiscard]] bool empty_active() const {
        return _backingVector.size() == _blanks.size();
    }

    /**
     * clears this container
     */
    void clear() {
        _backingVector.clear();
        _blanks.clear();
    }

    /**
     * Performs a push_back. If the blanks stack is empty, the element is simply pushed back to the backing vector,
     * otherwise it is inserted at the index the stack's first element is pointing to, which then is erased.
     * @param val the value to insert
     * @return an iterator pointing to the inserted element
     */
    iterator push_back(T &&val) {
        if (_blanks.empty()) {
            _backingVector.push_back(std::forward<T>(val));
            return std::prev(_backingVector.end());
        } else {
            const auto idx = _blanks.back();
            _blanks.pop_back();
            _backingVector.at(idx) = std::move(val);
            return _backingVector.begin() + idx;
        }
    }

    /**
     * Performs a push_back. Same as the implementation with an r-value.
     * @param val the value to insert
     * @return an iterator pointing to the inserted element
     */
    iterator push_back(const T &val) {
        if (_blanks.empty()) {
            _backingVector.push_back(val);
            return std::prev(_backingVector.end());
        } else {
            const auto idx = _blanks.back();
            _blanks.pop_back();
            _backingVector.at(idx) = val;
            return _backingVector.begin() + idx;
        }
    }

    /**
     * Performs an emplace_back. Works in the same way as push_back.
     * @tparam Args argument types
     * @param args arguments
     * @return an iterator pointed to the emplaced element
     */
    template<typename... Args>
    iterator emplace_back(Args &&... args) {
        if (_blanks.empty()) {
            _backingVector.emplace_back(std::forward<Args>(args)...);
            return std::prev(_backingVector.end());
        } else {
            const auto idx = _blanks.back();
            _blanks.pop_back();
            _backingVector.get_allocator().construct(&*_backingVector.begin() + idx, std::forward<Args>(args)...);
            return _backingVector.begin() + idx;
        }
    }

    /**
     * Removes an element by deactivating it and pushing the index to the blanks stack.
     * @param pos an iterator pointing to the element that should be erased
     */
    void erase(iterator pos) {
        deactivate(pos);
        insertBlank(std::distance(_backingVector.begin(), pos));
    }

    /**
     * Erases a range of elements.
     * @param start begin of the range, inclusive
     * @param end end of the range, exclusive
     */
    void erase(iterator start, const_iterator end) {
        auto offset = start - _backingVector.begin();
        for (auto it = start; it != end; ++it, ++offset) {
            deactivate(it);
            insertBlank(offset);
        }
    }

    /**
     * Yields the number of deactivated elements, i.e., size() - n_deactivated() is the effective size of this
     * container.
     * @return the number of deactivated elements
     */
    [[nodiscard]] typename BlanksList::size_type n_deactivated() const {
        return _blanks.size();
    }

    /**
     * The effective size of this container
     * @return the effective size
     */
    [[nodiscard]] size_type size_active() const {
        return _backingVector.size() - n_deactivated();
    }

    /**
     * Yields a reference to the element at the requested index.
     * @param index the index
     * @return a reference to the element
     */
    T &at(size_type index) {
        auto &x = _backingVector.at(index);
        if(x.deactivated()) {
            throw std::invalid_argument(fmt::format("Requested deactivated element {}", index));
        }
        return x;
    }

    /**
     * Yields a const reference to the element at the requested index.
     * @param index the index
     * @return a const reference to the element
     */
    const T &at(size_type index) const {
        const auto &x = _backingVector.at(index);
        if(x.deactivated()) {
            throw std::invalid_argument(fmt::format("Requested deactivated element {}", index));
        }
        return x;
    }

    /**
     * Yields an iterator pointing to the begin of this container.
     * @return the iterator
     */
    iterator begin() noexcept {
        return _backingVector.begin();
    }

    active_iterator begin_active() noexcept {
        return active_iterator(_backingVector.begin(), _backingVector.begin(), _backingVector.end(), &_blanks);
    }

    /**
     * Yields an iterator pointing to the end of this container.
     * @return the iterator
     */
    iterator end() noexcept {
        return _backingVector.end();
    }

    active_iterator end_active() noexcept {
        return active_iterator(_backingVector.end(), _backingVector.begin(), _backingVector.end(), &_blanks);
    }

    /**
     * Yields a const iterator pointing to the begin of this container.
     * @return the iterator
     */
    const_iterator begin() const noexcept {
        return _backingVector.begin();
    }

    /**
     * Yields a const iterator pointing to the begin of this container.
     * @return the iterator
     */
    const_iterator cbegin() const noexcept {
        return _backingVector.cbegin();
    }

    /**
     * Yields a const iterator pointing to the end of this container.
     * @return the iterator
     */
    const_iterator end() const noexcept {
        return _backingVector.end();
    }

    /**
     * Yields a const iterator pointing to the end of this container.
     * @return the iterator
     */
    const_iterator cend() const noexcept {
        return _backingVector.cend();
    }

    /**
     * Yields a reverse iterator to the begin of the reversed structure.
     * @return the reverse iterator.
     */
    reverse_iterator rbegin() noexcept {
        return _backingVector.rbegin();
    }

    /**
     * Yields a reverse iterator to the end of the reversed structure.
     * @return the reverse iterator
     */
    reverse_iterator rend() noexcept {
        return _backingVector.rend();
    }

    /**
     * Yields a const reverse iterator to the begin of the reversed structure.
     * @return the const reverse iterator
     */
    const_reverse_iterator rbegin() const noexcept {
        return _backingVector.rbegin();
    }

    /**
     * Yields a const reverse iterator to the end of the reversed structure.
     * @return the const reverse iterator
     */
    const_reverse_iterator rend() const noexcept {
        return _backingVector.rend();
    }

    /**
     * Yields a const reverse iterator to the begin of the reversed structure.
     * @return the const reverse iterator
     */
    const_reverse_iterator crbegin() const noexcept {
        return _backingVector.crbegin();
    }

    /**
     * Yields a const reverse iterator to the end of the reversed structure.
     * @return the const reverse iterator
     */
    const_reverse_iterator crend() const noexcept {
        return _backingVector.crend();
    }

private:

    /**
     * Deactivate an element if the backing structure contains raw elements.
     * @tparam Q element type
     * @param it the iterator to the element
     */
    template<typename Q = T>
    typename std::enable_if<detail::can_be_deactivated<Q>::value>::type deactivate(iterator it) {
        it->deactivate();
    }

    /**
     * Deactivate an element if the backing structure contains a pointer type.
     * @tparam Q element type
     * @param it the iterator to the element
     */
    template<typename Q = T>
    typename std::enable_if<detail::can_be_ptr_deactivated<Q>::value>::type deactivate(iterator it) {
        (*it)->deactivate();
    }

    void insertBlank(typename BlanksList::value_type val) {
        auto it = std::lower_bound(_blanks.begin(), _blanks.end(), val, std::less<>());
        _blanks.insert(it, val);
    }

    BlanksList _blanks;
    BackingVector<T, Rest...> _backingVector;
};

}
