#pragma once
#include "config.hpp"

#include <vector>

namespace VoKel {
class Scene {
public:
    Scene();

    std::vector<glm::vec3> trianglePositions;
};
}
