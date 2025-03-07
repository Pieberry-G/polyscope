// Copyright 2017-2019, Nicholas Sharp and the Polyscope contributors. http://polyscope.run.
#pragma once

#include "imgui.h"
#include "polyscope/group.h"
#include "polyscope/internal.h"
#include "polyscope/messages.h"
#include "polyscope/options.h"
#include "polyscope/screenshot.h"
#include "polyscope/slice_plane.h"
#include "polyscope/structure.h"
#include "polyscope/transformation_gizmo.h"
#include "polyscope/utilities.h"
#include "polyscope/widget.h"

#include <functional>
#include <map>
#include <set>
#include <unordered_set>

// Added by cyh
#include "Mesh/MeshSubset.h"

namespace polyscope {

// forward declarations
class Structure;
class Group;

// Initialize polyscope, including windowing system and openGL. Should be called exactly once at the beginning of a
// program. If initialization fails in any way, an exception will be thrown.
// The backend string sets which rendering backend to use. If "", a reasonable default backend will be chosen.
void init(std::string backend = "");

// Check that polyscope has been initialized. If not, an exception is thrown to prevent further problems.
void checkInitialized();

// Returns a bool indicating whether or not polyscope has been initialized
bool isInitialized();

// Give control to the polyscope GUI. Blocks until the user returns control via
// the GUI, possibly by exiting the window.
void show(size_t forFrames = std::numeric_limits<size_t>::max());

// Do shutdown work and de-initialize Polyscope
void shutdown();

// === Global variables ===
namespace state {

// has polyscope::init() been called?
extern bool initialized;

// what backend was set on initialization
extern std::string backend;

// lists of all structures in Polyscope, by category
extern std::map<std::string, std::map<std::string, Structure*>> structures;

// lists of all groups in Polyscope
extern std::map<std::string, Group*> groups;

// representative length scale for all registered structures
extern float lengthScale;

// axis-aligned bounding box for all registered structures
extern std::tuple<glm::vec3, glm::vec3> boundingBox;

// a list of widgets and other more specific doodads in the scene
extern std::set<Widget*> widgets;
extern std::vector<SlicePlane*> slicePlanes;

// should we allow default trackball mouse camera interaction? 
// Needs more interactions on when to turn this on/off
extern bool doDefaultMouseInteraction;

// callback functions used to render "user" guis
extern std::function<void()> mainMenuCallback;
extern std::vector<std::function<void()>> userCallbacks;

// Added by cyh
extern size_t facePickIndStart;
extern size_t edgePickIndStart;
extern size_t halfedgePickIndStart;
extern GemCraft::MeshSubset selectedRegion;
extern size_t interactiveFace;
extern std::vector<glm::vec3> targetPositions;
extern std::vector<glm::vec3> targetNormals;
extern std::vector<glm::vec3> strokePosition;
extern std::vector<glm::vec3> strokeNormal;
extern float edgeLengthScale;
extern std::array<glm::vec2, 2> selectionBox;
extern glm::vec3 startPath, endPath;


// representative center for all registered structures
glm::vec3 center();

} // namespace state

// === Manage structures tracked by polyscope

// Register a structure with polyscope
// Structure name must be a globally unique identifier for the structure.
bool registerStructure(Structure* structure, bool replaceIfPresent = true);

// Get a reference to a structure that has been registered
// The default version with name="" arbitrarily returns any structure of that type. This is useful as a shorthand when
// only using a single structure.
Structure* getStructure(std::string type, std::string name = "");

// True if such a structure exists
bool hasStructure(std::string type, std::string name = "");

// Group management
bool registerGroup(std::string name);
bool setParentGroupOfGroup(std::string child, std::string parent);
bool setParentGroupOfStructure(std::string typeName, std::string child, std::string parent);
bool setParentGroupOfStructure(Structure* child, std::string parent);
// De-register a group
void setGroupEnabled(std::string name, bool enabled);
void removeGroup(std::string name, bool errorIfAbsent = true);
void removeAllGroups();

// De-register a structure, of any type. Also removes any quantities associated with the structure
void removeStructure(Structure* structure, bool errorIfAbsent = true);
void removeStructure(std::string type, std::string name, bool errorIfAbsent = true);
void removeStructure(std::string name, bool errorIfAbsent = true);

// De-register all structures, of any type. Also removes any quantities associated with the structure
void removeAllStructures();

// Recompute the global state::lengthScale, boundingBox, and center by looping over registered structures
void updateStructureExtents();

// Essentially regenerates all state and programs within Polyscope, calling refresh() recurisvely on all structures and
// quantities
void refresh();

// === Handle draw flow, interrupts, and popups

// Main draw call, which handles all 3D rendering & UI management.
// End users generally should not call this function. Consider requestRedraw() or screenshot().
void draw(bool withUI = true, bool withContextCallback = true);

// Request that the 3D scene be redrawn for the next frame. Should be called anytime something changes in the scene.
void requestRedraw();

// Has a redraw been requested for the next frame?
bool redrawRequested();

// Managed a stack of of contexts to draw the UI. Usually contains one entry, which causes the main GUI to be drawn, but
// in general the top callback will be called instead. Primarily exists to manage the ImGUI context, so callbacks can
// create other contexts and circumvent the main draw loop. This is used internally to implement messages, element
// selections, etc.
void pushContext(std::function<void()> callbackFunction, bool drawDefaultUI = true);
void popContext();

// Get current ImGui context
// When linking to Polyscope as a shared library, we must set the current context
// explicitly before making calls to ImGui in host application
ImGuiContext* getCurrentContext();

// These helpers are called internally by Polyscope to render and build the UI.
// Normally, applications should not need to call them, but in advanced settings when making custom UIs, they may be
// useful to manually build pieces of the interface.
void buildPolyscopeGui();
void buildStructureGui();
void buildPickGui();
void buildUserGuiAndInvokeCallback();


// === Utility

// Execute one iteration of the main loop
// Exposed so that some weird flow (eg, errors) can re-enter the main loop when appropriate. Be careful!
void mainLoopIteration();
void initializeImGUIContext();
void drawStructures();

// Called to check any options that might have been changed and perform appropriate updates. Users generally should not
// need to call this directly.
void processLazyProperties();


} // namespace polyscope
