#include "polyscope/polyscope.h"

#include "polyscope/surface_mesh.h"

namespace polyscope {

void drawImGuizmo() {
  ImGui::SetNextWindowPos(ImVec2(0, 0));
  ImGui::SetNextWindowSize(ImGui::GetIO().DisplaySize);
  ImGui::Begin("FullscreenWindow", nullptr,
               ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove |
                   ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoInputs |
                   ImGuiWindowFlags_NoBackground);

  Structure* s = state::selectedStructure;
  if (s != nullptr) {
    ImGuizmo::SetOrthographic(false);
    ImGuizmo::SetDrawlist();
    ImGuizmo::SetRect(0, 0, view::windowWidth, view::windowHeight);

    glm::mat4 viewMat = view::getCameraViewMatrix();
    glm::mat4 projMat = view::getCameraPerspectiveMatrix();
    glm::mat4 transform = s->getTransform();
    ImGuizmo::OPERATION gizmoType = ImGuizmo::OPERATION::TRANSLATE;
    ImGuizmo::Manipulate(glm::value_ptr(viewMat), glm::value_ptr(projMat), gizmoType, ImGuizmo::LOCAL,
                            glm::value_ptr(transform), nullptr, nullptr);
  }
  ImGui::End();
}

void drawSelectionBox(glm::vec2 p0, glm::vec2 p1, glm::vec3 color) {
  p0 = {p0.x / (view::windowWidth - 1), p0.y / (view::windowHeight - 1)};
  p1 = {p1.x / (view::windowWidth - 1), p1.y / (view::windowHeight - 1)};
  
  render::engine->setDepthMode(DepthMode::Disable);
  render::engine->setBlendMode(BlendMode::Disable);

  std::shared_ptr<render::ShaderProgram> program;
  program = render::engine->requestShader("SCREENSPACE_LINE", std::vector<std::string>(),
                                          render::ShaderReplacementDefaults::Process);

  std::vector<glm::vec3> positions;
  std::vector<glm::vec3> colors;

  glm::vec3 topLeft = glm::vec3(p0.x, p0.y, 0.0f);
  glm::vec3 topRight = glm::vec3(p1.x, p0.y, 0.0f);
  glm::vec3 bottomLeft = glm::vec3(p0.x, p1.y, 0.0f);
  glm::vec3 bottomRight = glm::vec3(p1.x, p1.y, 0.0f);

  positions.push_back(topLeft);
  positions.push_back(bottomLeft);

  positions.push_back(bottomLeft);
  positions.push_back(bottomRight);

  positions.push_back(bottomRight);
  positions.push_back(topRight);

  positions.push_back(topRight);
  positions.push_back(topLeft);

  for (int i = 0; i < 8; i++) colors.push_back(color);

  // Store data in buffers
  program->setAttribute("a_position", positions);
  program->setAttribute("a_color", colors);

  program->draw();
}

static float GetSegmentLengthClipSpace(const glm::vec3& start, const glm::vec3& end, const glm::mat4& mvpMatrix)
{
  glm::vec4 startOfSegment = mvpMatrix * glm::vec4(start, 1.0f);
  if (fabsf(startOfSegment.w) > FLT_EPSILON) // check for axis aligned with camera direction
  {
    startOfSegment *= 1.f / startOfSegment.w;
  }

  glm::vec4 endOfSegment = mvpMatrix * glm::vec4(end, 1.0f);
  if (fabsf(endOfSegment.w) > FLT_EPSILON) // check for axis aligned with camera direction
  {
    endOfSegment *= 1.f / endOfSegment.w;
  }

  glm::vec3 clipSpaceAxis = endOfSegment - startOfSegment;
  // clipSpaceAxis.y /= mDisplayRatio;
  float segmentLengthInClipSpace = sqrtf(clipSpaceAxis.x * clipSpaceAxis.x + clipSpaceAxis.y * clipSpaceAxis.y);
  return segmentLengthInClipSpace;
}

static const glm::vec3 directionUnary[3] = { glm::vec3(1.f, 0.f, 0.f), glm::vec3(0.f, 1.f, 0.f), glm::vec3(0.f, 0.f, 1.f) };
static const glm::vec3 directionColor[3] = { glm::vec3(0.800f, 0.325f, 0.173f), glm::vec3(0.145f, 0.667f, 0.145f), glm::vec3(0.443f, 0.369f, 0.847f) };
static const float mScreenFactor = 0.8f;

static void ComputeTripodAxisAndVisibility(int axisIndex, glm::vec3& dirAxis, bool& belowAxisLimit, const glm::mat4& mvpMatrix)
{
  dirAxis = directionUnary[axisIndex];
  
   // new method
   float lenDir = GetSegmentLengthClipSpace(glm::vec3(0.f, 0.f, 0.f), dirAxis, mvpMatrix);
   float lenDirMinus = GetSegmentLengthClipSpace(glm::vec3(0.f, 0.f, 0.f), -dirAxis, mvpMatrix);

   // float mulAxis = (lenDir < lenDirMinus && fabsf(lenDir - lenDirMinus) > FLT_EPSILON) ? -1.f : 1.f;
   float mulAxis = 1.f;
   dirAxis *= mulAxis;

   // for axis
   float axisLengthInClipSpace = GetSegmentLengthClipSpace(glm::vec3(0.f, 0.f, 0.f), dirAxis * mScreenFactor, mvpMatrix);
   belowAxisLimit = (axisLengthInClipSpace > 0.02f);
}

static glm::vec2 worldToPos(const glm::vec3& worldPos, const glm::mat4& mvpMatrix)
{
  glm::vec4 trans = mvpMatrix * glm::vec4(worldPos, 1.0f);

  trans *= 0.5f / trans.w;
  trans += glm::vec4(0.5f, 0.5f, 0.0f, 0.0f);
  return glm::vec2(trans.x, trans.y);
}

static void drawAxis(const glm::mat4& viewMatrix, const glm::mat4& projMatrix) {

  // draw
  bool belowAxisLimit = false;
  constexpr float arrowSize = 0.03f;
  constexpr float lineWidth = 10.0f;

  std::shared_ptr<render::ShaderProgram> program;
  program = render::engine->requestShader("SCREENSPACE_LINE", std::vector<std::string>(),
                                          render::ShaderReplacementDefaults::Process);
  
  // render::engine->setDepthMode(DepthMode::Disable);
  // render::engine->setBlendMode(BlendMode::Disable);
  render::engine->setLineWidth(lineWidth);
  
  std::vector<glm::vec3> positions;
  std::vector<glm::vec3> colors;
  for (unsigned int i = 0; i < 3; ++i) {
    glm::vec3 dirAxis;
    glm::mat4 mvpMatrix = projMatrix * viewMatrix;
    ComputeTripodAxisAndVisibility(i, dirAxis, belowAxisLimit, mvpMatrix);

    // draw axis
    if (belowAxisLimit)
    {
      glm::vec2 baseSSpace = worldToPos(dirAxis * 0.0f * mScreenFactor, mvpMatrix);
      glm::vec2 worldDirSSpace = worldToPos(dirAxis * mScreenFactor, mvpMatrix);
      positions.push_back(glm::vec3(baseSSpace, 0.0f));
      positions.push_back(glm::vec3(worldDirSSpace, 0.0f));
      colors.push_back(directionColor[i]);
      colors.push_back(directionColor[i]);
    }
  }
  // Store data in buffers
  program->setAttribute("a_position", positions);
  program->setAttribute("a_color", colors);
  program->draw();
  render::engine->setLineWidth(1.0f);
  
  
  program = render::engine->requestShader("SCREENSPACE_TRIANGLE", std::vector<std::string>(),
                                        render::ShaderReplacementDefaults::Process);

  positions.clear();
  colors.clear();
  for (unsigned int i = 0; i < 3; ++i) {
    glm::vec3 dirAxis;
    glm::mat4 mvpMatrix = projMatrix * viewMatrix;
    ComputeTripodAxisAndVisibility(i, dirAxis, belowAxisLimit, mvpMatrix);

    // draw axis
    if (belowAxisLimit)
    {
      // Arrow head begin
      glm::vec2 origin = worldToPos(glm::vec3(0, 0, 0), mvpMatrix);
      glm::vec2 worldDirSSpace = worldToPos(dirAxis * mScreenFactor, mvpMatrix);
      glm::vec2 dir = glm::normalize(origin - worldDirSSpace);
      
      dir *= arrowSize;
      
      glm::vec2 ortogonalDir(dir.y * 0.8f, -dir.x * 0.8f); // Perpendicular vector
      glm::vec2 a(worldDirSSpace + dir);
      
      positions.push_back(glm::vec3(worldDirSSpace - dir, 0.0f));
      positions.push_back(glm::vec3(a + ortogonalDir, 0.0f));
      positions.push_back(glm::vec3(a - ortogonalDir, 0.0f));
      for (int j = 0; j < 3; j++) {
        colors.push_back(directionColor[i]);
      }
      // Arrow head end
    }
  }
  // Store data in buffers
  program->setAttribute("a_position", positions);
  program->setAttribute("a_color", colors);
  program->draw();
}

std::vector<glm::vec4> renderMeshImage(SurfaceMesh* mesh, const glm::mat4& viewMatrix, const glm::mat4& projMatrix, const glm::vec2& imageSize) {
  bool smooth = mesh->isSmoothShade();
  mesh->setSmoothShade(true);
  
  render::FrameBuffer* customRenderFrameBuffer = render::engine->customRenderFrameBuffer.get();
  customRenderFrameBuffer->resize(imageSize.x, imageSize.y);
  customRenderFrameBuffer->setViewport(0, 0, imageSize.x, imageSize.y);
  
  render::FrameBuffer* customImageFrameBuffer = render::engine->customImageFrameBuffer.get();
  customImageFrameBuffer->resize(imageSize.x, imageSize.y);
  customImageFrameBuffer->setViewport(0, 0, imageSize.x, imageSize.y);
  
  render::engine->setDepthMode();
  render::engine->setBlendMode(BlendMode::Disable);

  if (!customRenderFrameBuffer->bindForRendering()) return std::vector<glm::vec4>();
  customRenderFrameBuffer->clear();
  mesh->drawToCustomBuffer(viewMatrix, projMatrix);
  
  if (!customImageFrameBuffer->bindForRendering()) return std::vector<glm::vec4>();
  customImageFrameBuffer->clear();
  render::engine->applyLightingTransform(render::engine->customRenderColor);
  
  mesh->setSmoothShade(smooth);

  // drawAxis(viewMatrix, projMatrix);
  
  return render::engine->customImageColor->getDataVector4();
}

} // namespace polyscope