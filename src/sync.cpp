#include "sync.hpp"
#include "config.hpp"
#include <stdexcept>

namespace vkInit {

vk::Semaphore createSemaphore(const vk::Device& device)
{
    vk::SemaphoreCreateInfo semaphoreInfo {};
    semaphoreInfo.flags = vk::SemaphoreCreateFlags();

    try {
        return device.createSemaphore(semaphoreInfo);
        if (DEBUG_MODE) {
            std::cout << "Successfully created a semaphore\n";
        }
    } catch (const vk::SystemError& err) {
        throw std::runtime_error { "Failed to create semaphore" };
    }

    return nullptr;
}

vk::Fence createFence(const vk::Device& device)
{
    vk::FenceCreateInfo fenceInfo {};
    fenceInfo.flags = vk::FenceCreateFlags() | vk::FenceCreateFlagBits::eSignaled;

    try {
        return device.createFence(fenceInfo);
    } catch (const vk::SystemError& err) {
        throw std::runtime_error { "Failed to create fence" };
    }

    return nullptr;
}

}