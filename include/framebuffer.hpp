#pragma once
#include "config.hpp"
#include "swapchain.hpp"
#include <vector>

namespace vkInit {

struct framebufferInput {
    vk::Device device;
    vk::RenderPass renderpass;
    vk::Extent2D swapchainExtent;
};

void createFramebuffer(framebufferInput inputChunk, std::vector<vkInit::SwapchainFrame>& frames);

}