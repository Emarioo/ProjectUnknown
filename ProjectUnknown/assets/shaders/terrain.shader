#shader vertex
#version 330 core

layout(location = 0) in vec3 vPos;
layout(location = 1) in vec3 vColor;
layout(location = 2) in vec2 vUv;

uniform mat4 uProj;
uniform mat4 uTransform;

out vec3 fColor;
out vec2 fUv;

void main()
{
	gl_Position = uProj * uTransform * vec4(vPos, 1);
	fColor = vColor;
	fUv = vUv;
};

#shader fragment
#version 330 core

layout(location = 0) out vec4 o_Color;

in vec3 fColor;
in vec2 fUv;

uniform sampler2D u_Textures;

void main()
{
	o_Color = vec4(fColor,1)*texture(u_Textures,fUv);
};