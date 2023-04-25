#pragma once

#ifdef USE_MODULES
import <cstdint>;
import <utility>;
#else
#include <cstdint>
#include <utility>
#endif

template <typename T>
struct Owned
{
private:
    using value_type = std::remove_all_extents_t<T>;
    using pointer = value_type*;
    using reference = value_type&;
    using const_pointer = const value_type*;
    using const_reference = const value_type&;
    using Self = Owned;

private:
    pointer ptr;

    template <typename T2>
    friend struct Owned;

public:
    constexpr Owned(pointer ptr = nullptr) : ptr(ptr) {}

    Owned(const Owned<T>&) = delete;
    Owned<T>& operator = (const Owned<T>&) = delete;

    template <typename T2>
    Owned(Owned<T2>&& other) noexcept : ptr(other.ptr)
    {
        other.ptr = nullptr;
    }

    template <typename T2>
    auto operator = (Owned<T2>&& other) noexcept -> Self&
    {
        Owned(std::move(other)).swap(*this);
        return *this;
    }

    ~Owned()
    {
        if(ptr != nullptr)
        {
            if constexpr (std::is_array_v<T>)
                delete[] ptr;
            else
                delete ptr;

            ptr = nullptr;
        }
    }

    template <typename... Args>
    requires (!std::is_array_v<T>)
    static auto make(Args&&... args) -> Self
    {
        return {new value_type{std::forward<Args>(args)...}};
    }

    template <typename T2 = T>
    requires std::is_array_v<T>
    static auto make(std::size_t size) -> Self
    {
        return {new value_type[size]{}};
    }

    template <typename... Args>
    requires (!std::is_array_v<T>)
    static auto make_uninitialize() -> Self
    {
        return {new value_type};
    }

    template <typename T2 = T>
    requires std::is_array_v<T>
    static auto make_uninitialize(std::size_t size) -> Self
    {
        return {new value_type[size]};
    }

    template <typename T2>
    auto swap(Owned<T2>& other) -> void
    {
        using std::swap;
        swap(ptr, other.ptr);
    }

    auto get() const noexcept -> pointer
    {
        return ptr;
    }

    auto get_owner() noexcept -> pointer
    {
        auto tmp = ptr;
        ptr = nullptr;
        return tmp;
    }

    template <typename T2 = T>
    requires std::is_array_v<T>
    auto operator [] (std::size_t index) noexcept -> reference
    {
        return ptr[index];
    }

    template <typename T2 = T>
    requires std::is_array_v<T>
    auto operator [] (std::size_t index) const noexcept -> value_type
    {
        return ptr[index];
    }

    auto operator * () const noexcept -> value_type
    {
        return *ptr;
    }

    auto operator -> () const noexcept -> pointer
    {
        return ptr;
    }

    operator bool () const noexcept
    {
        return ptr != nullptr;
    }
};
