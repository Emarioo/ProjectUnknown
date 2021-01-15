#shader vertex
#version 330 core

const int MAX_BONES = 50;
const int MAX_WEIGHTS = 3;

layout(location = 0) in vec3 vPos;
layout(location = 1) in vec3 vColor;
layout(location = 2) in vec3 vNormal;
layout(location = 3) in vec3 vBones;
layout(location = 4) in vec3 vWeights;

out vec3 fColor;
flat out vec3 fNormal;

uniform mat4 uProj;
uniform mat4 uTransform;
uniform mat4 uBoneTransforms[MAX_BONES];

void main()
{
	vec4 P = vec4(0);
	vec4 N = vec4(0);
	
	for (int i = 0; i < 3; i++) {
		P += (uBoneTransforms[int(vBones[i])] * vec4(vPos,1)) * vWeights[i];
		N += (uBoneTransforms[int(vBones[i])] * vec4(vNormal, 0)) * vWeights[i];
	}
	// Used for the normal
	vec3 fPos = vec4(uTransform * vec4(vPos, 1)).xyz;
	
	fNormal = vec3(uTransform * vec4(vPos - vNormal, 1)) - fPos;;
	fColor = vColor;
	gl_Position = uProj * uTransform * P;
};

#shader fragment
#version 330 core

layout(location = 0) out vec4 oColor;

in vec3 fColor;
flat in vec3 fNormal;

void main()
{
	vec3 ambient = vec3(1.f, 0.5f, 0.25f);
	
	float diff = clamp(dot(normalize(vec3(0, -1, 0)), fNormal), 0, 1);

	oColor = vec4(fColor,1);
};