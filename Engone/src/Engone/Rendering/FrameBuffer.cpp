#include "Engone/Rendering/FrameBuffer.h"
#define GLEW_STATIC
#include <GL/glew.h>

#include "Engone/Logger.h"
namespace engone {
	FrameBuffer::FrameBuffer() {}
	void FrameBuffer::init() {
		glGenFramebuffers(1, &m_id);

		glGenTextures(1, &m_textureId);
		glBindTexture(GL_TEXTURE_2D, m_textureId);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, 
			1024, 1024, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
		float color[] = { 1,1,1,1 };
		glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, color);

		glBindFramebuffer(GL_FRAMEBUFFER, m_id);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, m_textureId, 0);
		glDrawBuffer(GL_NONE);
		glReadBuffer(GL_NONE);
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
	}
	void FrameBuffer::initBlur(int width, int height) {
		glGenFramebuffers(1, &m_id);

		glGenTextures(1, &m_textureId);
		glBindTexture(GL_TEXTURE_2D, m_textureId);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		//float color[] = { 1,1,1,1 };
		//glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, color);

		glGenRenderbuffers(1, &m_renderBufferId);
		glBindRenderbuffer(GL_RENDERBUFFER, m_renderBufferId);
		glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, width, height);

		bind();
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_textureId, 0);
		glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, m_renderBufferId);

		// NOTE: applying a renderbuffer without creating and setting buffer will cause wierd things do happen with the buffer.
		//		Probably because it expects a depth stencil when it doens't exist.

		int err = glCheckFramebufferStatus(GL_FRAMEBUFFER);
		if (err != GL_FRAMEBUFFER_COMPLETE) {
			log::out << log::RED<<"Error with framebuffer\n";
		}
		unbind();
	}
	void FrameBuffer::resize(int width, int height) {
		log::out << log::RED<<"FrameBuffer::resize - not implemented\n";
		//bindTexture();
		//m_width = width;
		//m_height = height;
		//glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
		// needs to resize render buffer as well

	}
	void FrameBuffer::bind(bool fixViewPort) {
		glBindFramebuffer(GL_DRAW_FRAMEBUFFER, m_id);
		//glViewport(0, 0, m_width, m_height);
	}
	void FrameBuffer::unbind() {
		glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
	}
	void FrameBuffer::blitDepth() {
		glBindFramebuffer(GL_READ_FRAMEBUFFER, m_id);
		glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
		glBlitFramebuffer(0, 0, m_width, m_height,
			0, 0, m_width, m_height,
			GL_DEPTH_BUFFER_BIT, GL_NEAREST);

		glBindFramebuffer(GL_READ_FRAMEBUFFER, 0);
	}
	void FrameBuffer::bindTexture() {
		glBindTexture(GL_TEXTURE_2D, m_textureId);
	}
	void FrameBuffer::bindRenderbuffer() {
		glBindRenderbuffer(GL_RENDERBUFFER, m_renderBufferId);
	}

}
//#endif