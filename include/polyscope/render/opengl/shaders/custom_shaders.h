// Added by cyh
#pragma once

#include "polyscope/render/opengl/gl_shaders.h"

namespace polyscope {
namespace render {
namespace backend_openGL3_glfw {

extern const ShaderStageSpecification MESH_GBUFFER_VERT_SHADER;
extern const ShaderStageSpecification MESH_GBUFFER_FRAG_SHADER;
extern const ShaderStageSpecification GLTF_VIEWER_VERT_SHADER;
extern const ShaderStageSpecification GLTF_VIEWER_FRAG_SHADER;
extern const ShaderStageSpecification SELECTION_BOX_VERT_SHADER;
extern const ShaderStageSpecification SELECTION_BOX_FRAG_SHADER;

} // namespace backend_openGL3
} // namespace render
} // namespace polyscope
