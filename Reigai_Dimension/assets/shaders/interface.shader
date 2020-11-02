#shader vertex
#version 330 core

layout(location = 0) in vec4 vPos;

uniform vec2 uTransform;
uniform vec4 uColor;

out vec2 fUV;
out vec4 fColor;

void main()
{
	gl_Position = vec4(vPos.xy + uTransform, 0, 1);
	fUV = vPos.zw;
	fColor = uColor;
};

#shader fragment
#version 330 core

layout(location = 0) out vec4 o_Color;

in vec2 fUV;
in vec4 fColor;

uniform sampler2D u_Textures;

void main()
{
	o_Color = fColor * texture(u_Textures, fUV);
};