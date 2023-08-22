#include "engine.hpp"
#include "commands.hpp"
#include "config.hpp"
#include "device.hpp"
#include "framebuffer.hpp"
#include "instance.hpp"
#include "logging.hpp"
#include "pipeline.hpp"
#include "swapchain.hpp"
#include "sync.hpp"
#include "window.hpp"

#include <array>
#include <stdint.h>
#include <tuple>

namespace VoKel {

Engine::Engine(uint32_t width, uint32_t height, Window& window)
    : width { width }
    , height { height }
    , window { window }
{
    createInstance();
    createDevice();
    createPipeline();
    finalizeSetup();
}

Engine::~Engine()
{
    device.waitIdle();

    device.destroyFence(inFlightFence);

    device.destroySemaphore(imageAvailable);
    device.destroySemaphore(renderFinished);

    device.destroyCommandPool(commandPool);

    device.destroyRenderPass(renderpass);
    device.destroyPipelineLayout(layout);
    device.destroyPipeline(pipeline);

    for (const auto frame : swapchainFrames) {
        device.destroyImageView(frame.imageView);
        device.destroyFramebuffer(frame.framebuffer);
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

void Engine::finalizeSetup()
{
    vkInit::framebufferInput framebufferInput {};
    framebufferInput.device = device;
    framebufferInput.renderpass = renderpass;
    framebufferInput.swapchainExtent = swapchainExtent;

    vkInit::createFramebuffer(framebufferInput, swapchainFrames);
    commandPool = vkInit::createCommandPool(device, physicalDevice, surface);
    vkInit::commandBufferInputChunk commandBufferInput { device, commandPool, swapchainFrames };
    mainCommandBuffer = vkInit::createCommandBuffer(commandBufferInput);

    inFlightFence = vkInit::createFence(device);
    imageAvailable = vkInit::createSemaphore(device);
    renderFinished = vkInit::createSemaphore(device);
}

void Engine::recordDrawCommands(const vk::CommandBuffer& commandBuffer, uint32_t imageIndex)
{
    vk::CommandBufferBeginInfo beginInfo {};
    try {
        commandBuffer.begin(beginInfo);
    } catch (const vk::SystemError& err) {
        if (DEBUG_MODE) {
            std::cout << "Failed to recording command buffer\n";
        }
    }

    vk::RenderPassBeginInfo renderPassInfo {};
    renderPassInfo.renderPass = renderpass;
    renderPassInfo.framebuffer = swapchainFrames[imageIndex].framebuffer;
    renderPassInfo.renderArea.offset.x = 0;
    renderPassInfo.renderArea.offset.y = 0;
    renderPassInfo.renderArea.extent = swapchainExtent;

    vk::ClearValue clearColor { std::array<float, 4> { 0.1f, 0.1f, 0.1f, 1.0f } };
    renderPassInfo.clearValueCount = 1;
    renderPassInfo.pClearValues = &clearColor;

    commandBuffer.beginRenderPass(&renderPassInfo, vk::SubpassContents::eInline);

    commandBuffer.bindPipeline(vk::PipelineBindPoint::eGraphics, pipeline);

    commandBuffer.draw(3, 1, 0, 0);

    commandBuffer.endRenderPass();

    try {
        commandBuffer.end();
    } catch (const vk::SystemError& err) {
        if (DEBUG_MODE) {
            std::cout << "Failed to finish recording command buffer\n";
        }
    }
}

void Engine::render()
{
    device.waitForFences(1, &inFlightFence, VK_TRUE, UINT64_MAX);

    device.resetFences(1, &inFlightFence);

    uint32_t imageIndex = device.acquireNextImageKHR(swapchain, UINT64_MAX, imageAvailable, nullptr).value;

    vk::CommandBuffer commandBuffer = swapchainFrames[imageIndex].commandBuffer;

    commandBuffer.reset();

    recordDrawCommands(commandBuffer, imageIndex);

    vk::SubmitInfo submitInfo {};
    vk::Semaphore waitSemaphores[] = { imageAvailable };
    vk::PipelineStageFlags waitStages[] = { vk::PipelineStageFlagBits::eColorAttachmentOutput };
    submitInfo.waitSemaphoreCount = 1;
    submitInfo.pWaitSemaphores = waitSemaphores;
    submitInfo.pWaitDstStageMask = waitStages;
    submitInfo.commandBufferCount = 1;
    submitInfo.pCommandBuffers = &commandBuffer;
    vk::Semaphore signalSemaphores[] = { renderFinished };
    submitInfo.signalSemaphoreCount = 1;
    submitInfo.pSignalSemaphores = signalSemaphores;

    try {
        graphicsQueue.submit(submitInfo, inFlightFence);
    } catch (const vk::SystemError& err) {
        if (DEBUG_MODE) {
            std::cout << "Failed to submit draw command buffer\n";
        }
    }

    vk::PresentInfoKHR presentInfo {};
    presentInfo.waitSemaphoreCount = 1;
    presentInfo.pWaitSemaphores = signalSemaphores;
    vk::SwapchainKHR swapchains[] = { swapchain };
    presentInfo.swapchainCount = 1;
    presentInfo.pSwapchains = swapchains;
    presentInfo.pImageIndices = &imageIndex;

    presentQueue.presentKHR(presentInfo);
}

} // namespace VoKel