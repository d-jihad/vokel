#pragma once
#include "config.hpp"

namespace vkInit {
vk::Semaphore createSemaphore(const vk::Device& device);
vk::Fence createFence(const vk::Device& device);
}
