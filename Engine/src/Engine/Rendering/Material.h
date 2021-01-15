#pragma once

#include "glm/glm.hpp"

class Material {
public:
	/*Material() : ambient({ 1,0.5,0.31 }), diffuse({ 1.0, 0.5, 0.31 }),
		specular({ 0.5,0.5,0.5 }), shininess(32) {}
	Material(glm::vec3 ambient, glm::vec3 diffuse,
		glm::vec3 specular,float shininess)
	 : ambient(ambient),diffuse(diffuse),
		specular(specular),shininess(shininess){}
	glm::vec3 ambient;
	glm::vec3 diffuse*/

	Material() : diffuse_map1("blank"), specular({ 0.5,0.5,0.5 }), shininess(64) {}
	Material(const std::string& diffuse) : diffuse_map1(diffuse), specular({ 0.5,0.5,0.5 }), shininess(32) {}
	Material(const std::string& diffuse, glm::vec3 specular, float shininess)
		: diffuse_map1(diffuse), specular(specular), shininess(shininess) {}

	// map0 is used for shadows
	std::string diffuse_map1;
	std::string normal_map2;
	glm::vec3 specular;
	float shininess;

};