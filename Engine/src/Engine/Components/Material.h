#pragma once

#include "glm/glm.hpp"

class Material {
public:
	Material() : 
		diffuse_map("blank"), diffuse_color({1,1,1}), specular({ 0.5,0.5,0.5 }), shininess(0.5) {}
	Material(const std::string& diffuse_map, glm::vec3 diffuse_color, glm::vec3 specular, float shininess)
		: diffuse_map(diffuse_map), diffuse_color(diffuse_color), specular(specular), shininess(shininess) {}

	// More maps?
	std::string diffuse_map;
	glm::vec3 diffuse_color;
	glm::vec3 specular;
	float shininess;

	bool hasError=false;

};