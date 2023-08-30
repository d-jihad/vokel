#include "mesh.hpp"

namespace vkMesh {

vk::VertexInputBindingDescription getPosColorBindingDescription()
{

    vk::VertexInputBindingDescription bindingDescription;
    bindingDescription.binding = 0;
    bindingDescription.stride = 5 * sizeof(float);
    bindingDescription.inputRate = vk::VertexInputRate::eVertex;

    return bindingDescription;
}

std::array<vk::VertexInputAttributeDescription, 2> getPosColorAttributeDescriptions()
{
    vk::VertexInputAttributeDescription pos;
    pos.binding = 0;
    pos.location = 0;
    pos.format = vk::Format::eR32G32Sfloat;
    pos.offset = 0;

    vk::VertexInputAttributeDescription col;
    col.binding = 0;
    col.location = 1;
    col.format = vk::Format::eR32G32B32Sfloat;
    col.offset = 2 * sizeof(float);

    return { pos, col };
}

}