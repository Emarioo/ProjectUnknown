
#include "Engone/Rendering/Light.h"

namespace engone {
	void Light::bind(Shader* shader, int index) {

	}
	void DirLight::bind(Shader* shader, int index) {
		shader->setVec3("uDirLight.ambient", ambient);
		shader->setVec3("uDirLight.diffuse", diffuse);
		shader->setVec3("uDirLight.specular", specular);
		shader->setVec3("uDirLight.direction", direction);
	}
	void PointLight::bind(Shader* shader, int index) {
		std::string u = "uPointLights[" + index + (std::string)"].";
		shader->setVec3(u + "ambient", ambient);
		shader->setVec3(u + "diffuse", diffuse);
		shader->setVec3(u + "specular", specular);
		shader->setVec3(u + "position", position);
		shader->setFloat(u + "constant", constant);
		shader->setFloat(u + "linear", linear);
		shader->setFloat(u + "quadratic", quadratic);
	}
	void SpotLight::bind(Shader* shader, int index) {
		std::string u = "uSpotLights[" + index + (std::string)"].";
		shader->setVec3(u + "ambient", ambient);
		shader->setVec3(u + "diffuse", diffuse);
		shader->setVec3(u + "specular", specular);
		shader->setVec3(u + "position", position);
		shader->setVec3(u + "direction", direction);
		shader->setFloat(u + "cutOff", cutOff);
		shader->setFloat(u + "outerCutOff", outerCutOff);
	}
}