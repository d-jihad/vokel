#include "window.hpp"
#include <SDL2/SDL_video.h>
#include <stdexcept>

namespace VoKel {

Window::Window(const std::string& title, const int w, const int h)
    : title { title }
    , width { w }
    , height { h }
{
    init();
};

Window::~Window()
{
    SDL_DestroyWindow(window);
    SDL_Quit();
}

void Window::init()
{
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        std::cout << "Failed to initialize the SDL2 library\n";
        throw std::runtime_error("Failed to initialize the SDL2 library");
    }

    window = SDL_CreateWindow(title.c_str(), SDL_WINDOWPOS_CENTERED,
        SDL_WINDOWPOS_CENTERED, width,
        height, SDL_WINDOW_VULKAN | SDL_WINDOW_RESIZABLE);

    if (!window) {
        std::cout << "Failed to create window\n";
        throw std::runtime_error(SDL_GetError());
    }
}

void Window::update()
{
    processInput();
    SDL_UpdateWindowSurface(window);
}

void Window::processInput()
{
    SDL_PollEvent(&e);

    switch (e.type) {
    case SDL_QUIT:
        keep_running = false;
        break;
    }
}

std::vector<const char*> Window::getVulkanRequiredExtensions()
{
    // Get the required Vulkan extensions
    unsigned int extensionCount { 0 };
    if (!SDL_Vulkan_GetInstanceExtensions(window, &extensionCount, nullptr)) {
        std::cerr << "Failed to get the number of required Vulkan extensions: "
                  << SDL_GetError() << std::endl;
        return {};
    }

    std::vector<const char*> extensionNames(extensionCount);
    if (!SDL_Vulkan_GetInstanceExtensions(window, &extensionCount,
            extensionNames.data())) {
        std::cerr << "Failed to get the required Vulkan extensions: "
                  << SDL_GetError() << std::endl;

        return {};
    }

    return extensionNames;
}

VkSurfaceKHR Window::createVulkanSurface(const vk::Instance& instance)
{
    VkSurfaceKHR surface { nullptr };
    if (SDL_FALSE == SDL_Vulkan_CreateSurface(window, instance, &surface)) {
        throw std::runtime_error { "Failed to abstract SDL surface for Vulkan" };
    } else {
        if (DEBUG_MODE) {
            std::cout << "Successfully abstracted SDL surface for Vulkan\n";
        }
    }

    return surface;
}

} // namespace VoKel