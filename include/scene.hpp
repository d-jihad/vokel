#pragma once
#include "config.hpp"

#include <vector>

class Scene {
public:
    Scene();

    std::vector<glm::vec3> trianglePositions;
};