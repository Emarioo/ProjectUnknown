#shader vertex
#version 330 core

const int MAX_BONES = 50;
const int MAX_WEIGHTS = 3;

layout(location = 0) in vec3 vPos;
layout(location = 1) in vec2 vUV;
layout(location = 2) in vec3 vNormal;
layout(location = 3) in ivec3 vBones;
layout(location = 4) in vec3 vWeights;

out vec2 fUV;
flat out vec3 fNormal;

uniform mat4 uProj;
uniform mat4 uBoneTransforms[MAX_BONES];

void main()
{
	vec4 totalLocalPos = vec4(0);
	vec4 totalNormal = vec4(0);
	for (int i = 0; i < MAX_WEIGHTS; i++) {
		if (vBones[i] == -1)// Bad?
			break;

		vec4 localPos = uBoneTransforms[vBones[i]] * vec4(vPos, 1);
		totalLocalPos += localPos * vWeights[i];

		vec4 worldNormal = uBoneTransforms[vBones[i]] * vec4(vNormal, 0);
		totalNormal += worldNormal * vWeights[i];
	}
	gl_Position = uProj * totalLocalPos;// Add transform?
	fNormal = totalNormal.xyz;
	fUV = vUV;
};

#shader fragment
#version 330 core

layout(location = 0) out vec4 oColor;

in vec2 fUV;
flat in vec3 fNormal;

uniform sampler2D u_Textures;

void main()
{
	oColor = texture(u_Textures, fUV);
};