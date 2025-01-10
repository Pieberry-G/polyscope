// Added by cyh
#include "polyscope/custom_render_tools.h"
#include "polyscope/pick.h"

namespace polyscope {
namespace customRenderTools {

void updateGBuffer() {

  render::FrameBuffer* gBuffer = render::engine->gBuffer.get();

  render::engine->setDepthMode();
  render::engine->setBlendMode();

  gBuffer->resize(view::bufferWidth, view::bufferHeight);
  gBuffer->setViewport(0, 0, view::bufferWidth, view::bufferHeight);
  gBuffer->clearColor = glm::vec3{0., 0., 0.};
  if (!gBuffer->bindForRendering()) return;
  gBuffer->clear();
  gBuffer->clear(2, {-1.0f, -1.0f, -1.0f});

  // Render pos buffer
  for (auto cat : state::structures) {
    for (auto x : cat.second) {
      if (x.first == "Ring") x.second->drawGBuffer();
    }
  }
}

// Index 0 is Position, index 1 is Normal, and index 2 is FaceID
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
      if (x.first == "Ring") x.second->drawPick();
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

} // namespace renderTools
} // namespace polyscope