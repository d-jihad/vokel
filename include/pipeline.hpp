#pragma once

#include "config.hpp"

namespace vkInit {

struct GraphicsPipelineInBundle {
    vk::Device device;
    std::string vertFilePath;
    std::string fragFilePath;
    vk::Extent2D swapchainExtent;
    vk::Format format;
};

struct GraphicsPipelineOutBundle {
    vk::PipelineLayout layout;
    vk::RenderPass renderpass;
    vk::Pipeline pipeline;
};

GraphicsPipelineOutBundle createGraphicsPipeline(const GraphicsPipelineInBundle& specification, vk::Pipeline oldPipeline = nullptr);

vk::PipelineLayout createPipelineLayout(const vk::Device& device);

vk::RenderPass createRenderPass(const vk::Device& device, const vk::Format& swapchainImageFormat);

}