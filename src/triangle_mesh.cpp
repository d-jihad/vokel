#include "triangle_mesh.hpp"
#include "memory.hpp"

#include <vcruntime_string.h>
#include <vector>

TriangleMesh::TriangleMesh(vk::Device device, vk::PhysicalDevice physicalDevice)
    : device { device }
{

    std::vector<float> vertices = {
        0.0f, -0.05f, 0.0f, 1.0f, 0.0f,
        0.05f, 0.05f, 0.0f, 1.0f, 0.0f,
        -0.05f, 0.05f, 0.0f, 1.0f, 0.0f
    };

    vkUtil::BufferInput bufferInput;
    bufferInput.device = device;
    bufferInput.physicalDevice = physicalDevice;
    bufferInput.size = sizeof(float) * vertices.size();
    bufferInput.usage = vk::BufferUsageFlagBits::eVertexBuffer;

    buffer = vkUtil::createBuffer(bufferInput);

    void* memoryLocation = device.mapMemory(buffer.bufferMemory, 0, bufferInput.size);
    memcpy(memoryLocation, vertices.data(), bufferInput.size);
    device.unmapMemory(buffer.bufferMemory);
}

TriangleMesh::~TriangleMesh()
{
    device.destroyBuffer(buffer.buffer);
    device.freeMemory(buffer.bufferMemory);
}