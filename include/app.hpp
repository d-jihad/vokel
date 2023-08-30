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
    VoKel::Scene scene;

    double lastTime, currentTime;
    int numFrames;
    float frameTime;

    void calculateFrameRate();

public:
    App(int width, int height);
    ~App();

    void run();
};