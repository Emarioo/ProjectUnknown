R"(
#shader vertex
#version 430 core

layout(std430, binding = 0) restrict buffer SSBO
{
	float data[];
} buf;
const int bufStride = 3*2+1;

out vec4 fColor;

uniform mat4 uProj;
uniform float uPointSize;

uniform float delta;

// Function from a video on youtube called newtonian particle system. You won't miss it.
vec3 newtonianColor(vec3 velocity){
	const float red = 0.0045 * dot(velocity, velocity);
    const float green = clamp(0.08 * max(velocity.x, max(velocity.y, velocity.z)), 0.2, 0.5);
    const float blue = 0.7 - red;
	return vec3(red,green,blue);
}
vec3 normalColor(vec3 velocity){
	return velocity/5;
}
vec3 warmth(float value) {
	vec3 warm = vec3(0,0,0);

	warm.x = min(2*(0+abs(value-0)),1);
	warm.y = 1.45*min(1.075-2.45*abs(value-0.5),0.8);
	warm.z = min(6*(0.3-abs(value-0.15)),0.94);

	warm.x = clamp(warm.x,0,1);
	warm.y = clamp(warm.y,0,1);
	warm.z = clamp(warm.z,0,1);

	return warm;
}
vec3 getVec(int offset){
	return vec3(
	buf.data[bufStride*gl_VertexID+offset+0],
	buf.data[bufStride*gl_VertexID+offset+1],
	buf.data[bufStride*gl_VertexID+offset+2]);
}
float getFloat(int offset){
	return buf.data[bufStride*gl_VertexID+offset];
}
void setVec(int offset, vec3 value){
	buf.data[bufStride*gl_VertexID+offset]=value.x;
	buf.data[bufStride*gl_VertexID+offset+1]=value.y;
	buf.data[bufStride*gl_VertexID+offset+2]=value.z;
}
void setFloat(int offset, float value){
	buf.data[bufStride*gl_VertexID+offset]=value;
}

void main() {
	// since structs can't have vec3 this is the way I have to do it.
	vec3 pos = getVec(0);
	vec3 vel = getVec(3);
	float lifeTime = getFloat(6);

	vec3 debugColor=vec3(0,0,0);

	lifeTime-=delta;
	if(lifeTime<=0){
		lifeTime=0;
		setFloat(6,lifeTime);
		gl_Position = uProj * vec4(0,0,0,-1);
		return;
	}

	//-- Physics

	vec3 acc = vec3(0,0,0);

	float velDist = sqrt(dot(vel,vel));
	vec3 velDir = vel/velDist;
	if(velDist==0){
		velDir=vec3(0,0,0);
	}

	//-- Formulas v=at, s=vt
	vel += acc*delta;
	pos += vel*delta;

	//pos.y = lifeTime;
	//if(lifeTime==0)
	//	pos.y=-1;

	//-- Setting new data
	setVec(0,pos);
	setVec(3,vel);
	setFloat(6,lifeTime);

	//-- Coloring
	//float warmthValue = clamp(velSqr/60,0,1);
	//fColor = vec4(warmth(warmthValue),1);

	//fColor = vec4(normalColor(vel),1);

	fColor = vec4(newtonianColor(vel),1);
	//if(velDist<0.01)
	//	fColor.w=0;

	if(!(debugColor.x==0&&debugColor.y==0&&debugColor.z==0)){
		fColor = vec4(debugColor,1);
	}

	gl_Position = uProj * vec4(pos,1);
	float size = 1/gl_Position.z;

	gl_PointSize = min(size,10);
};

#shader fragment
#version 430 core

layout(location = 0) out vec4 oColor;

//in vec4 gl_FragCoord;

in vec4 fColor;

const float near = 0.1;
const float far = 400.0;

void main()
{
	//oColor = vec4(1,1,1,1);
	oColor = fColor;
	//float z = gl_FragCoord.z*2.0-1.0;
	//float depth = (2.0*near*far)/(far+near-z*(far-near))/far;
	// is something isn't right it could be because you forget to change far and near plane values
	// while changing the projection matrix in the c++ code.
	
	//oColor = vec4(fColor.xyz,gl_FragCoord.z);

	//oColor = vec4(0,1-depth,0.1,depth);
};
)"