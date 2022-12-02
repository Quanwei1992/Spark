#type vertex
#version 460 core
layout(location = 0) in vec3 a_Position;
layout(location = 1) in vec4 a_Color;
layout(location = 2) in vec2 a_TexCoord;
uniform mat4 u_ViewProjection;
out vec2 v_TexCoord;
out vec4 v_Color;
void main()
{
	v_TexCoord = a_TexCoord;
	v_Color = a_Color;
	gl_Position = u_ViewProjection * vec4(a_Position,1.0);
}

#type fragment
#version 460 core
layout(location = 0) out vec4 color;
in vec2 v_TexCoord;
in vec4 v_Color;
uniform sampler2D u_Texture;
uniform float u_TilingFactor;
void main()
{
	color = texture(u_Texture,v_TexCoord * u_TilingFactor) * v_Color;
}