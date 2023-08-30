#include "engine.hpp"
#include "commands.hpp"
#include "config.hpp"
#include "device.hpp"
#include "framebuffer.hpp"
#include "instance.hpp"
#include "logging.hpp"
#include "pipeline.hpp"
#include "render_structs.hpp"
#include "scene.hpp"
#include "swapchain.hpp"
#include "sync.hpp"
#include "triangle_mesh.hpp"
#include "window.hpp"

#include <array>
#include <iostream>
#include <stdint.h>
#include <tuple>
#include <vulkan/vulkan.hpp>
#include <vulkan/vulkan_handles.hpp>

namespace VoKel {

Engine::Engine(int width, int height, Window& window)
    : width { width }
    , height { height }
    , window { window }
{
    createInstance();
    createDevice();
    createPipeline();
    finalizeSetup();
    createAssets();
}

Engine::~Engine()
{
    device.waitIdle();

    device.destroyCommandPool(commandPool);

    device.destroyRenderPass(renderpass);
    device.destroyPipelineLayout(layout);
    device.destroyPipeline(pipeline);

    cleanupSwapchain();

    delete triangleMesh;

    device.destroy();

    instance.destroySurfaceKHR(surface);

    if (DEBUG_MODE) {
        instance.destroyDebugUtilsMessengerEXT(debugMessenger, nullptr, dldy);
    }

    instance.destroy();
}

void Engine::cleanupSwapchain()
{
    for (auto& frame : swapchainFrames) {
        device.destroyFence(frame.inFlight);
        device.destroySemaphore(frame.imageAvailable);
        device.destroySemaphore(frame.renderFinished);
        device.destroyImageView(frame.imageView);
        device.destroyFramebuffer(frame.framebuffer);
    }

    device.destroySwapchainKHR(swapchain);
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

    createSwapchain();
    frameNumber = 0;
}

void Engine::createSwapchain()
{
    vkInit::SwapchainBundle bundle = vkInit::createSwapchain(device, physicalDevice, surface, width, height);
    swapchain = bundle.swapchain;
    swapchainFormat = bundle.format;
    swapchainFrames = bundle.frames;
    swapchainExtent = bundle.extent;
    maxFrameInFlight = static_cast<int>(swapchainFrames.size());
}

void Engine::recreateSwapchain()
{
    while (window.isMinimized()) {
        window.processInput();
    }

    device.waitIdle();

    cleanupSwapchain();
    createSwapchain();
    createFramebuffers();
    createFrameSyncObj();

    vkInit::commandBufferInputChunk commandBufferInput { device, commandPool, swapchainFrames };
    vkInit::createFrameCommandBuffer(commandBufferInput);
}

void Engine::createPipeline()
{
    vkInit::GraphicsPipelineInBundle specification {};
    specification.device = device;
    specification.vertFilePath = "../../shaders/bin/main.vert.spv";
    specification.fragFilePath = "../../shaders/bin/main.frag.spv";
    specification.swapchainExtent = swapchainExtent;
    specification.format = swapchainFormat;

    vkInit::GraphicsPipelineOutBundle output = vkInit::createGraphicsPipeline(specification, pipeline);
    layout = output.layout;
    renderpass = output.renderpass;
    pipeline = output.pipeline;
}

void Engine::createFramebuffers()
{
    vkInit::framebufferInput framebufferInput {};
    framebufferInput.device = device;
    framebufferInput.renderpass = renderpass;
    framebufferInput.swapchainExtent = swapchainExtent;

    vkInit::createFramebuffer(framebufferInput, swapchainFrames);
}

void Engine::createFrameSyncObj()
{
    for (auto& frame : swapchainFrames) {
        frame.inFlight = vkInit::createFence(device);
        frame.imageAvailable = vkInit::createSemaphore(device);
        frame.renderFinished = vkInit::createSemaphore(device);
    }
}

void Engine::finalizeSetup()
{
    createFramebuffers();
    commandPool = vkInit::createCommandPool(device, physicalDevice, surface);

    vkInit::commandBufferInputChunk commandBufferInput { device, commandPool, swapchainFrames };
    mainCommandBuffer = vkInit::createCommandBuffer(commandBufferInput);
    vkInit::createFrameCommandBuffer(commandBufferInput);

    createFrameSyncObj();
}

void Engine::createAssets()
{
    triangleMesh = new TriangleMesh(device, physicalDevice);
}

void Engine::prepareScene(vk::CommandBuffer commandBuffer)
{
    vk::Buffer vertexBuffer[] = { triangleMesh->buffer.buffer };
    vk::DeviceSize offsets[] = { 0 };
    commandBuffer.bindVertexBuffers(0, 1, vertexBuffer, offsets);
}

void Engine::recordDrawCommands(const vk::CommandBuffer& commandBuffer, uint32_t imageIndex, const Scene& scene)
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

    vk::Viewport viewport {};
    viewport.x = 0.0f;
    viewport.y = 0.0f;
    viewport.width = (float)swapchainExtent.width;
    viewport.height = (float)swapchainExtent.height;
    viewport.minDepth = 0.0f;
    viewport.maxDepth = 1.0f;
    commandBuffer.setViewport(0, viewport);

    vk::Rect2D scissor {};
    scissor.setOffset({ 0, 0 });
    scissor.extent = swapchainExtent;
    commandBuffer.setScissor(0, scissor);

    prepareScene(commandBuffer);

    for (auto& position : scene.trianglePositions) {
        glm::mat4 model = glm::translate(glm::mat4 { 1.0f }, position);
        vkUtil::ObjectData objectData;
        objectData.model = model;
        commandBuffer.pushConstants(layout, vk::ShaderStageFlagBits::eVertex, 0, sizeof(objectData), &objectData);
        commandBuffer.draw(3, 1, 0, 0);
    }

    commandBuffer.endRenderPass();

    try {
        commandBuffer.end();
    } catch (const vk::SystemError& err) {
        if (DEBUG_MODE) {
            std::cout << "Failed to finish recording command buffer\n";
        }
    }
}

void Engine::render(const Scene& scene)
{
    if (device.waitForFences(1, &swapchainFrames[frameNumber].inFlight, VK_TRUE, UINT64_MAX) != vk::Result::eSuccess) {
        if (DEBUG_MODE) {
            std::cout << "failed on waitForFences\n";
        }
    }

    uint32_t imageIndex;

    try {
        vk::ResultValue acquire = device.acquireNextImageKHR(swapchain, UINT64_MAX, swapchainFrames[frameNumber].imageAvailable, nullptr);
        imageIndex = acquire.value;

    } catch (const vk::OutOfDateKHRError& err) {
        recreateSwapchain();
        return;
    }

    if (device.resetFences(1, &swapchainFrames[frameNumber].inFlight) != vk::Result::eSuccess) {
        if (DEBUG_MODE) {
            std::cout << "failed on resetFences\n";
        }
    }

    vk::CommandBuffer commandBuffer = swapchainFrames[frameNumber].commandBuffer;

    commandBuffer.reset();

    recordDrawCommands(commandBuffer, imageIndex, scene);

    vk::SubmitInfo submitInfo {};
    vk::Semaphore waitSemaphores[] = { swapchainFrames[frameNumber].imageAvailable };
    vk::PipelineStageFlags waitStages[] = { vk::PipelineStageFlagBits::eColorAttachmentOutput };
    submitInfo.waitSemaphoreCount = 1;
    submitInfo.pWaitSemaphores = waitSemaphores;
    submitInfo.pWaitDstStageMask = waitStages;
    submitInfo.commandBufferCount = 1;
    submitInfo.pCommandBuffers = &commandBuffer;
    vk::Semaphore signalSemaphores[] = { swapchainFrames[frameNumber].renderFinished };
    submitInfo.signalSemaphoreCount = 1;
    submitInfo.pSignalSemaphores = signalSemaphores;

    try {
        graphicsQueue.submit(submitInfo, swapchainFrames[frameNumber].inFlight);
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

    vk::Result present;

    try {
        present = presentQueue.presentKHR(presentInfo);
    } catch (const vk::OutOfDateKHRError& error) {
        present = vk::Result::eErrorOutOfDateKHR;
    }

    if (present == vk::Result::eErrorOutOfDateKHR || present == vk::Result::eSuboptimalKHR) {
        recreateSwapchain();
        return;
    }

    frameNumber = (frameNumber + 1) % maxFrameInFlight;
}

} // namespace VoKel