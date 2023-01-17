#pragma once

#ifdef USE_MODULES
import <source_location>;
#else
#include <source_location>
#endif

#include <volk.h>

#include <runtime/helper/marco.hpp>

auto check_vulkan_result(const char* path, int line, const char* function, VkResult result) -> void;

#define CHECK_RESULT(result) check_vulkan_result( \
    std::source_location::current().file_name(),  \
    std::source_location::current().line(),       \
    std::source_location::current().function_name(), result)

NAMESPACE_BEGIN(vk_create_info)

inline auto command_pool() -> VkCommandPoolCreateInfo
{
    VkCommandPoolCreateInfo info{};
    info.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
    return info;
}

inline auto command_buffer_allocate(
    VkCommandPool commandPool,
    VkCommandBufferLevel level,
    uint32_t count) -> VkCommandBufferAllocateInfo
{
    VkCommandBufferAllocateInfo info{};
    info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    info.commandPool = commandPool;
    info.level = level;
    info.commandBufferCount = count;
    return info;
}

inline auto renderpass() -> VkRenderPassCreateInfo
{
    VkRenderPassCreateInfo info{};
    info.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
    return info;
}

inline auto image() -> VkImageCreateInfo
{
    VkImageCreateInfo info{};
    info.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
    return info;
}

inline auto iamge_view() -> VkImageViewCreateInfo
{
    VkImageViewCreateInfo info{};
    info.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
    return info;
}

inline auto sampler() -> VkSamplerCreateInfo
{
    VkSamplerCreateInfo info{};
    info.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
    info.maxAnisotropy = 1.0f;
    return info;
}

inline auto framebuffer() -> VkFramebufferCreateInfo
{
    VkFramebufferCreateInfo info{};
    info.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
    return info;
}

inline auto semaphore(VkSemaphoreCreateFlags flags = 0) -> VkSemaphoreCreateInfo
{
    VkSemaphoreCreateInfo info{};
    info.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;
    info.flags = flags;
    return info;
}

inline auto fence(VkFenceCreateFlags flags = 0) -> VkFenceCreateInfo
{
    VkFenceCreateInfo info{};
    info.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
    info.flags = flags;
    return info;
}

inline auto event() -> VkEventCreateInfo
{
    VkEventCreateInfo info{};
    info.sType = VK_STRUCTURE_TYPE_EVENT_CREATE_INFO;
    return info;
}

inline auto viewport(
    float width, float height,
    float minDepth, float maxDepth) -> VkViewport
{
    VkViewport viewport{};
    viewport.width = width;
    viewport.height = height;
    viewport.minDepth = minDepth;
    viewport.maxDepth = maxDepth;
    return viewport;
}

inline auto rect2D(
    uint32_t width, uint32_t height,
    int32_t offsetX, int32_t offsetY) -> VkRect2D
{
    VkRect2D rect2D{};
    rect2D.extent.width = width;
    rect2D.extent.height = height;
    rect2D.offset.x = offsetX;
    rect2D.offset.y = offsetY;
    return rect2D;
}

inline auto rect2D(
    VkExtent2D extent,
    int32_t offsetX, int32_t offsetY) -> VkRect2D
{
    VkRect2D rect2D{};
    rect2D.extent = extent;
    rect2D.offset.x = offsetX;
    rect2D.offset.y = offsetY;
    return rect2D;
}

inline auto buffer() -> VkBufferCreateInfo
{
    VkBufferCreateInfo info{};
    info.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
    return info;
}

inline auto buffer(VkBufferUsageFlags usage, VkDeviceSize size)
    -> VkBufferCreateInfo
{
    VkBufferCreateInfo info{};
    info.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
    info.usage = usage;
    info.size = size;
    return info;
}

inline auto descriptor_pool(
    uint32_t poolSizeCount,
    VkDescriptorPoolSize* pPoolSizes,
    uint32_t maxSets) -> VkDescriptorPoolCreateInfo
{
    VkDescriptorPoolCreateInfo info{};
    info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
    info.poolSizeCount = poolSizeCount;
    info.pPoolSizes = pPoolSizes;
    info.maxSets = maxSets;
    return info;
}

inline auto descriptor_pool_size(
    VkDescriptorType type,
    uint32_t descriptorCount) -> VkDescriptorPoolSize
{
    VkDescriptorPoolSize pool_size{};
    pool_size.type = type;
    pool_size.descriptorCount = descriptorCount;
    return pool_size;
}

inline auto descriptorSetLayoutBinding(
    VkDescriptorType type,
    VkShaderStageFlags stageFlags,
    uint32_t binding,
    uint32_t descriptorCount = 1) -> VkDescriptorSetLayoutBinding
{
    VkDescriptorSetLayoutBinding set_layout_binding{};
    set_layout_binding.descriptorType = type;
    set_layout_binding.stageFlags = stageFlags;
    set_layout_binding.binding = binding;
    set_layout_binding.descriptorCount = descriptorCount;
    return set_layout_binding;
}

inline auto descriptor_set_layout(
    const VkDescriptorSetLayoutBinding* pBindings,
    uint32_t bindingCount) -> VkDescriptorSetLayoutCreateInfo
{
    VkDescriptorSetLayoutCreateInfo info{};
    info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
    info.pBindings = pBindings;
    info.bindingCount = bindingCount;
    return info;
}

inline auto pipeline_layout(
    const VkDescriptorSetLayout* pSetLayouts,
    uint32_t setLayoutCount = 1) -> VkPipelineLayoutCreateInfo
{
    VkPipelineLayoutCreateInfo info{};
    info.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
    info.setLayoutCount = setLayoutCount;
    info.pSetLayouts = pSetLayouts;
    return info;
}

inline auto pipeline_layout(uint32_t setLayoutCount = 1)
    -> VkPipelineLayoutCreateInfo
{
    VkPipelineLayoutCreateInfo info{};
    info.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
    info.setLayoutCount = setLayoutCount;
    return info;
}

inline auto descriptor_set_allocate(
    VkDescriptorPool descriptorPool,
    const VkDescriptorSetLayout* pSetLayouts,
    uint32_t descriptorSetCount) -> VkDescriptorSetAllocateInfo
{
    VkDescriptorSetAllocateInfo info{};
    info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
    info.descriptorPool = descriptorPool;
    info.pSetLayouts = pSetLayouts;
    info.descriptorSetCount = descriptorSetCount;
    return info;
}

inline auto descriptor_image(
    VkSampler sampler, VkImageView imageView,
    VkImageLayout imageLayout) -> VkDescriptorImageInfo
{
    VkDescriptorImageInfo info{};
    info.sampler = sampler;
    info.imageView = imageView;
    info.imageLayout = imageLayout;
    return info;
}

inline auto write_descriptor_set(
    VkDescriptorSet dstSet,
    VkDescriptorType type,
    uint32_t binding,
    VkDescriptorBufferInfo* bufferInfo,
    uint32_t descriptorCount = 1) -> VkWriteDescriptorSet
{
    VkWriteDescriptorSet set{};
    set.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
    set.dstSet = dstSet;
    set.descriptorType = type;
    set.dstBinding = binding;
    set.pBufferInfo = bufferInfo;
    set.descriptorCount = descriptorCount;
    return set;
}

inline auto write_descriptor_set(
    VkDescriptorSet dstSet,
    VkDescriptorType type,
    uint32_t binding,
    VkDescriptorImageInfo* imageInfo,
    uint32_t descriptorCount = 1) -> VkWriteDescriptorSet
{
    VkWriteDescriptorSet set{};
    set.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
    set.dstSet = dstSet;
    set.descriptorType = type;
    set.dstBinding = binding;
    set.pImageInfo = imageInfo;
    set.descriptorCount = descriptorCount;
    return set;
}

inline auto vertex_input_binding_description(
    uint32_t binding, uint32_t stride,
    VkVertexInputRate inputRate) -> VkVertexInputBindingDescription
{
    VkVertexInputBindingDescription description{};
    description.binding = binding;
    description.stride = stride;
    description.inputRate = inputRate;
    return description;
}

inline auto vertex_input_attribute_description(
    uint32_t binding, uint32_t location,
    VkFormat format, uint32_t offset)
    -> VkVertexInputAttributeDescription
{
    VkVertexInputAttributeDescription description{};
    description.location = location;
    description.binding = binding;
    description.format = format;
    description.offset = offset;
    return description;
}

inline auto pipeline_vertex_input_state()
    -> VkPipelineVertexInputStateCreateInfo
{
    VkPipelineVertexInputStateCreateInfo info{};
    info.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
    return info;
}

inline auto pipeline_input_assembly_state(
    VkPrimitiveTopology topology,
    VkPipelineInputAssemblyStateCreateFlags flags,
    VkBool32 primitiveRestartEnable) -> VkPipelineInputAssemblyStateCreateInfo
{
    VkPipelineInputAssemblyStateCreateInfo info{};
    info.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
    info.topology = topology;
    info.flags = flags;
    info.primitiveRestartEnable = primitiveRestartEnable;
    return info;
}

inline auto pipeline_rasterization_state(
    VkPolygonMode polygonMode,
    VkCullModeFlags cullMode,
    VkFrontFace frontFace,
    VkPipelineRasterizationStateCreateFlags flags = 0)
    -> VkPipelineRasterizationStateCreateInfo
{
    VkPipelineRasterizationStateCreateInfo info{};
    info.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
    info.polygonMode = polygonMode;
    info.cullMode = cullMode;
    info.frontFace = frontFace;
    info.flags = flags;
    info.depthClampEnable = VK_FALSE;
    info.lineWidth = 1.0f;
    return info;
}

inline auto pipeline_color_blend_attachment_state(
    VkColorComponentFlags colorWriteMask,
    VkBool32 blendEnable) -> VkPipelineColorBlendAttachmentState
{
    VkPipelineColorBlendAttachmentState attachment{};
    attachment.colorWriteMask = colorWriteMask;
    attachment.blendEnable = blendEnable;
    return attachment;
}

inline auto pipeline_color_blend_state(
    uint32_t attachmentCount,
    const VkPipelineColorBlendAttachmentState* pAttachments)
    -> VkPipelineColorBlendStateCreateInfo
{
    VkPipelineColorBlendStateCreateInfo info{};
    info.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
    info.attachmentCount = attachmentCount;
    info.pAttachments = pAttachments;
    return info;
}

inline auto pipeline_depth_stencil_state()
    -> VkPipelineDepthStencilStateCreateInfo
{
    VkPipelineDepthStencilStateCreateInfo info{};
    info.sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
    info.depthTestEnable = VK_FALSE;
    info.depthWriteEnable = VK_FALSE;
    info.depthCompareOp = VK_COMPARE_OP_ALWAYS;
    return info;
}

inline auto pipeline_viewport_state(
    uint32_t viewportCount, uint32_t scissorCount,
    VkPipelineViewportStateCreateFlags flags = 0)
    -> VkPipelineViewportStateCreateInfo
{
    VkPipelineViewportStateCreateInfo info{};
    info.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
    info.viewportCount = viewportCount;
    info.scissorCount = scissorCount;
    info.flags = flags;
    return info;
}

inline auto pipeline_multisample_state(
    VkSampleCountFlagBits rasterizationSamples,
    VkPipelineMultisampleStateCreateFlags flags = 0)
    -> VkPipelineMultisampleStateCreateInfo
{
    VkPipelineMultisampleStateCreateInfo info{};
    info.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
    info.rasterizationSamples = rasterizationSamples;
    info.flags = flags;
    return info;
}

inline auto pipeline_dynamic_state(
    const VkDynamicState* pDynamicStates,
    uint32_t dynamicStateCount,
    VkPipelineDynamicStateCreateFlags flags = 0) -> VkPipelineDynamicStateCreateInfo
{
    VkPipelineDynamicStateCreateInfo info{};
    info.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
    info.pDynamicStates = pDynamicStates;
    info.dynamicStateCount = dynamicStateCount;
    info.flags = flags;
    return info;
}

inline auto pipeline_tessellation_state(uint32_t patchControlPoints)
    -> VkPipelineTessellationStateCreateInfo
{
    VkPipelineTessellationStateCreateInfo info{};
    info.sType = VK_STRUCTURE_TYPE_PIPELINE_TESSELLATION_STATE_CREATE_INFO;
    info.patchControlPoints = patchControlPoints;
    return info;
}

inline auto pipeline(
    VkPipelineLayout layout,
    VkRenderPass renderPass,
    VkPipelineCreateFlags flags = 0) -> VkGraphicsPipelineCreateInfo
{
    VkGraphicsPipelineCreateInfo info{};
    info.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
    info.layout = layout;
    info.renderPass = renderPass;
    info.flags = flags;
    info.basePipelineIndex = -1;
    info.basePipelineHandle = VK_NULL_HANDLE;
    return info;
}

inline auto pipeline() -> VkGraphicsPipelineCreateInfo
{
    VkGraphicsPipelineCreateInfo info{};
    info.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
    info.basePipelineIndex = -1;
    info.basePipelineHandle = VK_NULL_HANDLE;
    return info;
}

inline auto compute_pipeline(
    VkPipelineLayout layout,
    VkPipelineCreateFlags flags = 0) -> VkComputePipelineCreateInfo
{
    VkComputePipelineCreateInfo info{};
    info.sType = VK_STRUCTURE_TYPE_COMPUTE_PIPELINE_CREATE_INFO;
    info.layout = layout;
    info.flags = flags;
    return info;
}

inline auto push_constant_range(
    VkShaderStageFlags stageFlags,
    uint32_t size,
    uint32_t offset) -> VkPushConstantRange
{
    VkPushConstantRange range{};
    range.stageFlags = stageFlags;
    range.offset = offset;
    range.size = size;
    return range;
}

inline auto command_buffer_begin() -> VkCommandBufferBeginInfo
{
    VkCommandBufferBeginInfo info{};
    info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
    return info;
}

inline auto submit(uint32_t commandBufferCount, VkCommandBuffer* pCommandBuffers) -> VkSubmitInfo
{
    VkSubmitInfo info{};
    info.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
    info.commandBufferCount = commandBufferCount;
    info.pCommandBuffers = pCommandBuffers;
    return info;
}

inline auto present() -> VkPresentInfoKHR
{
    VkPresentInfoKHR info{};
    info.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
    return info;
}

inline auto shader_module(const uint32_t* pCode, const size_t codeSize)
    -> VkShaderModuleCreateInfo
{
    VkShaderModuleCreateInfo info{};
    info.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
    info.pCode = pCode;
    info.codeSize = codeSize;
    return info;
}

inline auto pipeline_shader_state(
    VkShaderStageFlagBits stage, VkShaderModule Module)
    -> VkPipelineShaderStageCreateInfo
{
    VkPipelineShaderStageCreateInfo info{};
    info.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
    info.stage = stage;
    info.module = Module;
    info.pName = "main";
    return info;
}

NAMESPACE_END(vk_create_info)
