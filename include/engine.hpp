#pragma once

#include "swapchain.hpp"
#include "window.hpp"
#include <stdint.h>
#include <vulkan/vulkan_handles.hpp>

namespace VoKel {

class Engine {
public:
    Engine(uint32_t width, uint32_t height, Window& window);
    ~Engine();

    void render();

private:
    uint32_t width, height;
    Window& window;

    // vulkan instance related handles
    vk::Instance instance { nullptr };
    vk::DebugUtilsMessengerEXT debugMessenger { nullptr };
    vk::DispatchLoaderDynamic dldy;
    vk::SurfaceKHR surface;

    // vulkan device
    vk::PhysicalDevice physicalDevice { nullptr };
    vk::Device device { nullptr };
    vk::Queue graphicsQueue { nullptr };
    vk::Queue presentQueue { nullptr };
    vk::SwapchainKHR swapchain;
    std::vector<vkInit::SwapchainFrame> swapchainFrames;
    vk::Format swapchainFormat;
    vk::Extent2D swapchainExtent;

    // pipeline-related variables
    vk::PipelineLayout layout;
    vk::RenderPass renderpass;
    vk::Pipeline pipeline;

    // command-related variables
    vk::CommandPool commandPool;
    vk::CommandBuffer mainCommandBuffer;

    // synchronization-related variables
    vk::Semaphore imageAvailable, renderFinished;
    vk::Fence inFlightFence;

    void createInstance();
    void createDevice();
    void createPipeline();

    void finalizeSetup();

    void recordDrawCommands(const vk::CommandBuffer& commandBuffer, uint32_t imageIndex);
};
} // namespace VoKel
