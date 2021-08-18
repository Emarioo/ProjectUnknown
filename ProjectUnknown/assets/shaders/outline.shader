#shader vertex
#version 330 core

layout(location = 0) in vec3 vPos;
layout(location = 1) in vec3 vColor;

uniform mat4 uProj;
uniform mat4 uTransform;

out vec3 fColor;

void main()
{
	gl_Position = uProj * uTransform * vec4(vPos, 1);
	fColor = vColor;
};

#shader fragment
#version 330 core

layout(location = 0) out vec4 oColor;

in vec3 fColor;

void main()
{
	oColor = fColor;
};