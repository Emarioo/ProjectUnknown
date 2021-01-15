#shader vertex
#version 330 core

layout(location = 0) in vec3 vPos;
layout(location = 1) in vec2 vUV;
//layout(location = 2) in vec3 vNormal;

uniform mat4 uProj;
uniform mat4 uTransform;

out vec2 fUV;
//flat out vec3 fNormal;
//out vec3 v_Light;

void main()
{
	gl_Position = uProj * uTransform * vec4(vPos, 1);
	fUV = vUV;
	//fNormal = vNormal;
	//v_Light = vec3(position) - vec3(0.0f, 50.0f, 0.0f);
};

#shader fragment
#version 330 core

layout(location = 0) out vec4 oColor;

in vec2 fUV;
//flat in vec3 fNormal;
//in vec3 v_Light;

uniform sampler2D u_Textures;

void main()
{
	/*vec3 ambient = vec3(0.2f, 0.2f, 0.2f);

	vec3 lightNorm = normalize(v_Light);
	vec3 bright = vec3(1, 1, 1);
	float diff = clamp(dot(lightNorm, v_Normal), 0, 1);

	o_Color = v_Color * (vec4(ambient, 1.f) + vec4(bright*diff, 1));
	*/
	oColor = texture(u_Textures, fUV);
};