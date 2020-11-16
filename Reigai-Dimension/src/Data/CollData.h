#pragma once

#include <string>

/*
Made out of vertexes with vertex coords and quad indecies
*/

class CollData {
public:
	CollData(){}
	CollData(std::string name) 
	: name(name){}
	std::string name;

	std::vector<std::uint16_t> quad; // 4 = quad
	std::vector<std::uint16_t> tri; // 3 = tri
	std::vector<glm::vec3> points;

private:

};