// Copyright 2017-2023, Nicholas Sharp and the Polyscope contributors. https://polyscope.run

#pragma once

#include "polyscope/render/opengl/gl_shaders.h"

namespace polyscope {
namespace render {
namespace backend_openGL3_glfw {

// Added by cyh
extern const ShaderStageSpecification MESH_GBUFFER_VERT_SHADER;
extern const ShaderStageSpecification MESH_GBUFFER_FRAG_SHADER;
extern const ShaderStageSpecification SELECTION_BOX_VERT_SHADER;
extern const ShaderStageSpecification SELECTION_BOX_FRAG_SHADER;
extern const ShaderStageSpecification BOUNDING_BOX_VERT_SHADER;
extern const ShaderStageSpecification BOUNDING_BOX_FRAG_SHADER;
extern const ShaderStageSpecification CIRCLE_VERT_SHADER;
extern const ShaderStageSpecification CIRCLE_FRAG_SHADER;


} // namespace backend_openGL3
} // namespace render
} // namespace polyscope
