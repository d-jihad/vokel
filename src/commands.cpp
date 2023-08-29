#include "commands.hpp"

#include "device.hpp"

#include <iostream>
#include <stdexcept>
#include <string>
#include <vulkan/vulkan_handles.hpp>

namespace vkInit {

vk::CommandPool createCommandPool(const vk::Device& device, const vk::PhysicalDevice& physicalDevice, const vk::SurfaceKHR& surface)
{
    vkInit::QueueFamilyIndices queueFamilyIndices = vkInit::findQueueFamilies(physicalDevice, surface);

    vk::CommandPoolCreateInfo poolInfo {};
    poolInfo.flags = vk::CommandPoolCreateFlags() | vk::CommandPoolCreateFlagBits::eResetCommandBuffer;
    poolInfo.queueFamilyIndex = queueFamilyIndices.graphicsFamily.value();

    try {
        return device.createCommandPool(poolInfo);
    } catch (const vk::SystemError& err) {
        std::runtime_error { std::string("Failed to create command pool: ") + err.what() };
    }

    return nullptr;
}

vk::CommandBuffer createCommandBuffer(commandBufferInputChunk inputChunk)
{
    vk::CommandBufferAllocateInfo allocInfo {};
    allocInfo.commandPool = inputChunk.commandPool;
    allocInfo.level = vk::CommandBufferLevel::ePrimary;
    allocInfo.commandBufferCount = 1;

    try {
        vk::CommandBuffer commandBuffer = inputChunk.device.allocateCommandBuffers(allocInfo)[0];

        if (DEBUG_MODE) {
            std::cout << "Successfully created the main command buffer\n";
        }

        return commandBuffer;

    } catch (const vk::SystemError& err) {
        throw std::runtime_error { "Failed to allocate main command buffer" };
    }

    return nullptr;
}

void createFrameCommandBuffer(commandBufferInputChunk inputChunk)
{
    vk::CommandBufferAllocateInfo allocInfo {};
    allocInfo.commandPool = inputChunk.commandPool;
    allocInfo.level = vk::CommandBufferLevel::ePrimary;
    allocInfo.commandBufferCount = 1;

    for (size_t i { 0 }; i < inputChunk.frames.size(); i++) {
        try {
            inputChunk.frames[i].commandBuffer = inputChunk.device.allocateCommandBuffers(allocInfo)[0];
            if (DEBUG_MODE) {
                std::cout << "Allocated command buffer info for frame " << i << '\n';
            }
        } catch (const vk::SystemError& err) {
            throw std::runtime_error { "Failed to create frame buffer" };
        }
    }
}

}