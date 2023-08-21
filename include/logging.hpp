#pragma once

#include "config.hpp"

namespace vkInit {

vk::DebugUtilsMessengerCreateInfoEXT createDebugMessengerCreateInfo();

VKAPI_ATTR VkBool32 VKAPI_CALL debugCallback(
    VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
    VkDebugUtilsMessageTypeFlagsEXT messageType,
    const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
    void* pUserData);

vk::DebugUtilsMessengerEXT createDebugMessenger(
    const vk::Instance& instance,
    const vk::DispatchLoaderDynamic& dldy);

void logPhysicalDeviceProperties(const vk::PhysicalDevice& physicalDevice);

std::vector<std::string> logTransformBits(vk::SurfaceTransformFlagsKHR bits);

std::vector<std::string> logAlphaCompositeBits(const vk::CompositeAlphaFlagsKHR& bits);

std::vector<std::string> logImageUsageBits(const vk::ImageUsageFlags& bits);

std::string logPresentMode(const vk::PresentModeKHR& presentMode);

}