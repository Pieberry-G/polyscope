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
Structure* selectedStructure = nullptr;
std::function<void()> mainMenuCallback;
std::vector<std::function<void()>> userCallbacks;
std::function<void()> tickSceneCallback;
bool doDefaultMouseInteraction = true;

// Lists of things
std::set<Widget*> widgets;
std::vector<SlicePlane*> slicePlanes;

} // namespace state
} // namespace polyscope
