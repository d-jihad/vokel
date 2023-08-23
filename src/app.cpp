#include "app.hpp"
#include "scene.hpp"

#include <sstream>
#include <stdint.h>

App::App(uint32_t width, uint32_t height)
    : window { "Voxelize this!", width, height }
    , graphicEngine { width, height, window }
    , scene {}
{
}

App::~App()
{
}

void App::run()
{
    while (!window.shouldClose()) {
        window.update();
        graphicEngine.render(scene);
        calculateFrameRate();
    }
}

void App::calculateFrameRate()
{
    double delta = window.getElapsedTime(lastTime);

    if (delta >= 1) {
        int framerate { std::max(1, int(numFrames / delta)) };

        std::stringstream title;
        title << "Voxelize this! @ " << framerate << "fps";
        window.setWindowTitle(title.str());
        lastTime = window.getTime();
        numFrames = -1;
        frameTime = float(1000.0 / framerate);
    }

    ++numFrames;
}