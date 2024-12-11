// Added by cyh
#include "polyscope/render/opengl/shaders/custom_shaders.h"

namespace polyscope {
namespace render {
namespace backend_openGL3_glfw {

// clang-format off

const ShaderStageSpecification MESH_GBUFFER_VERT_SHADER = {

    ShaderStageType::Vertex,

    // uniforms
    {
        {"u_modelView", DataType::Matrix44Float},
        {"u_projMatrix", DataType::Matrix44Float},
        {"u_normalMatrix", DataType::Matrix44Float},
    }, 

    // attributes
    {
        {"a_position", DataType::Vector3Float},
        {"a_normal", DataType::Vector3Float},
    },

    // textures
    {},

    // source
R"(
        ${ GLSL_VERSION }$

        uniform mat4 u_modelView;
        uniform mat4 u_projMatrix;
        uniform mat4 u_normalMatrix;
        in vec3 a_position;
        in vec3 a_normal;
        out vec3 v_position;
        out vec3 v_normal;
        
        void main()
        {
            gl_Position = u_projMatrix * u_modelView * vec4(a_position, 1.0);
            v_position = a_position;
            v_normal = a_normal;
        }
)"
};

const ShaderStageSpecification MESH_GBUFFER_FRAG_SHADER = {
    
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

        in vec3 v_position;
        in vec3 v_normal;
        layout(location = 0) out vec4 o_position;
        layout(location = 1) out vec4 o_normal;

        void main()
        {
            o_position = vec4(v_position, 1.0);
            o_normal = vec4(normalize(v_normal), 1.0);
        }
)"
};


const ShaderStageSpecification GLTF_VIEWER_VERT_SHADER = {

    ShaderStageType::Vertex,

    // uniforms
    {
        {"u_Model", DataType::Matrix44Float},
        {"u_View", DataType::Matrix44Float},
        {"u_Projection", DataType::Matrix44Float},
    }, 

    // attributes
    {
        {"a_Position", DataType::Vector3Float},
        {"a_Normal", DataType::Vector3Float},
        {"a_TexCoord", DataType::Vector2Float},
    },

    // textures
    {},

    // source
R"(
        ${ GLSL_VERSION }$

        in vec3 a_Position;
        in vec3 a_Normal;
        in vec2 a_TexCoord;

        out vec3 v_Position;
        out vec3 v_WorldNormal;
        out vec2 v_TexCoord;

        uniform mat4 u_Model;
        uniform mat4 u_View;
        uniform mat4 u_Projection;
 
        void main()
        {
            gl_Position = u_Projection * u_View * u_Model * vec4(a_Position, 1.0);

            v_Position = a_Position;
            v_WorldNormal = mat3(transpose(inverse(u_Model))) * a_Normal;
            v_TexCoord = a_TexCoord;
        }
)"
};

const ShaderStageSpecification GLTF_VIEWER_FRAG_SHADER = {
    
    ShaderStageType::Fragment,
    
    // uniforms
    {
        {"u_LightDir", DataType::Vector3Float},
        {"u_BaseColorFactor", DataType::Vector4Float},
        {"u_BaseColorTexture", DataType::Int},
        {"u_MaskColor", DataType::Vector3Float},
    }, 

    // attributes
    {},
    
    // textures 
    {},
 
    // source
R"(
        ${ GLSL_VERSION }$

        in vec3 v_Position;
        in vec3 v_WorldNormal;
        in vec2 v_TexCoord;

        layout(location = 0) out vec4 o_FragColor;
        layout(location = 1) out vec4 o_Position;
        layout(location = 2) out vec4 o_SegMask;

        uniform vec3 u_LightDir;
        uniform vec4 u_BaseColorFactor;
        uniform sampler2D u_BaseColorTexture;
        uniform vec3 u_MaskColor;

        const vec3 lightColor = vec3(1.0, 1.0, 1.0);

        void main()
        {
            // ambient
            float ambientStrength = 0.1;
            vec3 ambient = lightColor * ambientStrength;

            // diffuse
            vec3 norm = normalize(v_WorldNormal);
            vec3 lightDir = -normalize(u_LightDir);
            float diff = max(dot(norm, lightDir), 0.0);
            vec3 diffuse = diff * lightColor;

            // result
            vec4 baseColor = u_BaseColorFactor;
            o_FragColor = clamp(vec4((ambient + diffuse), 1.0) * baseColor, 0.0, 1.0);

            o_Position = vec4(0.0, v_Position.y / 5.0 + 0.5, 0.0, 1.0);
            o_Position = vec4(0.0, 0.0, v_Position.z / 30.0 + 0.5, 1.0);
            o_SegMask = vec4(u_MaskColor, 1.0);
        }
)"
};


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
