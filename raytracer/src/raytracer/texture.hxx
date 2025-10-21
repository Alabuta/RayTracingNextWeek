#pragma once

#include "math/math.hxx"


namespace texture {
struct constant final {
    glm::vec4 color{1};
};

struct checker final {
    glm::vec4 even{1};
    glm::vec4 odd{1};
};
}
