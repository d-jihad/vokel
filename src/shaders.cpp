#include "shaders.hpp"

#include <fstream>

namespace vkUtil {
std::vector<char> readFile(const std::string& filename)
{
    std::ifstream file(filename, std::ios::ate | std::ios::binary);

    if (!file.is_open()) {
        throw std::runtime_error { "Failed to open file \"" + filename + "\"" };
    }

    size_t filesize { static_cast<size_t>(file.tellg()) };

    std::vector<char> buffer(filesize);
    file.seekg(0);
    file.read(buffer.data(), filesize);

    return buffer;
}

vk::ShaderModule createShaderModule(const std::string& filename, const vk::Device& device)
{
    std::vector<char> sourceCode = readFile(filename);

    vk::ShaderModuleCreateInfo moduleInfo {
        vk::ShaderModuleCreateFlags(),
        sourceCode.size(),
        reinterpret_cast<const uint32_t*>(sourceCode.data())
    };

    try {
        return device.createShaderModule(moduleInfo);
    } catch (const vk::SystemError& err) {
        std::cout << "Failed to create shader module from: " << filename << "\n\t" << err.what();
    }

    return nullptr;
}

}