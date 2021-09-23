#pragma once

namespace engone {
	class FrameBuffer {
	public:
		FrameBuffer();
		void Init();
		void Bind();
		void Unbind();

		unsigned int fbo;
		unsigned int texture;
	};
}