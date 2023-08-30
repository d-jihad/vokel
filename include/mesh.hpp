#pragma once

#include "config.hpp"
#include <array>

namespace vkMesh {

vk::VertexInputBindingDescription getPosColorBindingDescription();

std::array<vk::VertexInputAttributeDescription, 2> getPosColorAttributeDescriptions();

}