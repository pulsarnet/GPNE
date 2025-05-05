// SPDX-FileCopyrightText: 2025 Nickolay Muravev
// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (C) 2025 Nickolay Muravev

#ifndef BOX_H
#define BOX_H

#include <memory>

namespace ptn
{
    template <typename T> T* allocate();

    template <typename T> void deallocate(T*);

    template <typename T> class Box final {

    public:
        Box(const Box&) = delete;
        Box(Box&&) noexcept;
        ~Box() noexcept;

        explicit Box(const T&);
        explicit Box(T&&);

        Box& operator=(const Box&) const = delete;
        Box& operator=(Box&&) noexcept;

        const T* operator->() const noexcept;
        const T& operator*() const noexcept;

        T* operator->() noexcept;
        T& operator*() noexcept;

        static Box from_raw(T*) noexcept;
        T* into_raw() noexcept;

    private:
        Box();

        T* m_ptr;
    };

    template <typename T> Box<T>::Box(Box&& other) noexcept : m_ptr(other.m_ptr)
    {
        other.m_ptr = nullptr;
    }

    template <typename T> Box<T>::~Box() noexcept
    {
        if (this->m_ptr) {
            deallocate(this->m_ptr);
        }
    }

    template <typename T> Box<T>::Box(const T& val)
    {
        // TODO: check if T is trivially copyable
        T* ptr = allocate<T>();
        ::new (ptr) T(val);
        this->m_ptr = ptr;
    }

    template <typename T> Box<T>::Box(T&& val)
    {
        T* ptr = allocate<T>();
        ::new (ptr) T(std::move(val));
        this->m_ptr = ptr;
    }

    template <typename T> Box<T>& Box<T>::operator=(Box&& other) noexcept
    {
        if (this->m_ptr) {
            deallocate(this->m_ptr);
        }
        this->m_ptr = other.m_ptr;
        other.m_ptr = nullptr;
        return *this;
    }

    template <typename T> const T* Box<T>::operator->() const noexcept { return this->m_ptr; }

    template <typename T> const T& Box<T>::operator*() const noexcept { return *this->m_ptr; }

    template <typename T> T* Box<T>::operator->() noexcept { return this->m_ptr; }

    template <typename T> T& Box<T>::operator*() noexcept { return *this->m_ptr; }

    template <typename T> Box<T> Box<T>::from_raw(T* raw) noexcept
    {
        Box box;
        box.m_ptr = raw;
        return box;
    }

    template <typename T> T* Box<T>::into_raw() noexcept
    {
        T* ptr = this->m_ptr;
        this->m_ptr = nullptr;
        return ptr;
    }

    template <typename T> Box<T>::Box() : m_ptr(nullptr) {}

} // namespace ptn

#endif // BOX_H
