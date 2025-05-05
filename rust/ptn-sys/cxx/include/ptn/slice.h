// SPDX-FileCopyrightText: 2025 Nickolay Muravev
// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (C) 2025 Nickolay Muravev

// Taken from https://github.com/dtolnay/cxx

#ifndef SLICE_H
#define SLICE_H

#include "impl.h"
#include "layout.h"
#include <array>
#include <cstddef>
#include <cstdint>

namespace rust
{

    namespace detail
    {
        template <bool> struct copy_assignable_if {};

        template <> struct copy_assignable_if<false> {
            copy_assignable_if() noexcept = default;
            copy_assignable_if(const copy_assignable_if&) noexcept = default;
            copy_assignable_if& operator=(const copy_assignable_if&) & noexcept = delete;
            copy_assignable_if& operator=(copy_assignable_if&&) & noexcept = default;
        };
    } // namespace detail

    // https://cxx.rs/binding/slice.html
    template <typename T>
    class Slice final : private detail::copy_assignable_if<std::is_const<T>::value> {
    public:
        using value_type = T;

        Slice() noexcept;
        Slice(T*, std::size_t count) noexcept;

        template <typename C> explicit Slice(C& c) : Slice(c.data(), c.size()) {}

        Slice& operator=(const Slice<T>&) & noexcept = default;
        Slice& operator=(Slice<T>&&) & noexcept = default;

        T* data() const noexcept;
        std::size_t size() const noexcept;
        std::size_t length() const noexcept;
        bool empty() const noexcept;

        T& operator[](std::size_t n) const noexcept;
        T& at(std::size_t n) const;
        T& front() const noexcept;
        T& back() const noexcept;

        // Important in order for System V ABI to pass in registers.
        Slice(const Slice<T>&) noexcept = default;
        ~Slice() noexcept = default;

        class iterator;
        iterator begin() const noexcept;
        iterator end() const noexcept;

        void swap(Slice&) noexcept;

    private:
        class uninit;
        Slice(uninit) noexcept;
        friend impl<Slice>;
        friend void sliceInit(void*, const void*, std::size_t) noexcept;
        friend void* slicePtr(const void*) noexcept;
        friend std::size_t sliceLen(const void*) noexcept;

        std::array<std::uintptr_t, 2> repr;
    };

    template <typename T> class Slice<T>::iterator final {
    public:
        using iterator_category = std::random_access_iterator_tag;
        using value_type = T;
        using difference_type = std::ptrdiff_t;
        using pointer = typename std::add_pointer<T>::type;
        using reference = typename std::add_lvalue_reference<T>::type;

        reference operator*() const noexcept;
        pointer operator->() const noexcept;
        reference operator[](difference_type) const noexcept;

        iterator& operator++() noexcept;
        iterator operator++(int) noexcept;
        iterator& operator--() noexcept;
        iterator operator--(int) noexcept;

        iterator& operator+=(difference_type) noexcept;
        iterator& operator-=(difference_type) noexcept;
        iterator operator+(difference_type) const noexcept;
        iterator operator-(difference_type) const noexcept;
        difference_type operator-(const iterator&) const noexcept;

        bool operator==(const iterator&) const noexcept;
        bool operator!=(const iterator&) const noexcept;
        bool operator<(const iterator&) const noexcept;
        bool operator<=(const iterator&) const noexcept;
        bool operator>(const iterator&) const noexcept;
        bool operator>=(const iterator&) const noexcept;

    private:
        friend class Slice;
        void* pos;
        std::size_t stride;
    };

    template <typename T> Slice<T>::Slice() noexcept
    {
        sliceInit(this, reinterpret_cast<void*>(align_of<T>()), 0);
    }

    template <typename T> Slice<T>::Slice(T* s, std::size_t count) noexcept
    {
        assert(s != nullptr || count == 0);
        sliceInit(
            this,
            s == nullptr && count == 0 ? reinterpret_cast<void*>(align_of<T>())
                                       : const_cast<typename std::remove_const<T>::type*>(s),
            count
        );
    }

    template <typename T> T* Slice<T>::data() const noexcept
    {
        return reinterpret_cast<T*>(slicePtr(this));
    }

    template <typename T> std::size_t Slice<T>::size() const noexcept { return sliceLen(this); }

    template <typename T> std::size_t Slice<T>::length() const noexcept { return this->size(); }

    template <typename T> bool Slice<T>::empty() const noexcept { return this->size() == 0; }

    template <typename T> T& Slice<T>::operator[](std::size_t n) const noexcept
    {
        assert(n < this->size());
        auto ptr = static_cast<char*>(slicePtr(this)) + size_of<T>() * n;
        return *reinterpret_cast<T*>(ptr);
    }

    template <typename T> T& Slice<T>::at(std::size_t n) const
    {
        if (n >= this->size()) {
            throw "rust::Slice index out of range";
        }
        return (*this)[n];
    }

    template <typename T> T& Slice<T>::front() const noexcept
    {
        assert(!this->empty());
        return (*this)[0];
    }

    template <typename T> T& Slice<T>::back() const noexcept
    {
        assert(!this->empty());
        return (*this)[this->size() - 1];
    }

    template <typename T>
    typename Slice<T>::iterator::reference Slice<T>::iterator::operator*() const noexcept
    {
        return *static_cast<T*>(this->pos);
    }

    template <typename T>
    typename Slice<T>::iterator::pointer Slice<T>::iterator::operator->() const noexcept
    {
        return static_cast<T*>(this->pos);
    }

    template <typename T>
    typename Slice<T>::iterator::reference
    Slice<T>::iterator::operator[](typename Slice<T>::iterator::difference_type n) const noexcept
    {
        auto ptr = static_cast<char*>(this->pos) + this->stride * n;
        return *reinterpret_cast<T*>(ptr);
    }

    template <typename T> typename Slice<T>::iterator& Slice<T>::iterator::operator++() noexcept
    {
        this->pos = static_cast<char*>(this->pos) + this->stride;
        return *this;
    }

    template <typename T> typename Slice<T>::iterator Slice<T>::iterator::operator++(int) noexcept
    {
        auto ret = iterator(*this);
        this->pos = static_cast<char*>(this->pos) + this->stride;
        return ret;
    }

    template <typename T> typename Slice<T>::iterator& Slice<T>::iterator::operator--() noexcept
    {
        this->pos = static_cast<char*>(this->pos) - this->stride;
        return *this;
    }

    template <typename T> typename Slice<T>::iterator Slice<T>::iterator::operator--(int) noexcept
    {
        auto ret = iterator(*this);
        this->pos = static_cast<char*>(this->pos) - this->stride;
        return ret;
    }

    template <typename T>
    typename Slice<T>::iterator&
    Slice<T>::iterator::operator+=(typename Slice<T>::iterator::difference_type n) noexcept
    {
        this->pos = static_cast<char*>(this->pos) + this->stride * n;
        return *this;
    }

    template <typename T>
    typename Slice<T>::iterator&
    Slice<T>::iterator::operator-=(typename Slice<T>::iterator::difference_type n) noexcept
    {
        this->pos = static_cast<char*>(this->pos) - this->stride * n;
        return *this;
    }

    template <typename T>
    typename Slice<T>::iterator
    Slice<T>::iterator::operator+(typename Slice<T>::iterator::difference_type n) const noexcept
    {
        auto ret = iterator(*this);
        ret.pos = static_cast<char*>(this->pos) + this->stride * n;
        return ret;
    }

    template <typename T>
    typename Slice<T>::iterator
    Slice<T>::iterator::operator-(typename Slice<T>::iterator::difference_type n) const noexcept
    {
        auto ret = iterator(*this);
        ret.pos = static_cast<char*>(this->pos) - this->stride * n;
        return ret;
    }

    template <typename T>
    typename Slice<T>::iterator::difference_type Slice<T>::iterator::operator-(const iterator& other
    ) const noexcept
    {
        auto diff = std::distance(static_cast<char*>(other.pos), static_cast<char*>(this->pos));
        return diff / static_cast<typename Slice<T>::iterator::difference_type>(this->stride);
    }

    template <typename T> bool Slice<T>::iterator::operator==(const iterator& other) const noexcept
    {
        return this->pos == other.pos;
    }

    template <typename T> bool Slice<T>::iterator::operator!=(const iterator& other) const noexcept
    {
        return this->pos != other.pos;
    }

    template <typename T> bool Slice<T>::iterator::operator<(const iterator& other) const noexcept
    {
        return this->pos < other.pos;
    }

    template <typename T> bool Slice<T>::iterator::operator<=(const iterator& other) const noexcept
    {
        return this->pos <= other.pos;
    }

    template <typename T> bool Slice<T>::iterator::operator>(const iterator& other) const noexcept
    {
        return this->pos > other.pos;
    }

    template <typename T> bool Slice<T>::iterator::operator>=(const iterator& other) const noexcept
    {
        return this->pos >= other.pos;
    }

    template <typename T> typename Slice<T>::iterator Slice<T>::begin() const noexcept
    {
        iterator it;
        it.pos = slicePtr(this);
        it.stride = size_of<T>();
        return it;
    }

    template <typename T> typename Slice<T>::iterator Slice<T>::end() const noexcept
    {
        iterator it = this->begin();
        it.pos = static_cast<char*>(it.pos) + it.stride * this->size();
        return it;
    }

    template <typename T> void Slice<T>::swap(Slice& rhs) noexcept { std::swap(*this, rhs); }

    template <typename T> class Slice<T>::uninit {};

    template <typename T> inline Slice<T>::Slice(uninit) noexcept {}

    namespace repr
    {
        using Fat = ::std::array<::std::uintptr_t, 2>;
    } // namespace repr

    namespace
    {
        template <typename T> class impl<Slice<T>> final {
        public:
            static Slice<T> slice(repr::Fat repr) noexcept
            {
                Slice<T> slice = typename Slice<T>::uninit{};
                slice.repr = repr;
                return slice;
            }
        };
    } // namespace
} // namespace rust

#endif // SLICE_H
