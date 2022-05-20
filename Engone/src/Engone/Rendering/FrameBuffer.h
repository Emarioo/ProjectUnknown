#pragma once

namespace engone {
	class FrameBuffer {
	public:
		FrameBuffer();
		void init();
		void bind();
		void unbind();

		unsigned int fbo=0;
		unsigned int texture=0;
	};
}
