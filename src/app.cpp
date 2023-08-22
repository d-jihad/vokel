#include "app.hpp"
#include <SDL2/SDL_timer.h>
#include <SDL2/SDL_video.h>
#include <algorithm>
#include <sstream>
#include <stdint.h>

App::App(uint32_t width, uint32_t height)
    : window { "Voxelize this!", width, height }
    , graphicEngine { width, height, window }
{
}

App::~App()
{
}

void App::run()
{
    while (!window.shouldClose()) {
        window.update();
        graphicEngine.render();
        calculateFrameRate();
    }
}

void App::calculateFrameRate()
{
    currentTime = window.getTime();
    double delta = (double)((currentTime - lastTime) / (double)SDL_GetPerformanceFrequency());

    if (delta >= 1) {
        int framerate { std::max(1, int(numFrames / delta)) };

        std::stringstream title;
        title << "Voxelize this! @ " << framerate << "fps";
        window.setWindowTitle(title.str());
        lastTime = currentTime;
        numFrames = -1;
        frameTime = float(1000.0 / framerate);
    }

    ++numFrames;
}