R"(
#shader vertex
#version 330 core

layout(location = 0) in vec3 vPos;

uniform mat4 uLightMatrix;
uniform mat4 uTransform;

void main()
{
	gl_Position = uLightMatrix * uTransform * vec4(vPos, 1);
};

#shader fragment
#version 330 core

out vec4 oColor;

void main()
{
	oColor = gl_FragCoord.z*vec4(1);
};
)"