#shader vertex
#version 330 core

const in MAX_BONES = 50;
const in MAX_WEIGHTS = 3;

layout(location = 0) in vec3 vPos;
layout(location = 1) in vec4 vColor;
layout(location = 2) in vec3 vNormal;
layout(location = 3) in ivec3 vBones;
layout(location = 4) in vec3 vWeights;

uniform mat4 uProj;
uniform mat4 uBoneMatrices[MAX_BONES];

out vec4 fColor;
flat out vec3 fNormal;

void main()
{
	vec4 totalLocalPos = vec4(0);
	vec4 totalNormal = vec4(0);
	for (int i = 0; i < MAX_WEIGHTS; i++) {
		vec4 localPos = uBoneTransforms[vBones[i]] * vec4(vPos, 1);
		totalLocaPos += localPosition * vWeights[i];

		vec4 worldNormal = uBoneTransforms[vBones[i]] * vec4(vNormal, 0);
		totalNormal += worldNormal * vWeights[i];
	}
	gl_Position = uProj * totalLocalPos;// Add another one?
	fColor = vColor;
	fNormal = totalNormal.xyz;

};

#shader fragment
#version 330 core

layout(location = 0) out vec4 oColor;

in vec4 fColor;
flat in vec3 fNormal;

void main()
{
	
	oColor = fColor;
};