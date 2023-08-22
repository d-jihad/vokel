#include "framebuffer.hpp"
#include <stdexcept>

namespace vkInit {

void createFramebuffer(framebufferInput inputChunk, std::vector<vkInit::SwapchainFrame>& frames)
{
    for (size_t i { 0 }; i < frames.size(); i++) {
        std::vector<vk::ImageView> attachments {
            frames[i].imageView
        };

        vk::FramebufferCreateInfo framebufferInfo {};
        framebufferInfo.flags = vk::FramebufferCreateFlags();
        framebufferInfo.renderPass = inputChunk.renderpass;
        framebufferInfo.attachmentCount = attachments.size();
        framebufferInfo.pAttachments = attachments.data();
        framebufferInfo.width = inputChunk.swapchainExtent.width;
        framebufferInfo.height = inputChunk.swapchainExtent.height;
        framebufferInfo.layers = 1;

        try {
            frames[i].framebuffer = inputChunk.device.createFramebuffer(framebufferInfo);
        } catch (const vk::SystemError& err) {
            std::runtime_error { "Failed to create framebuffer for frame " + std::to_string(i) + " " + err.what() };
        }
    }
}

}