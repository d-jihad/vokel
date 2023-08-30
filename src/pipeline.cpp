#include "pipeline.hpp"
#include "mesh.hpp"
#include "render_structs.hpp"
#include "shaders.hpp"
#include <array>

namespace vkInit {

GraphicsPipelineOutBundle createGraphicsPipeline(const GraphicsPipelineInBundle& specification, vk::Pipeline oldPipeline)
{
    vk::GraphicsPipelineCreateInfo pipelineInfo {};
    pipelineInfo.flags = vk::PipelineCreateFlags();

    std::vector<vk::PipelineShaderStageCreateInfo> shadersStages;

    // vertex input
    vk::VertexInputBindingDescription bindingDescription = vkMesh::getPosColorBindingDescription();
    std::array<vk::VertexInputAttributeDescription, 2> attributeDescription = vkMesh::getPosColorAttributeDescriptions();
    vk::PipelineVertexInputStateCreateInfo vertexInputInfo {};
    vertexInputInfo.flags = vk::PipelineVertexInputStateCreateFlags();
    vertexInputInfo.vertexBindingDescriptionCount = 1;
    vertexInputInfo.pVertexBindingDescriptions = &bindingDescription;
    vertexInputInfo.vertexAttributeDescriptionCount = 2;
    vertexInputInfo.pVertexAttributeDescriptions = attributeDescription.data();

    pipelineInfo.pVertexInputState = &vertexInputInfo;

    // input assembly
    vk::PipelineInputAssemblyStateCreateInfo inputAssemblyInfo {};
    inputAssemblyInfo.flags = vk::PipelineInputAssemblyStateCreateFlags();
    inputAssemblyInfo.topology = vk::PrimitiveTopology::eTriangleList;

    pipelineInfo.pInputAssemblyState = &inputAssemblyInfo;

    // vertex shader
    if (DEBUG_MODE) {
        std::cout << "Create vertex shader module\n";
    }

    vk::ShaderModule vertexShader = vkUtil::createShaderModule(specification.vertFilePath, specification.device);

    vk::PipelineShaderStageCreateInfo vertexShaderInfo {};
    vertexShaderInfo = vk::PipelineShaderStageCreateFlags();
    vertexShaderInfo.stage = vk::ShaderStageFlagBits::eVertex;
    vertexShaderInfo.module = vertexShader;
    vertexShaderInfo.pName = "main";

    shadersStages.push_back(vertexShaderInfo);

    // viewport and scissor
    vk::Viewport viewport {
        0.0f,
        0.0f,
        static_cast<float>(specification.swapchainExtent.width),
        static_cast<float>(specification.swapchainExtent.height),
        0.0f,
        1.0f
    };

    vk::Rect2D scissor {
        { 0, 0 },
        { specification.swapchainExtent.width, specification.swapchainExtent.height }
    };

    vk::PipelineViewportStateCreateInfo viewportState {};
    viewportState.flags = vk::PipelineViewportStateCreateFlags();
    viewportState.viewportCount = 1;
    viewportState.pViewports = &viewport;
    viewportState.scissorCount = 1;
    viewportState.pScissors = &scissor;

    pipelineInfo.pViewportState = &viewportState;

    // rasterizer
    vk::PipelineRasterizationStateCreateInfo rasterizer {};
    rasterizer.flags = vk::PipelineRasterizationStateCreateFlags();
    rasterizer.depthClampEnable = VK_FALSE;
    rasterizer.rasterizerDiscardEnable = VK_FALSE;
    rasterizer.polygonMode = vk::PolygonMode::eFill;
    rasterizer.lineWidth = 1.0f;
    rasterizer.cullMode = vk::CullModeFlagBits::eBack;
    rasterizer.frontFace = vk::FrontFace::eClockwise;
    rasterizer.depthBiasEnable = VK_FALSE;

    pipelineInfo.pRasterizationState = &rasterizer;

    // fragment shader
    if (DEBUG_MODE) {
        std::cout << "Create fragment shader module\n";
    }

    vk::ShaderModule fragmentShader = vkUtil::createShaderModule(specification.fragFilePath, specification.device);

    vk::PipelineShaderStageCreateInfo fragmentShaderInfo {};
    fragmentShaderInfo.flags = vk::PipelineShaderStageCreateFlags();
    fragmentShaderInfo.stage = vk::ShaderStageFlagBits::eFragment;
    fragmentShaderInfo.module = fragmentShader;
    fragmentShaderInfo.pName = "main";

    shadersStages.push_back(fragmentShaderInfo);

    pipelineInfo.stageCount = shadersStages.size();
    pipelineInfo.pStages = shadersStages.data();

    // multisampling
    vk::PipelineMultisampleStateCreateInfo multisampling {};
    multisampling.flags = vk::PipelineMultisampleStateCreateFlags();
    multisampling.sampleShadingEnable = VK_FALSE;
    multisampling.rasterizationSamples = vk::SampleCountFlagBits::e1;

    pipelineInfo.pMultisampleState = &multisampling;

    // color blend
    vk::PipelineColorBlendAttachmentState colorBlendAttachment {};
    colorBlendAttachment.colorWriteMask = vk::ColorComponentFlagBits::eR;
    colorBlendAttachment.colorWriteMask |= vk::ColorComponentFlagBits::eG;
    colorBlendAttachment.colorWriteMask |= vk::ColorComponentFlagBits::eB;
    colorBlendAttachment.colorWriteMask |= vk::ColorComponentFlagBits::eA;
    colorBlendAttachment.blendEnable = VK_FALSE;

    vk::PipelineColorBlendStateCreateInfo colorBlending {};
    colorBlending.flags = vk::PipelineColorBlendStateCreateFlags();
    colorBlending.logicOpEnable = VK_FALSE;
    colorBlending.logicOp = vk::LogicOp::eCopy;
    colorBlending.attachmentCount = 1;
    colorBlending.pAttachments = &colorBlendAttachment;
    colorBlending.blendConstants[0] = 0.0f;
    colorBlending.blendConstants[1] = 0.0f;
    colorBlending.blendConstants[2] = 0.0f;
    colorBlending.blendConstants[3] = 0.0f;

    pipelineInfo.pColorBlendState = &colorBlending;

    // pipeline layout
    if (DEBUG_MODE) {
        std::cout << "Creating pipeline layout\n";
    }

    vk::PipelineLayout layout = createPipelineLayout(specification.device);
    pipelineInfo.layout = layout;

    // renderpass
    if (DEBUG_MODE) {
        std::cout << "Creating render pass\n";
    }

    vk::RenderPass renderpass = createRenderPass(specification.device, specification.format);
    pipelineInfo.renderPass = renderpass;

    std::vector<vk::DynamicState> dynamicStates = { vk::DynamicState::eViewport, vk::DynamicState::eScissor };

    vk::PipelineDynamicStateCreateInfo dynamicStateInfo {};
    dynamicStateInfo.flags = vk::PipelineDynamicStateCreateFlags();
    dynamicStateInfo.dynamicStateCount = dynamicStates.size();
    dynamicStateInfo.pDynamicStates = dynamicStates.data();

    pipelineInfo.pDynamicState = &dynamicStateInfo;

    // extra
    pipelineInfo.basePipelineHandle = oldPipeline;

    // creating a graphics pipeline
    if (DEBUG_MODE) {
        std::cout << "Creating Graphics Pipeline\n";
    }

    vk::Pipeline graphicsPipeline;

    try {
        graphicsPipeline = (specification.device.createGraphicsPipeline(nullptr, pipelineInfo)).value;
    } catch (const vk::SystemError& err) {
        std::cout << "Failed to create a Graphics Pipeline: " << err.what() << '\n';
    }

    GraphicsPipelineOutBundle output {};
    output.layout = layout;
    output.renderpass = renderpass;
    output.pipeline = graphicsPipeline;

    specification.device.destroyShaderModule(vertexShader);
    specification.device.destroyShaderModule(fragmentShader);

    return output;
}

vk::PipelineLayout createPipelineLayout(const vk::Device& device)
{
    vk::PipelineLayoutCreateInfo layoutInfo;
    layoutInfo.flags = vk::PipelineLayoutCreateFlags();
    layoutInfo.setLayoutCount = 0;

    layoutInfo.pushConstantRangeCount = 1;
    vk::PushConstantRange pushConstantInfo;
    pushConstantInfo.offset = 0;
    pushConstantInfo.size = sizeof(vkUtil::ObjectData);
    pushConstantInfo.stageFlags = vk::ShaderStageFlagBits::eVertex;
    layoutInfo.pPushConstantRanges = &pushConstantInfo;

    try {
        return device.createPipelineLayout(layoutInfo);
    } catch (const vk::SystemError& err) {
        std::cout << "Failed to create a pipeline layout: " << err.what() << "\n";
    }

    return nullptr;
}

vk::RenderPass createRenderPass(const vk::Device& device, const vk::Format& swapchainImageFormat)
{
    vk::AttachmentDescription colorAttachment {};
    colorAttachment.flags = vk::AttachmentDescriptionFlags();
    colorAttachment.format = swapchainImageFormat;
    colorAttachment.samples = vk::SampleCountFlagBits::e1;
    colorAttachment.loadOp = vk::AttachmentLoadOp::eClear;
    colorAttachment.storeOp = vk::AttachmentStoreOp::eStore;
    colorAttachment.stencilLoadOp = vk::AttachmentLoadOp::eDontCare;
    colorAttachment.stencilStoreOp = vk::AttachmentStoreOp::eDontCare;
    colorAttachment.initialLayout = vk::ImageLayout::eUndefined;
    colorAttachment.finalLayout = vk::ImageLayout::ePresentSrcKHR;

    vk::AttachmentReference colorAttachmentRef {};
    colorAttachmentRef.attachment = 0;
    colorAttachmentRef.layout = vk::ImageLayout::eColorAttachmentOptimal;

    vk::SubpassDescription subpass {};
    subpass.flags = vk::SubpassDescriptionFlags();
    subpass.pipelineBindPoint = vk::PipelineBindPoint::eGraphics;
    subpass.colorAttachmentCount = 1;
    subpass.pColorAttachments = &colorAttachmentRef;

    vk::RenderPassCreateInfo renderpassInfo {};
    renderpassInfo = vk::RenderPassCreateFlags();
    renderpassInfo.attachmentCount = 1;
    renderpassInfo.pAttachments = &colorAttachment;
    renderpassInfo.subpassCount = 1;
    renderpassInfo.pSubpasses = &subpass;

    try {
        return device.createRenderPass(renderpassInfo);
    } catch (const vk::SystemError& err) {
        std::cout << "Failed to create a render pass: " << err.what() << '\n';
    }

    return nullptr;
}

}