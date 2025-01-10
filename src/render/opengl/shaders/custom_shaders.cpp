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
        {"a_faceID", DataType::Float},
    },

    // textures
    {},

    // source
R"(
        ${ GLSL_VERSION }$

        in vec3 a_position;
        in vec3 a_normal;
        in float a_faceID;

        out vec3 v_position;
        out vec3 v_normal;
        out float v_faceID;

        uniform mat4 u_modelView;
        uniform mat4 u_projMatrix;
        uniform mat4 u_normalMatrix;
        
        void main()
        {
            gl_Position = u_projMatrix * u_modelView * vec4(a_position, 1.0);
            v_position = a_position;
            v_normal = a_normal;
            v_faceID = a_faceID;
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
        in float v_faceID;
        layout(location = 0) out vec4 o_position;
        layout(location = 1) out vec4 o_normal;
        layout(location = 2) out vec4 o_faceID;

        void main()
        {
            o_position = vec4(v_position, 1.0);
            o_normal = vec4(normalize(v_normal), 1.0);
            o_faceID = vec4(v_faceID, 1.0, 1.0, 1.0);
        }
)"
};


const ShaderStageSpecification TINY_RENDERER_VERT_SHADER = {

    ShaderStageType::Vertex,

    // uniforms
    {
        {"u_model", DataType::Matrix44Float},
        {"u_view", DataType::Matrix44Float},
        {"u_projection", DataType::Matrix44Float},
    }, 

    // attributes
    {
        {"a_position", DataType::Vector3Float},
        {"a_normal", DataType::Vector3Float},
        {"a_texcoord", DataType::Vector2Float},
        {"a_faceID", DataType::Float},
    },

    // textures
    {},

    // source
R"(
        ${ GLSL_VERSION }$

        in vec3 a_position;
        in vec3 a_normal;
        in vec2 a_texcoord;
        in float a_faceID;

        out vec3 v_position;
        out vec3 v_worldNormal;
        out vec2 v_texcoord;
        out float v_faceID;

        uniform mat4 u_model;
        uniform mat4 u_view;
        uniform mat4 u_projection;
 
        void main()
        {
            gl_Position = u_projection * u_view * u_model * vec4(a_position, 1.0);

            v_worldNormal = mat3(transpose(inverse(u_model))) * a_normal;
            v_texcoord = a_texcoord;
            v_faceID = a_faceID;
        }
)"
};

const ShaderStageSpecification TINY_RENDERER_FRAG_SHADER = {
    
    ShaderStageType::Fragment,
    
    // uniforms
    {
        {"u_lightDir", DataType::Vector3Float},
        {"u_baseColorFactor", DataType::Vector4Float},
        {"u_baseColorTexture", DataType::Int},
        {"u_maskColor", DataType::Vector3Float},
    }, 

    // attributes
    {},
    
    // textures 
    {},
 
    // source
R"(
        ${ GLSL_VERSION }$

        in vec3 v_worldNormal;
        in vec2 v_texcoord;
        in float v_faceID;

        layout(location = 0) out vec4 o_fragColor;
        layout(location = 1) out vec4 o_segMask;
        layout(location = 2) out vec4 o_faceID;

        uniform vec3 u_lightDir;
        uniform vec4 u_baseColorFactor;
        uniform sampler2D u_baseColorTexture;
        uniform vec3 u_maskColor;

        const vec3 lightColor = vec3(1.0, 1.0, 1.0);

        void main()
        {
            // ambient
            float ambientStrength = 0.1;
            vec3 ambient = lightColor * ambientStrength;

            // diffuse
            vec3 norm = normalize(v_worldNormal);
            vec3 lightDir = -normalize(u_lightDir);
            float diff = max(dot(norm, lightDir), 0.0);
            vec3 diffuse = diff * lightColor;

            // result
            vec4 baseColor = texture(u_baseColorTexture, v_texcoord) * u_baseColorFactor;
            o_fragColor = clamp(vec4((ambient + diffuse), 1.0) * baseColor, 0.0, 1.0);

            //o_position = vec4(0.0, v_position.y / 5.0 + 0.5, 0.0, 1.0);
            //o_position = vec4(0.0, 0.0, v_position.z / 30.0 + 0.5, 1.0);
            //o_position = vec4(v_position.x, v_position.y, v_position.z, 1.0);

            o_segMask = vec4(u_maskColor, 1.0);
            o_faceID = vec4(v_faceID, 1.0, 1.0, 1.0);
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
