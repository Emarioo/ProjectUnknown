#pragma once

#include <string>

/*
Made out of vertexes with vertex coords and quad indecies
*/
namespace engine {
	class CollData {
	public:
		CollData() {}

		float furthestPoint;
		std::vector<std::uint16_t> quad; // 4 = quad
		std::vector<std::uint16_t> tri; // 3 = tri - not supported at the moment
		std::vector<glm::vec3> points;

		bool hasError = false;
	private:

	};
}