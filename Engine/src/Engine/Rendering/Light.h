#pragma once

#include "glm/glm.hpp"

namespace engine {
	enum class LightType {
		None,
		Direction,
		Spot,
		Point
	};
	class Light {
	public:
		Light(LightType type) : lightType(type), position({0,0,0}), 
			ambient({ 0.2,0.2,0.2 }), diffuse({ 1,1,1 }), specular({ 1,1,1 }) {}
		Light(LightType type, glm::vec3 position) : lightType(type), position(position), 
			ambient({ 0.2,0.2,0.2 }), diffuse({ 1,1,1 }), specular({ 1,1,1 }) {}
		Light(LightType type, glm::vec3 ambient, glm::vec3 diffuse,glm::vec3 specular) 
			: lightType(type), position({ 0,0,0 }), ambient(ambient), diffuse(diffuse), specular(specular) {}
		Light(LightType type, glm::vec3 position, glm::vec3 ambient, glm::vec3 diffuse,glm::vec3 specular) : 
			lightType(type), position(position), ambient(ambient), diffuse(diffuse), specular(specular) {}
		
		LightType lightType;
		glm::vec3 position;
		glm::vec3 ambient;
		glm::vec3 diffuse;
		glm::vec3 specular;
	};
	class DirLight : public Light {
	public:
		DirLight() : Light(LightType::Direction), direction({-1,-1,-1}) {}
		DirLight(glm::vec3 direction) : Light(LightType::Direction), direction(direction) {}
		DirLight(glm::vec3 direction, glm::vec3 ambient, glm::vec3 diffuse, glm::vec3 specular)
			: Light(LightType::Direction,ambient,diffuse,specular), direction(direction) {}
		glm::vec3 direction;
	};
	class PointLight : public Light {
	public:
		PointLight() : Light(LightType::Point) {}
		PointLight(glm::vec3 position) : Light(LightType::Point,position) {}
		PointLight(glm::vec3 position, glm::vec3 ambient, glm::vec3 diffuse, glm::vec3 specular)
			: Light(LightType::Point,position,ambient, diffuse, specular) {}

		float constant = 1;
		float linear = 0.09;
		float quadratic = 0.032;
	};
	class SpotLight : public Light {
	public:
		SpotLight() : Light(LightType::Spot), direction({ -1,-1,-1}) {}
		SpotLight(glm::vec3 position, glm::vec3 direction) : Light(LightType::Spot,position), direction(direction) {}
		SpotLight(glm::vec3 position, glm::vec3 direction, glm::vec3 ambient, glm::vec3 diffuse, glm::vec3 specular)
			: Light(LightType::Spot,position,ambient, diffuse, specular), direction(direction) {}

		glm::vec3 direction;

		float cutOff = glm::cos(glm::radians(30.f));
		float outerCutOff = glm::cos(glm::radians(35.f));
	};
}