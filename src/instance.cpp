#include "instance.hpp"
#include "logging.hpp"

namespace vkInit {

bool supported(const std::vector<const char*>& requestedExtensions, const std::vector<const char*>& requestedLayers)
{
    auto supportedExtensions = vk::enumerateInstanceExtensionProperties();
    auto supportedLayers = vk::enumerateInstanceLayerProperties();

    if (DEBUG_MODE) {
        std::cout << "System can support the following instance extensions:\n";

        for (const auto& extension : supportedExtensions) {
            std::cout << '\t' << extension.extensionName << '\n';
        }

        std::cout << "System can support the following layers:\n";

        for (const auto& layer : supportedLayers) {
            std::cout << '\t' << layer.layerName << '\n';
        }
    }

    for (const auto& extension : requestedExtensions) {
        bool found = false;
        for (const auto& supportedExtension : supportedExtensions) {
            if (strcmp(extension, supportedExtension.extensionName) == 0) {
                found = true;
                break;
            }
        }

        if (!found) {
            if (DEBUG_MODE) {
                std::cout << "Cannot found extension: " << extension << '\n';
            }

            return false;
        }
    }

    for (const auto& layer : requestedLayers) {
        bool found = false;
        for (const auto& supportedLayer : supportedLayers) {
            if (strcmp(layer, supportedLayer.layerName) == 0) {
                found = true;
                break;
            }
        }

        if (!found) {
            if (DEBUG_MODE) {
                std::cout << "Cannot found layer: " << layer << '\n';
            }

            return false;
        }
    }

    return true;
}

vk::Instance createInstance(
    const char* applicationName,
    const std::vector<const char*>& extensions,
    const std::vector<const char*>& layers)
{
    // VkResult vkEnumerateInstanceVersion(
    //  uint32_t*                                   pApiVersion);
    uint32_t version = vk::enumerateInstanceVersion();
    if (DEBUG_MODE) {
        std::cout
            << "System can support vulkan:"
            << "\n\t Variant: " << VK_API_VERSION_VARIANT(version)
            << "\n\t Major:   " << VK_API_VERSION_MAJOR(version)
            << "\n\t Minor:   " << VK_API_VERSION_MINOR(version)
            << "\n\t Patch:   " << VK_API_VERSION_PATCH(version) << '\n';
    }

    /*
     * we can either use this version
     * (we should just be sure to set the patch to 0 for best
     * compatibility/stability)
     */
    // version &= ~(0xFFFU);

    /*
     * or drop down to an earlier version to ensure compatibility with more
     * devices VK_MAKE_API_VERSION(variant, major, minor, patch)
     */
    version = VK_MAKE_API_VERSION(0, 1, 0, 0);

    // typedef struct VkApplicationInfo {
    //     VkStructureType    sType;
    //     const void*        pNext;
    //     const char*        pApplicationName;
    //     uint32_t           applicationVersion;
    //     const char*        pEngineName;
    //     uint32_t           engineVersion;
    //     uint32_t           apiVersion;
    // } VkApplicationInfo;
    vk::ApplicationInfo appInfo {
        applicationName,
        version,
        "The hard way",
        version,
        version
    };

    if (DEBUG_MODE) {
        std::cout << "Requested extensions:\n";

        for (const auto& extensionName : extensions) {
            std::cout << "\t\"" << extensionName << "\"\n";
        }
    }

    // typedef struct VkInstanceCreateInfo {
    //      VkStructureType             sType;
    //      const void*                 pNext;
    //      VkInstanceCreateFlags       flags;
    //      const VkApplicationInfo*    pApplicationInfo;
    //      uint32_t                    enabledLayerCount;
    //      const char* const*          ppEnabledLayerNames;
    //      uint32_t                    enabledExtensionCount;
    //      const char* const*          ppEnabledExtensionNames;
    // } VkInstanceCreateInfo;
    vk::InstanceCreateInfo createInfo {
        vk::InstanceCreateFlags(),
        &appInfo,
        static_cast<uint32_t>(layers.size()),
        layers.data(),
        static_cast<uint32_t>(extensions.size()),
        extensions.data()
    };

    vk::DebugUtilsMessengerCreateInfoEXT debugMessenger {};
    if (DEBUG_MODE) {
        debugMessenger = vkInit::createDebugMessengerCreateInfo();
        createInfo.pNext = &debugMessenger;
    }

    try {
        // VkResult vkCreateInstance(
        //      const VkInstanceCreateInfo*                 pCreateInfo,
        //      const VkAllocationCallbacks*                pAllocator,
        //      VkInstance*                                 pInstance);
        return vk::createInstance(createInfo, nullptr);
    } catch (const vk::SystemError& err) {
        if (DEBUG_MODE) {
            std::cout << "Failed to create a Vulkan Instance!\n";
        }
        return nullptr;
    }
}

} // namespace vkInit