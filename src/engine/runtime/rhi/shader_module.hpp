#pragma once

#ifdef USE_MODULES
import <string_view>;
#else
#include <string_view>
#endif

#include <volk.h>

#include <runtime/helper/basic.hpp>
#include "vk_create_info.hpp"

NAMESPACE_BEGIN(runtime)
NAMESPACE_BEGIN(rhi)

struct shader_module
{
private:
    using Self = shader_module;
    using reference = Self&;

private:
    VkShaderModule self = VK_NULL_HANDLE;
    VkDevice device = VK_NULL_HANDLE;

public:
    shader_module(VkDevice device) : device(device) {}

    shader_module(const shader_module&) = delete;
    shader_module& operator=(const shader_module&) = delete;

    shader_module(shader_module&& Module) noexcept
        : self(Module.self), device(Module.device)
    {
        Module.self = VK_NULL_HANDLE;
    }

    auto operator = (shader_module&& Module) noexcept -> reference
    {
        shader_module(std::move(Module)).swap(*this);
        return *this;
    }

    ~shader_module()
    {
        destory();
    }

    auto create(std::string_view view) -> VkResult
    {
        return create(reinterpret_cast<const uint32_t*>(view.data()), view.size());
    }

    auto create(const uint32_t* pCode, const size_t codeSize) -> VkResult
    {
        const auto info = vk_create_info::shader_module(pCode, codeSize);
        return vkCreateShaderModule(device, &info, nullptr, &self);
    }

    auto destory() -> void
    {
        if (self != VK_NULL_HANDLE)
        {
            vkDestroyShaderModule(device, self, nullptr);
            self = VK_NULL_HANDLE;
        }
    }

    auto swap(shader_module& Module) noexcept -> void
    {
        using std::swap;
        swap(self, Module.self);
    }

    auto get() const -> VkShaderModule
    {
        return self;
    }
};

NAMESPACE_END(rhi)
NAMESPACE_END(runtime)
