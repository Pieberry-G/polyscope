// Added by cyh
#pragma once

#include "polyscope/structure.h"

namespace polyscope {
namespace renderTools {

// Added by cyh
// Calculate 3D position corresponding to every pixel
void updateGBuffer();
glm::vec3 gBufferQuery(int xPos, int yPos, int index);
// Calculate pick ID corresponding to every pixel
void updatePickFramebuffer();
std::vector<std::pair<Structure*, size_t>> pickQuery(glm::vec2 p1, glm::vec2 p2);

} // namespace renderUtils
} // namespace polyscope