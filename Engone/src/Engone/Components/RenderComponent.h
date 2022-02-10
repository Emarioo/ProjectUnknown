#pragma once

namespace engone {

	class RenderComponent {
	public:
		RenderComponent() = default;

		bool isActive;

		/*
		Matrix which contains the position and rotation the renderer should use when rendering.
		It gets updated before calling the RenderObjects function.
		Changes to this function will not affect anything.
		*/
		glm::mat4 matrix=glm::mat4(1);
		
	};
}