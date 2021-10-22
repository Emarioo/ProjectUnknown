#pragma once

#include "../Rendering/Shader.h"
#include "../Rendering/Buffer.h"
#include "Material.h"

namespace engone {

	class Mesh {
	public:
		Mesh() = default;
		~Mesh() = default;

		std::vector<Material*> materials;
		const char* shaderType = "";
		TriangleBuffer container;
		void Draw() {
			container.Draw();
		}

		bool hasError = false;
	private:

	};
}