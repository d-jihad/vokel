#pragma once

#include "config.hpp"

namespace vkUtil {

std::vector<char> readFile(const std::string& filename);

vk::ShaderModule createShaderModule(const std::string& filename, const vk::Device& device);

} // namespace vkUtil