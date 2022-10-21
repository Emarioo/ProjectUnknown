#include "ProUnk/Shaders/Shaders.h"

namespace prounk {
	//static const char* experimentGLSL = {
	//#include "Shaders/experiment.glsl"
	//};
	const char* particleGLSL = {
	#include "Engone/Shaders/particle.glsl"
		//#include "Engone/Tests/testParticle.glsl"
	};
	const char* combatParticlesGLSL = {
	#include "ProUnk/Shaders/combatParticles.glsl"
	};
	//static const char* depthGLSL = {
	//#include "Shaders/depth.glsl"
	//};
	//static const char* testGLSL = {
	//#include "Shaders/test.glsl"
	//};
	const char* blurGLSL = {
	#include "Engone/Shaders/blur.glsl"
	};
}