#include "Light.h"
float defaultRange = 20;
glm::vec3 defaultDiffuse = glm::vec3(1,1,1);
glm::vec3 defaultAttune = glm::vec3(0.9,0.1,0.05);
Light::Light(float x,float y,float z)
: position(glm::vec3(x,y,z)), range(defaultRange),diffuse(defaultDiffuse), attune(defaultAttune) {
	
}
Light::Light(float x, float y, float z,float r)
	: position(glm::vec3(x, y, z)), range(r),diffuse(defaultDiffuse), attune(defaultAttune) {

}
Light::Light(float x, float y, float z,float r,float d0,float d1,float d2)
	: position(glm::vec3(x, y, z)), range(r), diffuse(glm::vec3(d0, d1, d2)),attune(defaultAttune) {

}
Light::Light(float x, float y, float z,float r, float d0, float d1, float d2,float a0,float a1,float a2)
	: position(glm::vec3(x, y, z)), range(r), diffuse(glm::vec3(d0,d1,d2)),attune(glm::vec3(a0,a1,a2)) {

}