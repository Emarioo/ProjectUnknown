R"(
#shader vertex
#version 330 core

layout(location = 0) in vec2 vPos;
layout(location = 1) in vec2 vUV;

out vec2 fUV;

void main()
{
	gl_Position = vec4(vPos,0, 1);
	fUV = vUV;
};

#shader fragment
#version 330 core

out vec4 oColor;

in vec2 fUV;

uniform sampler2D uTexture;

void main()
{
	oColor = vec4(vec3(texture(uTexture, fUV).r),1);
};
)"