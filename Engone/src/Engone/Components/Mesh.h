#pragma once

#include "../Rendering/Buffer.h"

namespace engone {

	class Mesh {
	public:
		Mesh() = default;
		~Mesh() = default;

	//	std::vector<Material*> materials;
		std::string shaderType = "";
		TriangleBuffer container;
		void Draw() {
			container.Draw();
		}

		bool hasError = false;
	private:

	};
}