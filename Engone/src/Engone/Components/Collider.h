#pragma once

namespace engone {
	class Collider {
	public:
		Collider() = default;

		float furthestPoint = 0;
		std::vector<std::uint16_t> quad; // 4 = quad
		std::vector<std::uint16_t> tri; // 3 = tri - not supported at the moment
		std::vector<glm::vec3> points;

		bool hasError = false;
	private:

	};
}