#pragma once

#include "config.hpp"
#include <vulkan/vulkan_handles.hpp>

namespace vkInit {
struct SwapchainSupportDetails {
    vk::SurfaceCapabilitiesKHR capabilities;
    std::vector<vk::SurfaceFormatKHR> formats;
    std::vector<vk::PresentModeKHR> presentModes;
};

struct SwapchainFrame {
    vk::Image image;
    vk::ImageView imageView;
    vk::Framebuffer framebuffer;
    vk::CommandBuffer commandBuffer;
    vk::Semaphore imageAvailable, renderFinished;
    vk::Fence inFlight;
};

struct SwapchainBundle {
    vk::SwapchainKHR swapchain;
    std::vector<SwapchainFrame> frames;
    vk::Format format;
    vk::Extent2D extent;
};

SwapchainSupportDetails querySwapchainSupport(const vk::PhysicalDevice& physicalDevice, const vk::SurfaceKHR& surface);

vk::SurfaceFormatKHR chooseSwapchainSurfaceFormat(const std::vector<vk::SurfaceFormatKHR>& formats);

vk::PresentModeKHR chooseSwapchainPresentMode(const std::vector<vk::PresentModeKHR>& presentModes);

SwapchainBundle createSwapchain(const vk::Device& device, const vk::PhysicalDevice& physicalDevice, const vk::SurfaceKHR& surface, const uint32_t width, const uint32_t height);

}