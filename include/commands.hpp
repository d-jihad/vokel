#pragma once
#include "config.hpp"
#include "device.hpp"
#include "swapchain.hpp"

namespace vkInit {

struct commandBufferInputChunk {
    vk::Device device;
    vk::CommandPool commandPool;
    std::vector<vkInit::SwapchainFrame>& frames;
};

vk::CommandPool createCommandPool(const vk::Device& device, const vk::PhysicalDevice& physicalDevice, const vk::SurfaceKHR& surface);

vk::CommandBuffer createCommandBuffer(commandBufferInputChunk inputChunk);

}
