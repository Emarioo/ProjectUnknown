R"(
#shader vertex
#version 330 core

layout(location = 0) in vec2 vPos;
layout(location = 1) in vec2 iPos;

void main()
{
	gl_Position = vec4(vPos+iPos,0, 1);
};

#shader fragment
#version 330 core

out vec4 oColor;

void main()
{
	oColor = vec4(1,1,1,1);
};
)"