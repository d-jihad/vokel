#pragma once
#include "config.hpp"
#include "memory.hpp"

class TriangleMesh {
public:
    TriangleMesh(vk::Device device, vk::PhysicalDevice physicalDevice);
    ~TriangleMesh();
    vkUtil::Buffer buffer;

private:
    vk::Device device;
};