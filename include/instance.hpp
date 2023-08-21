#pragma once

#include "config.hpp"

namespace vkInit {

bool supported(const std::vector<const char*>& extensions, const std::vector<const char*>& layers);

vk::Instance createInstance(
    const char* applicationName,
    const std::vector<const char*>& extensions,
    const std::vector<const char*>& layers);

} // namespace vkInit