// Copyright 2017-2023, Nicholas Sharp and the Polyscope contributors. https://polyscope.run


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

    {}, // textures

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
    {
    }, 

    { }, // attributes
    
    // textures 
    {
    },
 
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

    {}, // textures

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
    {
    }, 

    { }, // attributes
    
    // textures 
    {
    },
 
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

const ShaderStageSpecification BOUNDING_BOX_VERT_SHADER = {

    ShaderStageType::Vertex,

    // uniforms
    {
        {"u_modelView", DataType::Matrix44Float},
        {"u_projMatrix", DataType::Matrix44Float},
    }, 

    // attributes
    {
        {"a_position", DataType::Vector3Float},
        {"a_color", DataType::Vector3Float},
    },

    {}, // textures

    // source
R"(
        ${ GLSL_VERSION }$

        uniform mat4 u_modelView;
        uniform mat4 u_projMatrix;
        in vec3 a_position;
        in vec3 a_color;
        out vec3 v_color;
        
        void main()
        {
            gl_Position = u_projMatrix * u_modelView * vec4(a_position, 1.0);
            v_color = a_color;
        }
)"
};

const ShaderStageSpecification BOUNDING_BOX_FRAG_SHADER = {
    
    ShaderStageType::Fragment,
    
    // uniforms
    {
    }, 

    { }, // attributes
    
    // textures 
    {
    },
 
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

const ShaderStageSpecification CIRCLE_VERT_SHADER = {

    ShaderStageType::Vertex,

    // uniforms
    {
        {"u_viewProj", DataType::Matrix44Float},
    }, 

    // attributes
    {
        {"a_worldPosition", DataType::Vector3Float},
        {"a_localPosition", DataType::Vector3Float},
        {"a_color", DataType::Vector4Float},
        {"a_thickness", DataType::Float},
        {"a_fade", DataType::Float},
    },

    {}, // textures

    // source
R"(
        ${ GLSL_VERSION }$

        uniform mat4 u_viewProj;
        in vec3 a_worldPosition;
        in vec3 a_localPosition;
        in vec4 a_color;
        in float a_thickness;
        in float a_fade;

        struct VertexOutput
        {
	        vec3 LocalPosition;
	        vec4 Color;
	        float Thickness;
	        float Fade;
        };
        out VertexOutput v_VO;
        
        void main()
        {
	        v_VO.Color = a_color;
	        v_VO.LocalPosition = a_localPosition;
	        v_VO.Thickness = a_thickness;
	        v_VO.Fade = a_fade;

            gl_Position = u_viewProj * vec4(a_worldPosition, 1.0);
        }
)"
};

const ShaderStageSpecification CIRCLE_FRAG_SHADER = {
    
    ShaderStageType::Fragment,
    
    // uniforms
    {
    }, 

    { }, // attributes
    
    // textures 
    {
    },
 
    // source
R"(
        ${ GLSL_VERSION }$

        struct VertexOutput
        {
	        vec3 LocalPosition;
	        vec4 Color;
	        float Thickness;
	        float Fade;
        };
        in VertexOutput v_VO;

        out vec4 o_color;

        void main()
        {
           	// Calculate distance and fill circle with white
	        float distance = 1.0 - length(v_VO.LocalPosition);
	        float circle = smoothstep(0.0, v_VO.Fade, distance);
	        circle *= smoothstep(v_VO.Thickness + v_VO.Fade, v_VO.Thickness, distance);

	        if (circle == 0.0)
		        discard;

	        // Set output color
	        o_color = v_VO.Color;
	        o_color.a *= circle;
        }
)"
};

// clang-format on

} // namespace backend_openGL3
} // namespace render
} // namespace polyscope
