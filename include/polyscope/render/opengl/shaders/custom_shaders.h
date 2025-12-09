#pragma once

#include "polyscope/render/opengl/gl_shaders.h"

namespace polyscope {
namespace render {
namespace backend_openGL3_glfw {

extern const ShaderStageSpecification SCREENSPACE_LINE_VERT_SHADER;
extern const ShaderStageSpecification SCREENSPACE_LINE_FRAG_SHADER;
extern const ShaderStageSpecification SCREENSPACE_TRIANGLE_VERT_SHADER;
extern const ShaderStageSpecification SCREENSPACE_TRIANGLE_FRAG_SHADER;

} // namespace backend_openGL3
} // namespace render
} // namespace polyscope
