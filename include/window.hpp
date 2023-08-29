#pragma once

#include "config.hpp"
#include <stdint.h>

#define SDL_MAIN_HANDLED
#include <SDL2/SDL.h>
#include <SDL2/SDL_vulkan.h>

#include <SDL2/SDL_timer.h>
#include <SDL2/SDL_video.h>

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

    double getTime() { return SDL_GetPerformanceCounter(); }

    double getElapsedTime(double start)
    {
        return (SDL_GetPerformanceCounter() - start) / (double)SDL_GetPerformanceFrequency();
    }

    void setWindowTitle(const std::string title) { SDL_SetWindowTitle(window, title.c_str()); }

    std::tuple<uint32_t, uint32_t> getFramebufferSize()
    {
        int w, h;
        SDL_Vulkan_GetDrawableSize(window, &w, &h);
        std::cout << "--------------------------- Get Drawable size returned " << w << " - " << h << "\n";
        return { w, h };
    }

    bool isMinimized()
    {
        return SDL_GetWindowFlags(window) & SDL_WINDOW_MINIMIZED;
    }

    void processInput();

private:
    SDL_Window* window;
    uint32_t width, height;
    std::string title;

    bool keep_running { true };

    SDL_Event e;

    void init();
};
} // namespace VoKel
