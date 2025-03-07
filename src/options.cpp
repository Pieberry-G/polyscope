// Copyright 2017-2019, Nicholas Sharp and the Polyscope contributors. http://polyscope.run.
#include "polyscope/options.h"
#include "polyscope/imgui_config.h"

namespace polyscope {
namespace options {

std::string programName = "GemCraft";
int verbosity = 2;
std::string printPrefix = "[polyscope] ";
bool errorsThrowExceptions = false;
bool debugDrawPickBuffer = false;
int maxFPS = 60;
bool usePrefsFile = true;
bool initializeWithDefaultStructures = true;
bool alwaysRedraw = true;
bool autocenterStructures = false;
bool autoscaleStructures = false;
bool automaticallyComputeSceneExtents = true;
bool invokeUserCallbackForNestedShow = false;
bool giveFocusOnShow = false;

bool screenshotTransparency = true;
std::string screenshotExtension = ".png";

// == Scene options

// Ground plane / shadows
bool groundPlaneEnabled = true;
GroundPlaneMode groundPlaneMode = GroundPlaneMode::TileReflection;
ScaledValue<float> groundPlaneHeightFactor = 0;
int shadowBlurIters = 2;
float shadowDarkness = 0.25;

// Rendering options

int ssaaFactor = 1;

// Transparency
TransparencyMode transparencyMode = TransparencyMode::None;
int transparencyRenderPasses = 8;

// === Advanced ImGui configuration

bool buildGui = true;
bool openImGuiWindowForUserCallback = true;
std::function<void()> configureImGuiStyleCallback = configureImGuiStyle;
std::string themeColor = "Green";
std::function<std::tuple<ImFontAtlas*, ImFont*, ImFont*>()> prepareImGuiFontsCallback = prepareImGuiFonts;


// enabled by default in debug mode
#ifndef NDEBUG
bool enableRenderErrorChecks = false;
#else
bool enableRenderErrorChecks = true;
#endif

} // namespace options
} // namespace polyscope
