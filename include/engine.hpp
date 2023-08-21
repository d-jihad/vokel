#pragma once

#include "swapchain.hpp"
#include "window.hpp"

namespace VoKel {

class Engine {
public:
    Engine();
    ~Engine();

    void run();

private:
    uint32_t width { 680 }, height { 680 };
    Window window;

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

    void createInstance();
    void createDevice();
    void createPipeline();
};
} // namespace VoKel
