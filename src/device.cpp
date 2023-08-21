#include "device.hpp"
#include "logging.hpp"

#include <set>
#include <tuple>

namespace vkInit {

bool checkDeviceExtensionSupport(const vk::PhysicalDevice& physicalDevice, const std::vector<const char*>& requestedExtensions)
{
    // we need transform const char* on string, because the erase method use == operator and that doesn't work with const char*
    std::set<std::string> requiredExtensions(requestedExtensions.begin(), requestedExtensions.end());

    if (DEBUG_MODE) {
        std::cout << "\t\tDevice can support the following extensions:\n";
    }

    for (const auto& extension : physicalDevice.enumerateDeviceExtensionProperties()) {
        if (DEBUG_MODE) {
            std::cout << "\t\t\t\"" << extension.extensionName << "\"\n";
        }

        requiredExtensions.erase(extension.extensionName);
    }

    if (DEBUG_MODE) {
        if (!requiredExtensions.empty()) {
            std::cout << "\t\tSome required device extensions are missing:\n";
            for (const auto& extension : requiredExtensions) {
                std::cout << "\t\t\t\"" << extension << "\"\n";
            }
        }
    }

    return requiredExtensions.empty();
}

uint32_t ratePhysicalDevice(const vk::PhysicalDevice& physicalDevice)
{
    uint32_t score { 1 };

    auto properties = physicalDevice.getProperties();
    auto features = physicalDevice.getFeatures();

    if (!features.geometryShader) {
        return 0;
    }

    if (properties.deviceType == vk::PhysicalDeviceType::eDiscreteGpu) {
        score += 1000;
    }

    score += properties.limits.maxImageDimension2D;

    return score;
}

vk::PhysicalDevice choosePhysicalDevice(const vk::Instance& instance)
{
    auto availableDevices = instance.enumeratePhysicalDevices();

    if (availableDevices.empty()) {
        throw std::runtime_error { "Cannot find any compatible device with Vulkan" };
    }

    if (DEBUG_MODE) {
        std::cout << "Choosing a physical device:\n";
        std::cout << "\tThere are " << availableDevices.size() << " compatible devices available\n";
    }

    std::vector<const char*> requestedExtensions {
        VK_KHR_SWAPCHAIN_EXTENSION_NAME
    };

    uint32_t maxScore { 0 };
    size_t bestDeviceIdx { 0 };

    for (size_t i { 0 }; i < availableDevices.size(); i++) {
        if (DEBUG_MODE) {
            logPhysicalDeviceProperties(availableDevices[i]);
        }

        if (!checkDeviceExtensionSupport(availableDevices[i], requestedExtensions)) {
            continue;
        }

        uint32_t deviceScore = ratePhysicalDevice(availableDevices[i]);

        if (deviceScore > maxScore) {
            maxScore = deviceScore;
            bestDeviceIdx = i;
        }
    }

    if (maxScore == 0) {
        throw std::runtime_error { "No suitable device found!" };
    }

    if (DEBUG_MODE) {
        std::cout << "Picked device:\n";
        logPhysicalDeviceProperties(availableDevices[bestDeviceIdx]);
    }

    return availableDevices[bestDeviceIdx];
}

QueueFamilyIndices findQueueFamilies(
    const vk::PhysicalDevice& physicalDevice,
    const vk::SurfaceKHR& surface)
{
    QueueFamilyIndices indices {};

    auto queueFamilies = physicalDevice.getQueueFamilyProperties();

    if (DEBUG_MODE) {
        std::cout << "System can support " << queueFamilies.size() << " queue families\n";
    }

    for (uint32_t i { 0 }; i < queueFamilies.size(); i++) {
        if (queueFamilies[i].queueFlags & vk::QueueFlagBits::eGraphics) {
            indices.graphicsFamily = i;

            if (DEBUG_MODE) {
                std::cout << "Queue family " << i << " is suitable for graphics\n";
            }
        }

        if (physicalDevice.getSurfaceSupportKHR(i, surface)) {
            indices.presentFamily = i;

            if (DEBUG_MODE) {
                std::cout << "Queue family " << i << " is suitable for presenting\n";
            }
        }

        if (indices.isComplete()) {
            break;
        }
    }

    return indices;
}

vk::Device createLogicalDevice(const vk::PhysicalDevice& physicalDevice, const vk::SurfaceKHR& surface)
{
    QueueFamilyIndices indices = findQueueFamilies(physicalDevice, surface);

    std::vector<uint32_t> uniqueIndices { indices.graphicsFamily.value() };

    if (indices.graphicsFamily.value() != indices.presentFamily.value()) {
        uniqueIndices.push_back(indices.presentFamily.value());
    }

    float queuePriority { 1.0f };

    std::vector<vk::DeviceQueueCreateInfo> queueCreateInfos {};

    for (const auto& queueFamilyIndex : uniqueIndices) {

        queueCreateInfos.push_back(vk::DeviceQueueCreateInfo {
            vk::DeviceQueueCreateFlags(),
            queueFamilyIndex,
            1,
            &queuePriority });
    }

    std::vector<const char*> deviceExtensions {
        VK_KHR_SWAPCHAIN_EXTENSION_NAME
    };

    vk::PhysicalDeviceFeatures enabledFeatures
        = vk::PhysicalDeviceFeatures {};
    enabledFeatures.setGeometryShader(true);

    std::vector<const char*> enabledLayers;
    if (DEBUG_MODE) {
        enabledLayers.push_back("VK_LAYER_KHRONOS_validation");
    }

    vk::DeviceCreateInfo createInfo {
        vk::DeviceCreateFlags(),
        static_cast<uint32_t>(queueCreateInfos.size()),
        queueCreateInfos.data(),
        static_cast<uint32_t>(enabledLayers.size()),
        enabledLayers.data(),
        static_cast<uint32_t>(deviceExtensions.size()),
        deviceExtensions.data(),
        &enabledFeatures
    };

    try {
        vk::Device device = physicalDevice.createDevice(createInfo);

        if (DEBUG_MODE) {
            std::cout << "GPU has been successfully abstracted\n";
        }

        return device;

    } catch (vk::SystemError err) {
        if (DEBUG_MODE) {
            std::cout << "Device creation failed\n";
        }
    }

    return nullptr;
}

std::tuple<vk::Queue, vk::Queue> getQueue(const vk::PhysicalDevice& physicalDevice, const vk::Device& device, const vk::SurfaceKHR& surface)
{
    QueueFamilyIndices indices = findQueueFamilies(physicalDevice, surface);

    return {
        device.getQueue(indices.graphicsFamily.value(), 0),
        device.getQueue(indices.presentFamily.value(), 0)
    };
}

}