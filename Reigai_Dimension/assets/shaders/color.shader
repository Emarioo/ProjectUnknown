#shader vertex
#version 330 core

struct Light
{
	vec3 position;
	float range;
	vec3 diffuse;
	vec3 attune;
};

layout(location = 0) in vec3 vPos;
layout(location = 1) in vec4 vColor;
layout(location = 2) in vec3 vNormal;

uniform mat4 uProj;
uniform mat4 uTransform;
uniform Light uLights[4];

out vec4 fColor;
out vec3 fPos;
flat out vec3 fNormal;
out Light fLights[4];

void main()
{
	fColor = vColor;
	fPos = vec4(uTransform * vec4(vPos,1)).xyz;

	fNormal = vec4(uTransform * vec4(vPos-vNormal, 1)).xyz-fPos;
	fLights = uLights;

	gl_Position = uProj * uTransform * vec4(vPos, 1);
};

#shader fragment
#version 330 core

struct Light
{
	vec3 position;
	float range;
	vec3 diffuse;
	vec3 attune;
};

layout(location = 0) out vec4 oColor;

in vec4 fColor;
in vec3 fPos;
flat in vec3 fNormal;
in Light fLights[4];

void main()
{
	vec4 ambient = vec4(0.3f, 0.3f, 0.3f,1);
	vec3 alt = vec3(0,0,0);
	
	for (int i = 0; i < 4; i++) {
		float dist = length(fLights[i].position - fPos);
		if (dist<fLights[i].range) {
			//float diff = 1;
			//if (fNormal != vec3(0, 0, 0)) {
			float diff = clamp(dot(fPos-fLights[i].position, fNormal), 0, 1);
			//}
			float att = 1 / (fLights[i].attune.x + dist * (fLights[i].attune.y + fLights[i].attune.z*dist));
			alt = fLights[i].diffuse * diff;// * att;
		}
	}
	//float diff = clamp(dot(normalize(vec3(0,-1,0)), fNormal), 0, 1);

	//oColor = fColor * diff;
	//oColor = vec4(fNormal, 1);
	oColor = fColor*ambient + vec4(fColor.xyz*alt, fColor.w);
};