#include "swapchain.hpp"
#include "device.hpp"
#include "logging.hpp"

namespace vkInit {
SwapchainSupportDetails querySwapchainSupport(const vk::PhysicalDevice& physicalDevice, const vk::SurfaceKHR& surface)
{
    SwapchainSupportDetails support;

    support.capabilities = physicalDevice.getSurfaceCapabilitiesKHR(surface);

    if (DEBUG_MODE) {
        std::cout << "Swapchain can support the following surface capabilities:\n";

        std::cout << "\tmin image count: " << support.capabilities.minImageCount << '\n';
        std::cout << "\tmax image count: " << support.capabilities.maxImageCount << '\n';

        std::cout << "\tcurrent extent:\n";

        std::cout << "\t\twidth: " << support.capabilities.currentExtent.width << '\n';
        std::cout << "\t\theight: " << support.capabilities.currentExtent.height << '\n';

        std::cout << "\tmin supported extent:\n";

        std::cout << "\t\twidth: " << support.capabilities.minImageExtent.width << '\n';
        std::cout << "\t\theight: " << support.capabilities.minImageExtent.height << '\n';

        std::cout << "\tmax supported extent:\n";

        std::cout << "\t\twidth: " << support.capabilities.maxImageExtent.width << '\n';
        std::cout << "\t\theight: " << support.capabilities.maxImageExtent.height << '\n';

        std::cout << "\tmax image array layers: " << support.capabilities.maxImageArrayLayers << '\n';

        std::cout << "\tsupported transforms:\n";
        std::vector<std::string> stringList = logTransformBits(support.capabilities.supportedTransforms);
        for (const auto& line : stringList) {
            std::cout << "\t\t" << line << '\n';
        }

        std::cout << "\tcurrent transform:\n";
        stringList = logTransformBits(support.capabilities.currentTransform);
        for (const auto& line : stringList) {
            std::cout << "\t\t" << line << '\n';
        }

        std::cout << "\tsupported alpha operations:\n";
        stringList = logAlphaCompositeBits(support.capabilities.supportedCompositeAlpha);
        for (const auto& line : stringList) {
            std::cout << "\t\t" << line << '\n';
        }

        std::cout << "\tsupported image usage:\n";
        stringList = logImageUsageBits(support.capabilities.supportedUsageFlags);
        for (const auto& line : stringList) {
            std::cout << "\t\t" << line << '\n';
        }

        support.formats = physicalDevice.getSurfaceFormatsKHR(surface);

        if (DEBUG_MODE) {

            for (const auto supportedFormat : support.formats) {
                std::cout << "supported pixel format: " << vk::to_string(supportedFormat.format) << '\n';
                std::cout << "supported color space: " << vk::to_string(supportedFormat.colorSpace) << '\n';
            }
        }

        support.presentModes = physicalDevice.getSurfacePresentModesKHR(surface);

        if (DEBUG_MODE) {
            std::cout << "supported present modes:\n";
            for (const auto& presentMode : support.presentModes) {
                std::cout << "\n\t"
                          << logPresentMode(presentMode) << '\n';
            }
        }
    }

    return support;
}

vk::SurfaceFormatKHR chooseSwapchainSurfaceFormat(const std::vector<vk::SurfaceFormatKHR>& formats)
{
    for (const auto& format : formats) {
        if (format.format == vk::Format::eB8G8R8A8Unorm && format.colorSpace == vk::ColorSpaceKHR::eSrgbNonlinear) {
            return format;
        }
    }

    return formats[0];
}

vk::PresentModeKHR chooseSwapchainPresentMode(const std::vector<vk::PresentModeKHR>& presentModes)
{
    for (const auto& presentMode : presentModes) {
        if (presentMode == vk::PresentModeKHR::eMailbox) {
            return presentMode;
        }
    }

    return vk::PresentModeKHR::eFifo;
}

vk::Extent2D chooseSwapchainExtent(const uint32_t width, const uint32_t height, const vk::SurfaceCapabilitiesKHR& capabilities)
{
    if (capabilities.currentExtent != UINT32_MAX) {
        return capabilities.currentExtent;
    } else {
        vk::Extent2D extent;

        extent.width = std::min(
            capabilities.maxImageExtent.width,
            width);

        extent.height = std::min(
            capabilities.maxImageExtent.height,
            height);

        return extent;
    }
}

SwapchainBundle createSwapchain(const vk::Device& device, const vk::PhysicalDevice& physicalDevice, const vk::SurfaceKHR& surface, const uint32_t width, const uint32_t height)
{
    SwapchainSupportDetails support = querySwapchainSupport(physicalDevice, surface);

    vk::SurfaceFormatKHR format = chooseSwapchainSurfaceFormat(support.formats);
    vk::PresentModeKHR presentMode = chooseSwapchainPresentMode(support.presentModes);
    vk::Extent2D extent = chooseSwapchainExtent(width, height, support.capabilities);

    uint32_t imageCount = std::min(
        support.capabilities.maxImageCount,
        support.capabilities.minImageCount + 1);

    vk::SwapchainCreateInfoKHR createInfo {
        vk::SwapchainCreateFlagsKHR(),
        surface,
        imageCount,
        format.format,
        format.colorSpace,
        extent,
        1,
        vk::ImageUsageFlagBits::eColorAttachment
    };

    QueueFamilyIndices indices = findQueueFamilies(physicalDevice, surface);
    uint32_t queueFamilyIndices[] = { indices.graphicsFamily.value(), indices.presentFamily.value() };

    if (indices.graphicsFamily.value() != indices.presentFamily.value()) {
        createInfo.imageSharingMode = vk::SharingMode::eConcurrent;
        createInfo.queueFamilyIndexCount = 2;
        createInfo.pQueueFamilyIndices = queueFamilyIndices;
    } else {
        createInfo.imageSharingMode = vk::SharingMode::eExclusive;
    }

    createInfo.preTransform = support.capabilities.currentTransform;
    createInfo.compositeAlpha = vk::CompositeAlphaFlagBitsKHR::eOpaque;
    createInfo.presentMode = presentMode;
    createInfo.clipped = VK_TRUE;

    createInfo.oldSwapchain = vk::SwapchainKHR { nullptr };

    SwapchainBundle bundle {};

    try {
        bundle.swapchain = device.createSwapchainKHR(createInfo);
    } catch (vk::SystemError err) {
        throw std::runtime_error { std::string { "Failed to create a swapchain: " } + err.what() };
    }

    std::vector<vk::Image> images = device.getSwapchainImagesKHR(bundle.swapchain);

    bundle.frames.resize(images.size());

    for (size_t i { 0 }; i < images.size(); i++) {

        vk::ImageViewCreateInfo createInfo {};
        createInfo.image = images[i];

        createInfo.viewType = vk::ImageViewType::e2D;

        createInfo.components.r = vk::ComponentSwizzle::eIdentity;
        createInfo.components.g = vk::ComponentSwizzle::eIdentity;
        createInfo.components.b = vk::ComponentSwizzle::eIdentity;
        createInfo.components.a = vk::ComponentSwizzle::eIdentity;

        createInfo.subresourceRange.aspectMask = vk::ImageAspectFlagBits::eColor;
        createInfo.subresourceRange.baseMipLevel = 0;
        createInfo.subresourceRange.levelCount = 1;
        createInfo.subresourceRange.baseArrayLayer = 0;
        createInfo.subresourceRange.layerCount = 1;

        createInfo.format = format.format;

        bundle.frames[i].image = images[i];
        bundle.frames[i].imageView = device.createImageView(createInfo);
    }

    bundle.format = format.format;
    bundle.extent = extent;

    return bundle;
}
}