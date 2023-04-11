#include "Pipeline.hpp"
#include "Shader.hpp"

NAMESPACE_BEGIN()

auto shader_stage_create_info(VkShaderStageFlagBits flags, VkShaderModule shader) noexcept
    -> VkPipelineShaderStageCreateInfo
{
    return 
    {
        .sType  = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO,
        .stage  = flags,
        .module = shader,
        .pName  = "main",
    };
}

NAMESPACE_END()

NAMESPACE_BEGIN(rhi)

Pipeline::~Pipeline()
{
    if (layout)
    {
        vkDestroyPipelineLayout(device, layout, VK_NULL_HANDLE);
    }

    if (handle)
    {
        vkDestroyPipeline(device, handle, VK_NULL_HANDLE);
    }
}

Pipeline::Pipeline(VkDevice device, const create_info& info) : device{device}
{
    auto pipeline_info = VkGraphicsPipelineCreateInfo
    {
        .sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO,
    };

    const auto vertex_input_state_info = VkPipelineVertexInputStateCreateInfo
    {
        .sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO,
    };
    pipeline_info.pVertexInputState = &vertex_input_state_info;

    const auto vs = Shader{device, info.vertex};
    const auto fs = Shader{device, info.fragment};
    const VkPipelineShaderStageCreateInfo shader_stage_info[]
    {
        shader_stage_create_info(VK_SHADER_STAGE_VERTEX_BIT, vs.handle),
        shader_stage_create_info(VK_SHADER_STAGE_FRAGMENT_BIT, fs.handle),
    };
    pipeline_info.stageCount = std::size(shader_stage_info);
    pipeline_info.pStages    = shader_stage_info;

    const auto input_asm_state_info = VkPipelineInputAssemblyStateCreateInfo
    {
        .sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO,
        .topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST,
        .primitiveRestartEnable = VK_FALSE,
    };
    pipeline_info.pInputAssemblyState = &input_asm_state_info;

    const auto [width, height] = info.target->rect;
    const auto viewport = VkViewport
    {
        .x        = 0,
        .y        = 0,
        .width    = static_cast<f32>(width),
        .height   = static_cast<f32>(height),
        .minDepth = 0,
        .maxDepth = 1,
    };

    const auto scissor = VkRect2D
    {
        .offset = {},
        .extent = {width, height}
    };

    const auto viewport_state_info = VkPipelineViewportStateCreateInfo
    {
        .sType         = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO,
        .viewportCount = 1,
        .pViewports    = &viewport,
        .scissorCount  = 1,
        .pScissors     = &scissor,
    };
    pipeline_info.pViewportState = &viewport_state_info;

    const auto rasterizer_state_info = VkPipelineRasterizationStateCreateInfo
    {
        .sType       = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO,
        .polygonMode = VK_POLYGON_MODE_FILL,
        .cullMode    = VK_CULL_MODE_NONE,
        .frontFace   = VK_FRONT_FACE_CLOCKWISE,
        .lineWidth   = 1,
    };
    pipeline_info.pRasterizationState = &rasterizer_state_info;

    const auto multismple_state_info = VkPipelineMultisampleStateCreateInfo
    {
        .sType                = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO,
        .rasterizationSamples = VK_SAMPLE_COUNT_1_BIT,
        .sampleShadingEnable  = VK_FALSE,
    };
    pipeline_info.pMultisampleState = &multismple_state_info;

    const auto color_blend_attachment_state = VkPipelineColorBlendAttachmentState 
    {
        .blendEnable = VK_FALSE,
        .colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT,
    };

    const auto depth_stencil_state = VkPipelineDepthStencilStateCreateInfo
    {
        .sType           = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO,
        .depthTestEnable = VK_FALSE,
    };
    pipeline_info.pDepthStencilState = &depth_stencil_state;

    const auto color_blend_state_info = VkPipelineColorBlendStateCreateInfo  
    {
        .sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO,
        .logicOpEnable = VK_FALSE,
        .logicOp = VK_LOGIC_OP_COPY,
        .attachmentCount = 1,
        .pAttachments = &color_blend_attachment_state,
    };
    pipeline_info.pColorBlendState = &color_blend_state_info;

    const VkDynamicState dynamic_states[]
    {
        VK_DYNAMIC_STATE_VIEWPORT,
        VK_DYNAMIC_STATE_SCISSOR,
    };

    const auto dynamic_state_info = VkPipelineDynamicStateCreateInfo
    {
        .sType             = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO,
        .dynamicStateCount = std::size(dynamic_states),
        .pDynamicStates    = dynamic_states,
    };
    pipeline_info.pDynamicState = &dynamic_state_info;
    // dynamic rendering
    const auto rendering_info = VkPipelineRenderingCreateInfo
    {
        .sType                   = VK_STRUCTURE_TYPE_PIPELINE_RENDERING_CREATE_INFO_KHR,
        .colorAttachmentCount    = 1,
        .pColorAttachmentFormats = &info.target->format,
    };
    pipeline_info.pNext = &rendering_info;

    const auto pipeline_layout_info = VkPipelineLayoutCreateInfo
    {
        .sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO,
    };
    CHECK_RESULT(vkCreatePipelineLayout(device, &pipeline_layout_info, nullptr, &layout));
    pipeline_info.layout = layout;

    CHECK_RESULT(vkCreateGraphicsPipelines(
        device, VK_NULL_HANDLE,
        1, &pipeline_info,
        VK_NULL_HANDLE, &handle));
}

NAMESPACE_END(rhi)
