#pragma once

#include "config.hpp"

#define SDL_MAIN_HANDLED
#include <SDL2/SDL.h>
#include <SDL2/SDL_vulkan.h>

#include <string>
#include <vector>

namespace VoKel {

class Window {
public:
    Window(const std::string& title, const uint32_t w, const uint32_t h);
    ~Window();

    Window(const Window&) = delete;
    Window& operator=(const Window&) = delete;

    void update();
    [[nodiscard]] bool shouldClose() { return !keep_running; };

    [[nodiscard]] std::vector<const char*> getVulkanRequiredExtensions();

    VkSurfaceKHR createVulkanSurface(const vk::Instance& instance);

private:
    SDL_Window* window;
    uint32_t width, height;
    std::string title;

    bool keep_running { true };

    SDL_Event e;

    void init();
    void processInput();
};
} // namespace VoKel
