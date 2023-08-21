#pragma once

#include <vulkan/vulkan.hpp>

#ifdef NDEBUG
const bool DEBUG_MODE = false;
#else
const bool DEBUG_MODE = true;
#endif

/*
 * Statically linking the prebuilt header from the LunarG SDK,
 * load most functions, but not all.
 *
 * Functions can also be dynamically loaded, using call
 *
 * PNF_vkVoidFunction vkGetInstanceProcAddr(
 *   VkInstance     instance,
 *   const char*    pName);
 *
 * or
 *
 * PNF_vkVoidFunction vkGetDeviceProcAddress(
 *   VkDevice       device,
 *   const char*    pName);
 */

#include <iostream>
#include <stdexcept>
#include <stdint.h>
#include <string>
#include <vector>
