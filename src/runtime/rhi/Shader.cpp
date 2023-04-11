#include "Shader.hpp"

NAMESPACE_BEGIN(rhi)

Shader::Shader(VkDevice device, const std::string_view file) : device{device}
{
    const auto info = VkShaderModuleCreateInfo
    {
        .sType    = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO,
        .codeSize = file.size(),
        .pCode    = reinterpret_cast<const std::uint32_t*>(file.data()),
    };
    CHECK_RESULT(vkCreateShaderModule(device, &info, VK_NULL_HANDLE, &handle));
}

Shader::~Shader()
{
    if (handle)
    {
        vkDestroyShaderModule(device, handle, VK_NULL_HANDLE);
    }
}

NAMESPACE_END(rhi)
