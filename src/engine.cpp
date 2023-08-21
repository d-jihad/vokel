#include "engine.hpp"
#include "device.hpp"
#include "instance.hpp"
#include "logging.hpp"
#include "pipeline.hpp"
#include "swapchain.hpp"
#include "window.hpp"

#include <tuple>

namespace VoKel {

Engine::Engine()
    : window { "VoKel Engine", width, height }
{
    createInstance();
    createDevice();
    createPipeline();
}

Engine::~Engine()
{
    device.destroyRenderPass(renderpass);
    device.destroyPipelineLayout(layout);
    device.destroyPipeline(pipeline);

    for (const auto frame : swapchainFrames) {
        device.destroyImageView(frame.imageView);
    }

    device.destroySwapchainKHR(swapchain);
    device.destroy();

    instance.destroySurfaceKHR(surface);

    if (DEBUG_MODE) {
        instance.destroyDebugUtilsMessengerEXT(debugMessenger, nullptr, dldy);
    }

    instance.destroy();
}

void Engine::createInstance()
{
    auto requiredExtensions = window.getVulkanRequiredExtensions();
    std::vector<const char*> requiredLayers {};

    if (DEBUG_MODE) {
        requiredExtensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
        requiredLayers.push_back("VK_LAYER_KHRONOS_validation");
    }

    if (false == vkInit::supported(requiredExtensions, requiredLayers)) {
        throw std::runtime_error { "Some required extensions/layers are not supported by the device" };
    }

    instance = vkInit::createInstance("VoKel Engine", requiredExtensions, requiredLayers);

    dldy = vk::DispatchLoaderDynamic { instance, vkGetInstanceProcAddr };

    if (DEBUG_MODE) {
        debugMessenger = vkInit::createDebugMessenger(instance, dldy);
    }

    surface = window.createVulkanSurface(instance);
}

void Engine::createDevice()
{
    physicalDevice = vkInit::choosePhysicalDevice(instance);
    device = vkInit::createLogicalDevice(physicalDevice, surface);
    std::tie(graphicsQueue, presentQueue) = vkInit::getQueue(physicalDevice, device, surface);

    vkInit::SwapchainBundle bundle = vkInit::createSwapchain(device, physicalDevice, surface, width, height);
    swapchain = bundle.swapchain;
    swapchainFormat = bundle.format;
    swapchainFrames = bundle.frames;
    swapchainExtent = bundle.extent;
}

void Engine::createPipeline()
{
    vkInit::GraphicsPipelineInBundle specification {};
    specification.device = device;
    specification.vertFilePath = "../../shaders/bin/main.vert.spv";
    specification.fragFilePath = "../../shaders/bin/main.frag.spv";
    specification.swapchainExtent = swapchainExtent;
    specification.format = swapchainFormat;

    vkInit::GraphicsPipelineOutBundle output = vkInit::createGraphicsPipeline(specification);
    layout = output.layout;
    renderpass = output.renderpass;
    pipeline = output.pipeline;
}

void Engine::run()
{
    while (!window.shouldClose()) {
        window.update();
    }
}

} // namespace VoKel