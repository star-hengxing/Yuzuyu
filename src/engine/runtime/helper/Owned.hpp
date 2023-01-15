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
    using value_type = T;
    using pointer = T*;
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
    Owned(Owned<T2>&& Owned) noexcept : ptr(Owned.ptr)
    {
        Owned.ptr = nullptr;
    }

    template <typename T2>
    auto operator = (Owned<T2>&& Owned) noexcept -> Self&
    {
        Owned(std::move(Owned)).swap(*this);
        return *this;
    }

    ~Owned()
    {
        if(ptr != nullptr)
        {
            delete ptr;
            ptr = nullptr;
        }
    }

    template <typename... Args>
    static auto make(Args&&... args) -> Self
    {
        return {new T{std::forward<Args>(args)...}};
    }

    template <typename T2>
    auto swap(Owned<T2>& Owned) -> void
    {
        using std::swap;
        swap(ptr, Owned.ptr);
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

    auto operator * () const noexcept -> value_type
    {
        return *get();
    }

    auto operator -> () const noexcept -> pointer
    {
        return get();
    }

    operator bool () const noexcept
    {
        return ptr != nullptr;
    }
};
