#pragma once
#include "config.hpp"
#include "engine.hpp"
#include "scene.hpp"
#include "window.hpp"

#include <stdint.h>

class App {

private:
    VoKel::Window window;
    VoKel::Engine graphicEngine;
    Scene scene;

    double lastTime, currentTime;
    int numFrames;
    float frameTime;

    void init(uint32_t width, uint32_t height);
    void calculateFrameRate();

public:
    App(uint32_t width, uint32_t height);
    ~App();

    void run();
};