#include "memory.hpp"

namespace vkUtil {

uint32_t findMemoryTypeIndex(vk::PhysicalDevice physicalDevice, uint32_t supportedMemoryIndices, vk::MemoryPropertyFlags requestedProperties)
{
    vk::PhysicalDeviceMemoryProperties memoryProperties = physicalDevice.getMemoryProperties();

    for (uint32_t i { 0 }; i < memoryProperties.memoryTypeCount; i++) {
        bool supported = supportedMemoryIndices & (1 << i);
        bool sufficient = (memoryProperties.memoryTypes[i].propertyFlags & requestedProperties) == requestedProperties;

        if (sufficient && supported) {
            return i;
        }
    }

    return 0;
}

void allocateBufferMemory(Buffer& buffer, const BufferInput& input)
{
    vk::MemoryRequirements memoryRequirements = input.device.getBufferMemoryRequirements(buffer.buffer);
    vk::MemoryAllocateInfo allocInfo;
    allocInfo.allocationSize = memoryRequirements.size;
    allocInfo.memoryTypeIndex = findMemoryTypeIndex(
        input.physicalDevice,
        memoryRequirements.memoryTypeBits,
        vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent);

    buffer.bufferMemory = input.device.allocateMemory(allocInfo);
    input.device.bindBufferMemory(buffer.buffer, buffer.bufferMemory, 0);
}

Buffer createBuffer(BufferInput input)
{
    vk::BufferCreateInfo bufferInfo {};
    bufferInfo.flags = vk::BufferCreateFlags();
    bufferInfo.size = input.size;
    bufferInfo.usage = input.usage;
    bufferInfo.sharingMode = vk::SharingMode::eExclusive;

    Buffer buffer;
    buffer.buffer = input.device.createBuffer(bufferInfo);

    allocateBufferMemory(buffer, input);

    return buffer;
}

}