#pragma once

#include "config.hpp"

#include <optional>

namespace vkInit {

struct QueueFamilyIndices {
    std::optional<uint32_t> graphicsFamily;
    std::optional<uint32_t> presentFamily;

    bool isComplete() { return graphicsFamily.has_value() && presentFamily.has_value(); }
};

QueueFamilyIndices findQueueFamilies(const vk::PhysicalDevice& physicalDevice, const vk::SurfaceKHR& surface);

void logPhysicalDeviceProperties(const vk::PhysicalDevice& physicalDevice);

uint32_t ratePhysicalDevice(const vk::PhysicalDevice& physicalDevice);

bool checkDeviceExtensionSupport(const vk::PhysicalDevice& physicalDevice, const std::vector<const char*>& requestedExtensions);

vk::PhysicalDevice choosePhysicalDevice(const vk::Instance& instance);

vk::Device createLogicalDevice(const vk::PhysicalDevice& physicalDevice, const vk::SurfaceKHR& surface);

std::tuple<vk::Queue, vk::Queue> getQueue(const vk::PhysicalDevice& physicalDevice, const vk::Device& device, const vk::SurfaceKHR& surface);

}