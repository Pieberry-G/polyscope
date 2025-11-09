#include "polyscope/render/opengl/shaders/custom_shaders.h"

namespace polyscope {
namespace render {
namespace backend_openGL3_glfw {

// clang-format off

const ShaderStageSpecification SELECTION_BOX_VERT_SHADER = {

    ShaderStageType::Vertex,

    // uniforms
    {
    }, 

    // attributes
    {
        {"a_position", DataType::Vector3Float},
        {"a_color", DataType::Vector3Float},
    },

    // textures
    {},

    // source
R"(
        ${ GLSL_VERSION }$

        in vec3 a_position;
        in vec3 a_color;

        out vec3 v_color;
        
        void main()
        {
            gl_Position = vec4(a_position * 2 - 1, 1.0);
            v_color = a_color;
        }
)"
};

const ShaderStageSpecification SELECTION_BOX_FRAG_SHADER = {
    
    ShaderStageType::Fragment,
    
    // uniforms
    {}, 

    // attributes
    {},
    
    // textures 
    {},
 
    // source
R"(
        ${ GLSL_VERSION }$

        in vec3 v_color;
        layout(location = 0) out vec4 o_color;

        void main()
        {
            o_color = vec4(v_color, 1.0);
        }
)"
};

// clang-format on

} // namespace backend_openGL3
} // namespace render
} // namespace polyscope
