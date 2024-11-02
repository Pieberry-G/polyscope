// Copyright 2017-2019, Nicholas Sharp and the Polyscope contributors. http://polyscope.run.
#include "polyscope/pick.h"

#include "polyscope/polyscope.h"

#include <limits>
#include <tuple>

using std::cout;
using std::endl;

namespace polyscope {
namespace pick {

size_t currLocalPickInd = 0;
Structure* currPickStructure = nullptr;
bool haveSelectionVal = false;

// The next pick index that a structure can use to identify its elements
// (get it by calling request pickBufferRange())
size_t nextPickBufferInd = 1; // 0 reserved for "none"
// Track which ranges have been allocated to which structures
std::vector<std::tuple<size_t, size_t, Structure*>> structureRanges;


// == Set up picking
size_t requestPickBufferRange(Structure* requestingStructure, size_t count) {

  // Check if we can satisfy the request
  size_t maxPickInd = std::numeric_limits<size_t>::max();
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wshift-count-overflow"
  if (bitsForPickPacking < 22) {
    uint64_t bitMax = 1ULL << (bitsForPickPacking * 3);
    if (bitMax < maxPickInd) {
      maxPickInd = bitMax;
    }
  }
#pragma GCC diagnostic pop

  if (count > maxPickInd || maxPickInd - count < nextPickBufferInd) {
    error("Wow, you sure do have a lot of stuff, Polyscope can't even count it all. (Ran out of indices while "
          "enumerating structure elements for pick buffer.)");
  }

  size_t ret = nextPickBufferInd;
  nextPickBufferInd += count;
  structureRanges.push_back(std::make_tuple(ret, nextPickBufferInd, requestingStructure));
  return ret;
}

// == Manage stateful picking

void resetSelection() {
  haveSelectionVal = false;
  currLocalPickInd = 0;
  currPickStructure = nullptr;
}

bool haveSelection() { return haveSelectionVal; }

void resetSelectionIfStructure(Structure* s) {
  if (haveSelectionVal && currPickStructure == s) {
    resetSelection();
  }
}

std::pair<Structure*, size_t> getSelection() {
  if (haveSelectionVal) {
    return {currPickStructure, currLocalPickInd};
  } else {
    return {nullptr, 0};
  }
}

void setSelection(std::pair<Structure*, size_t> newPick) {
  if (newPick.first == nullptr) {
    resetSelection();
  } else {
    haveSelectionVal = true;
    currPickStructure = newPick.first;
    currLocalPickInd = newPick.second;
  }
}

// == Helpers

std::pair<Structure*, size_t> globalIndexToLocal(size_t globalInd) {
  // Loop through the ranges that we have allocated to find the one correpsonding to this structure.
  for (const auto& x : structureRanges) {

    size_t rangeStart = std::get<0>(x);
    size_t rangeEnd = std::get<1>(x);
    Structure* structure = std::get<2>(x);

    if (globalInd >= rangeStart && globalInd < rangeEnd) {
      return {structure, globalInd - rangeStart};
    }
  }

  return {nullptr, 0};
}

size_t localIndexToGlobal(std::pair<Structure*, size_t> localPick) {
  if (localPick.first == nullptr) return 0;

  for (const auto& x : structureRanges) {
    size_t rangeStart = std::get<0>(x);
    size_t rangeEnd = std::get<1>(x);
    Structure* structure = std::get<2>(x);

    if (structure == localPick.first) {
      return rangeStart + localPick.second;
    }
  }

  throw std::runtime_error("structure does not match any allocated pick range");
  return 0;
}


std::pair<Structure*, size_t> evaluatePickQuery(int xPos, int yPos) {

  // NOTE: hack used for debugging: if xPos == yPos == 1 we do a pick render but do not query the value.

  // Be sure not to pick outside of buffer
  if (xPos < -1 || xPos >= view::bufferWidth || yPos < -1 || yPos >= view::bufferHeight) {
    return {nullptr, 0};
  }

  render::FrameBuffer* pickFramebuffer = render::engine->pickFramebuffer.get();

  render::engine->setDepthMode();
  render::engine->setBlendMode(BlendMode::Disable);

  pickFramebuffer->resize(view::bufferWidth, view::bufferHeight);
  pickFramebuffer->setViewport(0, 0, view::bufferWidth, view::bufferHeight);
  pickFramebuffer->clearColor = glm::vec3{0., 0., 0.};
  if (!pickFramebuffer->bindForRendering()) return {nullptr, 0};
  pickFramebuffer->clear();

  // Render pick buffer
  for (auto cat : state::structures) {
    for (auto x : cat.second) {
      x.second->drawPick();
    }
  }

  if (xPos == -1 || yPos == -1) {
    return {nullptr, 0};
  }

  // Read from the pick buffer
  std::array<float, 4> result = pickFramebuffer->readFloat4(xPos, view::bufferHeight - yPos);
  size_t globalInd = pick::vecToInd(glm::vec3{result[0], result[1], result[2]});

  return pick::globalIndexToLocal(globalInd);
}

// Added by cyh
void updateGBuffer() {

  render::FrameBuffer* gBuffer = render::engine->gBuffer.get();

  render::engine->setDepthMode();
  render::engine->setBlendMode();

  gBuffer->resize(view::bufferWidth, view::bufferHeight);
  gBuffer->setViewport(0, 0, view::bufferWidth, view::bufferHeight);
  gBuffer->clearColor = glm::vec3{0., 0., 0.};
  if (!gBuffer->bindForRendering()) return;
  gBuffer->clear();

  // Render pos buffer
  for (auto cat : state::structures) {
    for (auto x : cat.second) {
      if (x.first == "Ring")
        x.second->drawGBuffer();
    }
  }
}

// Index 0 is Position, and index 1 is Normal
glm::vec3 gBufferQuery(int xPos, int yPos, int index) {

  // NOTE: hack used for debugging: if xPos == yPos == 1 we do a pick render but do not query the value.

  // Be sure not to pick outside of buffer
  if (xPos < 0 || xPos >= view::bufferWidth || yPos < 0 || yPos >= view::bufferHeight) {
    return glm::vec4{0., 0., 0., 0.};
  }

  render::FrameBuffer* gBuffer = render::engine->gBuffer.get();

  // Read from the pick buffer
  std::array<float, 4> result = gBuffer->readFloat4(xPos, view::bufferHeight - yPos, index);
  
  return glm::vec3{result[0], result[1], result[2]};
}

void updatePickFramebuffer() {

  render::FrameBuffer* pickFramebuffer = render::engine->pickFramebuffer.get();

  render::engine->setDepthMode();
  render::engine->setBlendMode(BlendMode::Disable);

  pickFramebuffer->resize(view::bufferWidth, view::bufferHeight);
  pickFramebuffer->setViewport(0, 0, view::bufferWidth, view::bufferHeight);
  pickFramebuffer->clearColor = glm::vec3{0., 0., 0.};
  if (!pickFramebuffer->bindForRendering()) return;
  pickFramebuffer->clear();

  // Render pick buffer
  for (auto cat : state::structures) {
    for (auto x : cat.second) {
      if (x.first == "Ring")
        x.second->drawPick();
    }
  }
}

std::vector<std::pair<Structure*, size_t>> pickQuery(glm::vec2 p1, glm::vec2 p2) {

  // NOTE: hack used for debugging: if xPos == yPos == 1 we do a pick render but do not query the value.

  // Be sure not to pick outside of buffer
  int width = p2.x - p1.x + 1;
  int height = p2.y - p1.y + 1;
  if (p1.x < 0 || p1.x >= view::bufferWidth || p1.y < 0 || p1.y >= view::bufferHeight) {
    return std::vector<std::pair<Structure*, size_t>>();
  }
  if (p2.x < 0 || p2.x >= view::bufferWidth || p2.y < 0 || p2.y >= view::bufferHeight) {
    return std::vector<std::pair<Structure*, size_t>>();
  }

  render::FrameBuffer* pickFramebuffer = render::engine->pickFramebuffer.get();
  std::vector<std::pair<Structure*, size_t>> pickFramebufferContent(width * height);
  // Read from the pick buffer
  std::vector<std::array<float, 4>> result = pickFramebuffer->readFloatBuffer();
  for (int i = 0; i < height; i++) {
    for (int j = 0; j < width; j++) {
      int index = i * width + j;
      int resIndex = (view::bufferHeight - (p1.y + i)) * view::bufferWidth + (p1.x + j);
      size_t globalInd = pick::vecToInd(glm::vec3{result[resIndex][0], result[resIndex][1], result[resIndex][2]});
      pickFramebufferContent[index] = pick::globalIndexToLocal(globalInd);
    }
  }

  return pickFramebufferContent;
}

} // namespace pick


} // namespace polyscope
