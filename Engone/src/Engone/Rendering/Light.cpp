#include "gonpch.h"

#include "Light.h"

namespace engone {
	void Light::Bind(Shader* shader, int index) {

	}
	void DirLight::Bind(Shader* shader, int index) {
		shader->SetVec3("uDirLight.ambient", ambient);
		shader->SetVec3("uDirLight.diffuse", diffuse);
		shader->SetVec3("uDirLight.specular", specular);
		shader->SetVec3("uDirLight.direction", direction);
	}
	void PointLight::Bind(Shader* shader, int index) {
		std::string u = "uPointLights[" + index + (std::string)"].";
		shader->SetVec3(u + "ambient", ambient);
		shader->SetVec3(u + "diffuse", diffuse);
		shader->SetVec3(u + "specular", specular);
		shader->SetVec3(u + "position", position);
		shader->SetFloat(u + "constant", constant);
		shader->SetFloat(u + "linear", linear);
		shader->SetFloat(u + "quadratic", quadratic);
	}
	void SpotLight::Bind(Shader* shader, int index) {
		std::string u = "uSpotLights[" + index + (std::string)"].";
		shader->SetVec3(u + "ambient", ambient);
		shader->SetVec3(u + "diffuse", diffuse);
		shader->SetVec3(u + "specular", specular);
		shader->SetVec3(u + "position", position);
		shader->SetVec3(u + "direction", direction);
		shader->SetFloat(u + "cutOff", cutOff);
		shader->SetFloat(u + "outerCutOff", outerCutOff);
	}
}