#shader vertex
#version 330 core

layout(location = 0) in vec3 vPos;
layout(location = 2) in vec3 vNormal;

uniform mat4 uProj;
uniform mat4 uTransform;


void main()
{
	gl_Position = uProj * uTransform * vec4(vPos, 1);
};

#shader fragment
#version 330 core

layout(location = 0) out vec4 oColor;

in vec3 fColor;
uniform vec3 uLightColor;

void main()
{
	oColor = vec4(uLightColor,1);
};