R"(
#shader vertex
#version 430 core

const int ForceTypeNone=0;
const int ForceTypeAttractive=1;
const int ForceTypeRepellent=2;

struct FocalPoint {
	int forceType;
	vec3 position;
	float strength;
	float rangeMin;
	float rangeMax;
	float rangeBlend; 
};

layout(std430, binding = 0) restrict buffer SSBO
{
	float data[];
} buf;
const int bufStride = 3*2;

out vec4 fColor;

const int maxFocalPoints=5;
uniform FocalPoint uFocalPoints[maxFocalPoints];

uniform mat4 uProj;
uniform float uPointSize;
uniform int uClipping;

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

// NOTE: any physical formulas won't be 100% accurate because of poor precision in floats, i think? also just general error when computing physics?

void main() {
	// since structs can't have vec3 this is the way I have to do it.
	vec3 pos = vec3(buf.data[bufStride*gl_VertexID+0],buf.data[bufStride*gl_VertexID+1],buf.data[bufStride*gl_VertexID+2]);
	vec3 vel = vec3(buf.data[bufStride*gl_VertexID+3],buf.data[bufStride*gl_VertexID+4],buf.data[bufStride*gl_VertexID+5]);

	//-- Physics

	vec3 acc = vec3(0,0,0);

	for(int i=0;i<maxFocalPoints;i++){
		FocalPoint focal = uFocalPoints[0];
		if(focal.forceType==ForceTypeNone)
			continue;
		vec3 toFocal = focal.position-pos;
		float focalDist = sqrt(dot(toFocal,toFocal));
		float velDist = sqrt(dot(vel,vel));

		if(focalDist>focal.rangeMin&&focalDist<focal.rangeMax){
			//float blend = 1;
			if(focal.forceType==ForceTypeAttractive){
				acc+=toFocal/focalDist*focal.strength;
			}else if(focal.forceType==ForceTypeRepellent){
				acc-=toFocal/focalDist*focal.strength;
			}
		}
	}

	//-- Euler integration
	//pos += vel*delta + 0.5f*acc*delta*delta;
	///vel += acc*delta;

	//-- Formulas v=at, s=vt
	vel += acc*delta;
	pos += vel*delta;

	//float velSqr=dot(vel,vel);
	//float velDist=sqrt(velSqr);
	//vec3 velDir = vel/velDist;
	//if (velDist==0)
	//	velDir=vec3(0,0,0);

	//if(velSqr==0){
		//vel=vec3(1,0,0);
		//velSqr=1;
		//velDist=1;
	//	int range = 5;
	//	vel.x = gl_VertexID%range-range/2.0;
		//vel.y = (gl_VertexID/range)%range-range/2.0;
		//vel.z = (gl_VertexID/range/range)%range-range/2.0;
		//vel/=range;
		//velSqr=dot(vel,vel);
	//}

	//vec3 dFocus = focusPoint-pos;
	//float dFocusSqr = dot(dFocus,dFocus);
	//float dFocusDist = sqrt(dFocusSqr);
	//vec3 focusDir = dFocus/dFocusDist;
	//if(dFocusDist==0)
	//	focusDir=vec3(0,0,0);

	//vec3 acceleration = vec3(0,0,0);

	//vec3 centralForce = focusDir * (pMass*velDist*velDist/(dFocusDist));
	//float centDiv = max(dFocusSqr,0.1);
	//vec3 centralAcc = dFocus*(velSqr/centDiv);
	//vec3 centralForce = focusDir*pMass*velDist*velDist/dFocusDist;
	//vec3 centralAcc = focusDir*velDist*velDist/dFocusDist;
	//vec3 centralAcc = focusDir*velDist*velDist/dFocusDist;

	//float gravDiv=max(dFocusSqr,0.1);// prevent explosion
	//vec3 gravitationalAcc = focusDir/gravDiv;
	//vec3 gravitationalAcc = focusDir/gravDiv;

	//vec3 dragAcc = focusDir*10;
	//vec3 drag2Acc = focusDir*dFocusSqr/40;
	//vec3 drag3Acc = focusDir*dFocusSqr/40;
	
	//float perpenValue = dot(velDir,focusDir)/(1*1);
	//vec3 frictionAcc = -velDir*(1-perpenValue)*0.08;

	//force+=centralForce;

	//acceleration+=centralAcc;
	//acceleration+=gravitationalAcc;

	//if(focusPoint.y<0)
	//	acceleration+=dragAcc;
	
	//acceleration+=frictionAcc;

	//const float range=0;
	//if(dFocusDist<range){
	//	acceleration=vec3(0,0,0);
	//	force=vec3(0,0,0);
	//	vel*=0.8;
	//	vel-=focusDir*(range-dFocusDist)/delta;
	//}

	//vel += acceleration*delta;
	//pos += vel*delta;

	//-- Setting new data
	buf.data[bufStride*gl_VertexID+0]=pos.x;
	buf.data[bufStride*gl_VertexID+1]=pos.y;
	buf.data[bufStride*gl_VertexID+2]=pos.z;
	buf.data[bufStride*gl_VertexID+3]=vel.x;
	buf.data[bufStride*gl_VertexID+4]=vel.y;
	buf.data[bufStride*gl_VertexID+5]=vel.z;

	//-- Coloring
	//float warmthValue = clamp(velSqr/60,0,1);
	//fColor = vec4(warmth(warmthValue),1);

	//fColor = vec4(normalColor(vel),1);

	fColor = vec4(newtonianColor(vel),1);
	//if(velDist<0.01)
	//	fColor.w=0;

	gl_Position = uProj * vec4(pos,1);
	
	float size = 1/gl_Position.z;

	//int clipped = 0;
	//int clipLevels=100;
	//float clipChance = 1-2*size;
	
	//if(gl_VertexID%clipLevels<clipChance*clipLevels&&1==uClipping){
	//	clipped=1;
	//}

	gl_PointSize = min(size,10);
	//if(clipped==1){
	//	gl_ClipDistance[0]=-1;
	//}else{
	//	gl_ClipDistance[0]=1;
	//}
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