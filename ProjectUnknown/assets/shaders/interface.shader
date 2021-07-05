#shader vertex
#version 330 core

layout(location = 0) in vec4 vPos;

uniform vec2 uTransform;
uniform vec2 uSize;
uniform vec4 uColor;

out vec4 fPos;
out vec4 fColor;

void main()
{
	fPos = vec4(vec2(vPos.x * uSize.x, vPos.y * uSize.y) + uTransform, vPos.z, vPos.w);
	gl_Position = vec4(fPos.xy, 0, 1);
	fColor = uColor;
};

#shader fragment
#version 330 core

layout(location = 0) out vec4 o_Color;

in vec4 fPos;
in vec4 fColor;

uniform sampler2D u_Textures;
uniform vec4 uRenderArea;

void main()
{
	if (fPos.x>uRenderArea.x&&fPos.x<uRenderArea.x+uRenderArea.z&&fPos.y>uRenderArea.y&&fPos.y<uRenderArea.y+uRenderArea.w) {
		o_Color = fColor * texture(u_Textures, fPos.zw);
	}
};