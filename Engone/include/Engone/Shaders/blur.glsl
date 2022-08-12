R"(
#shader vertex
#version 330 core

layout(location = 0) in vec4 vPos;

out vec2 fUV;

void main()
{
	fUV=vPos.zw;
	//gl_Position = vec4((vPos.x*uSize.x+uPos.x)/uWindow.x*2-1, 1-(vPos.y*uSize.y+uPos.y)/uWindow.y*2, 0, 1);
	//gl_Position = vec4(vPos.x, vPos.y, 0, 1);
	gl_Position = vec4(2*vPos.x-1, 2*vPos.y-1, 0, 1);
	gl_Position = vec4(2*vPos.x-1, 2*vPos.y-1, 0, 1);
};

#shader fragment
#version 330 core

layout(location = 0) out vec4 oColor;

in vec2 fUV;

uniform sampler2D uTexture;
uniform vec2 uInvTextureSize;
//const vec2 uInvTextureSize=vec2(1/1000.f,1/800.f);

void main()
{
	const int kernelSize=3;
	float kernel[kernelSize*kernelSize] = float[](
	1,1,1,
	1,1,1,
	1,1,1

	//-1,1,1,1,-1,
	//1,-1,1,-1,1,
	//1,1,1,1,1,
	//1,-1,1,-1,1,
	//-1,1,1,1,-1

	//-1,-1,-1,
	//-1,9,-1,
	//-1,-1,-1
	);
	vec4 sum = vec4(0,0,0,0);
	float div=0;
	for(int i=0;i<kernelSize*kernelSize;i++){
		vec2 coord = vec2(0,0);
		float ox = i%kernelSize-1;
		float oy = i/kernelSize-1;

		coord.x=fUV.x+ox*uInvTextureSize.x;
		coord.y=fUV.y+oy*uInvTextureSize.y;

		sum += kernel[i]*texture(uTexture,coord);
		div+=kernel[i];
	}
	sum/=div;
	//vec4 tx = texture(uTexture,fUV);
	
	oColor = sum;
	//oColor = vec4(1000/uTextureSize.x,1000/uTextureSize.y,0,1);
	//oColor = vec4(1,1,1,1);
};
)"