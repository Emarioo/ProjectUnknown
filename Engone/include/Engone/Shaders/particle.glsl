R"(
#shader vertex
#version 430 core

layout(std430, binding = 0) restrict buffer SSBO
{
	float data[];
} buf;
const int bufStride = 6;

out vec4 fColor;

uniform mat4 uProj;
uniform vec3 focusPoint;
uniform float delta;

const float pMass = 10;
const float focusMass = 10;
const float G = 6.674e-11;

// Function from a video on youtube called newtonian particle system. You won't miss it.
vec3 newtonianColor(vec3 velocity){
	const float red = 0.0045 * dot(velocity, velocity);
    const float green = clamp(0.08 * max(velocity.x, max(velocity.y, velocity.z)), 0.2, 0.5);
    const float blue = 0.7 - red;
	return vec3(red,green,blue);
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
void main() {
	// since structs can't have vec3 this is the way I have to do it.
	vec3 pos = vec3(buf.data[bufStride*gl_VertexID],buf.data[bufStride*gl_VertexID+1],buf.data[bufStride*gl_VertexID+2]);
	vec3 vel = vec3(buf.data[bufStride*gl_VertexID+3],buf.data[bufStride*gl_VertexID+4],buf.data[bufStride*gl_VertexID+5]);

	float velSqr=dot(vel,vel);
	float velDist=sqrt(dot(vel,vel));
	vec3 velDir = vel/velDist;
	if (velDist==0)
		velDir=vec3(0,0,0);

	//if(velSqr==0){
	//	int range = 5;
	//	vel.x = gl_VertexID%range-range/2.0;
	//	vel.y = (gl_VertexID/range)%range-range/2.0;
	//	vel.z = (gl_VertexID/range/range)%range-range/2.0;
	//	vel/=range;
	//	velSqr=dot(vel,vel);
	//}

	vec3 dFocus = focusPoint-pos;
	float dFocusSqr = dot(dFocus,dFocus);
	float dFocusDist = sqrt(dot(dFocus,dFocus)); // used by focus dir which is used by gravity field
	vec3 focusDir = dFocus/dFocusDist;
	if(dFocusDist==0)
		focusDir=vec3(0,0,0);

	vec3 acceleration = vec3(0,0,0);

	//vec3 centralAcc = focusDir * (velDist*velDist/(dFocusDist*dFocusDist));
	//vec3 centralAcc = dFocus*(velSqr/dFocusSqr);
	//acceleration+=centralAcc;

	//float gravDiv=max(dFocusSqr,0.1);// prevent explosion
	//vec3 gravitationalAcc = focusDir/gravDiv;
	//acceleration+=gravitationalAcc;

	vec3 dragAcc = focusDir*10;
	acceleration+=dragAcc;
	
	vec3 frictionAcc = -velDir*0.08;
	acceleration+=frictionAcc;

	const float range=2;
	if(dFocusDist>range){
		vel += acceleration*delta;
	}else{
		vel*=0.8;
		vel-=focusDir*(range-dFocusDist)/delta;
	}

	pos += vel*delta;

	dFocus = focusPoint-pos;
	dFocusSqr = dot(dFocus,dFocus);

	buf.data[bufStride*gl_VertexID]=pos.x;
	buf.data[bufStride*gl_VertexID+1]=pos.y;
	buf.data[bufStride*gl_VertexID+2]=pos.z;
	buf.data[bufStride*gl_VertexID+3]=vel.x;
	buf.data[bufStride*gl_VertexID+4]=vel.y;
	buf.data[bufStride*gl_VertexID+5]=vel.z;

	float warmthValue = clamp(velSqr/60,0,1);
	fColor = vec4(warmth(warmthValue),1);

	//fColor = vec4(newtonianColor(vel),1);
	if(velDist<0.01)
		fColor.w=0;

	gl_Position = uProj * vec4(pos,1);
};

#shader fragment
#version 430 core

layout(location = 0) out vec4 oColor;

in vec4 fColor;

void main()
{
	//oColor = vec4(1,1,1,1);
	oColor = fColor;
};
)"