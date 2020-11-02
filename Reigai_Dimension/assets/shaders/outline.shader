#shader vertex
#version 330 core

layout(location = 0) in vec3 vPos;

uniform mat4 uProj;
uniform mat4 uTransform;
uniform vec4 uColor;

out vec3 fPos;
out vec4 fColor;

void main()
{
	fPos = vec4(uTransform * vec4(vPos, 1)).xyz;

	gl_Position = uProj * uTransform * vec4(vPos, 1);
	fColor = uColor;
};

#shader fragment
#version 330 core

layout(location = 0) out vec4 oColor;

in vec3 fPos;
in vec4 fColor;

void main()
{
	oColor = fColor;
};