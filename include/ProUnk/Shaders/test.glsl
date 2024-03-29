R"(
#shader vertex
#version 330 core

layout(location = 0) in vec3 vPos;

uniform mat4 uProj;
uniform mat4 uTransform;

void main()
{
	gl_Position = uProj * uTransform * vec4(vPos, 1);
	//gl_Position = vec4(vPos,0, 1);
};

#shader fragment
#version 330 core

layout(location = 0) out vec4 oColor;

void main()
{
	oColor = vec4(1,1,1,1);
};
)"