#type vertex
#version 450 core
layout(location = 0) in vec3 a_Position;
layout(location = 1) in vec3 a_Normal;
layout(location = 2) in vec3 a_Tangent;
layout(location = 3) in vec3 a_Bitangent;
layout(location = 4) in vec2 a_TexCoord;

layout(std140,binding=0) uniform Camera
{
	mat4 u_ViewProjection;
};

struct VertexOutput
{
	vec3 Normal;
    vec2 TexCoord;
};

layout(location = 0) out VertexOutput Output;

void main()
{
    Output.Normal = a_Normal;
    Output.TexCoord = a_TexCoord;
	gl_Position = u_ViewProjection * vec4(a_Position,1.0);
}

#type fragment
#version 450 core

layout(location = 0) out vec4 finalColor;

struct VertexOutput
{
	vec3 Normal;
    vec2 TexCoord;
};

layout(location = 0) in VertexOutput Input;
void main()
{
    finalColor = vec4((Input.Normal * 0.5 + 0.5),1.0);
}
