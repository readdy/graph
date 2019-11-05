/**
 * This file contains the index_persistent_vector, a vector structure together with a stack of 'blanks'. Removal of
 * elements will result in a push back onto the stack of their respective indices, rendering them 'blank'. This handling
 * potentially increases the memory requirements but avoids the shift of access indices.
 *
 * @file index_persistent_vector.h
 * @brief Definitions for the index_persistent_vector
 * @author clonker
 * @date 09.06.17
 * @copyright BSD-3
 */

#pragma once

#include <vector>
#include <stack>
#include <algorithm>

namespace graphs {

namespace detail {
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
class index_persistent_container {
    static_assert(detail::can_be_deactivated<T>::value || detail::can_be_ptr_deactivated<T>::value,
                  "index_persistent_vector can only work with (ptr) element types which have a deactivate() method");
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
        return _backingVector.size() - _blanks.size();
    }

    /**
     * returns whether the container is empty
     * @return true if it is empty, false otherwise
     */
    [[nodiscard]] bool empty() const {
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
        _blanks.push_back(pos - _backingVector.begin());
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
            _blanks.push_back(offset);
        }
    }

    /**
     * Yields the number of deactivated elements, i.e., size() - n_deactivated() is the effective size of this
     * container.
     * @return the number of deactivated elements
     */
    typename BlanksList::size_type n_deactivated() const {
        return _blanks.size();
    }

    /**
     * The effective size of this container
     * @return the effective size
     */
    size_type effective_size() const {
        return _backingVector.size();
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

    /**
     * Yields an iterator pointing to the end of this container.
     * @return the iterator
     */
    iterator end() noexcept {
        return _backingVector.end();
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

    BlanksList _blanks;
    BackingVector<T, Rest...> _backingVector;
};

}

template<typename T>
using index_persistent_vector = detail::index_persistent_container<std::vector, T>;

}
