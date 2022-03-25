#pragma once

//#if ENGONE_GLFW

namespace engone {
	class FrameBuffer {
	public:
		FrameBuffer();
		void init();
		void bind();
		void unbind();

		unsigned int fbo;
		unsigned int texture;
	};
}
//#endif