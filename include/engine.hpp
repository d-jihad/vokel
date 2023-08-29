#pragma once

#include "render_structs.hpp"
#include "scene.hpp"
#include "swapchain.hpp"
#include "window.hpp"

#include <stdint.h>

namespace VoKel {

class Engine {
public:
    Engine(uint32_t width, uint32_t height, Window& window);
    ~Engine();

    void render(const Scene& scene);

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
    int maxFrameInFlight, frameNumber;

    void createInstance();
    void createDevice();
    void createSwapchain();
    void recreateSwapchain();
    void createPipeline();

    void finalizeSetup();
    void createFramebuffers();
    void createFrameSyncObj();

    void recordDrawCommands(const vk::CommandBuffer& commandBuffer, uint32_t imageIndex, const Scene& scene);

    void cleanupSwapchain();
};
} // namespace VoKel
