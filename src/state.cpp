// Copyright 2017-2019, Nicholas Sharp and the Polyscope contributors. http://polyscope.run.
#include "polyscope/polyscope.h"

namespace polyscope {

namespace state {

bool initialized = false;
std::string backend = "";
float lengthScale = 1.0;
std::tuple<glm::vec3, glm::vec3> boundingBox =
    std::tuple<glm::vec3, glm::vec3>{glm::vec3{-1., -1., -1.}, glm::vec3{1., 1., 1.}};
std::map<std::string, std::map<std::string, Structure*>> structures;
std::vector<std::function<void()>> userCallbacks;
bool doDefaultMouseInteraction = true;

// Lists of things
std::set<Widget*> widgets;
std::vector<SlicePlane*> slicePlanes;

// Added by cyh
size_t facePickIndStart;
size_t edgePickIndStart;
size_t halfedgePickIndStart;
GemCraft::MeshSubset selectedRegion;
std::vector<glm::vec3> targetPositions;
std::vector<glm::vec3> targetNormals;
std::vector<glm::vec3> strokePosition;
std::vector<glm::vec3> strokeNormal;
float edgeLengthScale;
std::array<glm::vec2, 2> selectionBox = {glm::vec2(-1, -1), glm::vec2(-1, -1)};
glm::vec3 startPath, endPath;

} // namespace state
} // namespace polyscope
