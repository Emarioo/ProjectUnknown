#include "Engone/Rendering/Buffer.h"
#include "Engone/Logger.h"
#define GLEW_STATIC
#include <GL/glew.h>


#define CHECK()  {int err = glGetError();if(err) log::out << log::RED<<"GLError: "<<err<<" "<<(const char*)glewGetErrorString(err)<<"\n";}
// unknown error can happen if vertex array isn't bound when a vertex buffer is bound
namespace engone {

	//static void CHECK() {
	//	{
	//		int err = glGetError();
	//		if (err)
	//			log::out << log::RED << "GLError: " << err << " " << (const char*)glewGetErrorString(err) << "\n";
	//	}
	//}


	void VertexBuffer::bind() const {
		glBindBuffer(GL_ARRAY_BUFFER, m_id);
	}
	void VertexBuffer::unbind() const {
		glBindBuffer(GL_ARRAY_BUFFER, 0);
	}
	void VertexBuffer::cleanup() {
		if (m_id != 0)
			glDeleteBuffers(1, &m_id);
	}
	void VertexBuffer::setData(uint32_t size, void* data, uint32_t offset) {
		if (m_id == 0)
			glGenBuffers(1, &m_id);

		bind();
		if (size > m_size) {
			glBufferData(GL_ARRAY_BUFFER, size, data, GL_DYNAMIC_DRAW);
			m_size = size;
		}
		else if (size <= m_size) {
			if (data)
				glBufferSubData(GL_ARRAY_BUFFER, offset, size, data);
		}
		CHECK();
		unbind();
	}
	void VertexBuffer::getData(uint32_t size, void* outData, uint32_t offset) {
		if (size == 0) return;
		bind();
		glGetBufferSubData(GL_ARRAY_BUFFER, offset, size, outData);
		CHECK();
		unbind();
	}
	void IndexBuffer::bind() const {
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_id);
	}
	void IndexBuffer::unbind() const {
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	}
	void IndexBuffer::cleanup() {
		if (m_id != 0)
			glDeleteBuffers(1, &m_id);
	}
	void IndexBuffer::setData(uint32_t size, void* data, uint32_t offset) {
		if (m_id == 0)
			glGenBuffers(1, &m_id);

		bind();
		if (size > m_size) {
			glBufferData(GL_ELEMENT_ARRAY_BUFFER, size, data, GL_DYNAMIC_DRAW);
			m_size = size;
		}
		else if (size <= m_size) {
			if (data)
				glBufferSubData(GL_ELEMENT_ARRAY_BUFFER, offset, size, data);
		}
		CHECK();
		unbind();
	}
	void IndexBuffer::getData(uint32_t size, void* outData, uint32_t offset) {
		if (size == 0) return;
		bind();
		glGetBufferSubData(GL_ELEMENT_ARRAY_BUFFER, offset, size, outData);
		unbind();
	}
	void ShaderBuffer::bind() const {
		glBindBuffer(GL_SHADER_STORAGE_BUFFER, m_id);
	}
	void ShaderBuffer::unbind() const {
		glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
	}
	void ShaderBuffer::cleanup() {
		if (m_id != 0) {
			glDeleteBuffers(1, &m_id);
			glDeleteVertexArrays(1, &m_vaId);
		}
	}
	void ShaderBuffer::setData(uint32_t size, void* data, uint32_t offset) {
		if (m_id == 0) {
			glGenBuffers(1, &m_id);
			glGenVertexArrays(1, &m_vaId);
		}

		bind();
		if (size > m_size) {
			glBufferData(GL_SHADER_STORAGE_BUFFER, size, data, GL_DYNAMIC_DRAW);
			m_size = size;
		}
		else if (size <= m_size) {
			if (data)
				glBufferSubData(GL_SHADER_STORAGE_BUFFER, offset, size, data);
		}
		CHECK();
		unbind();
	}
	void ShaderBuffer::getData(uint32_t size, void* outData, uint32_t offset) {
		if (size == 0) return;
		if (!initialized()) {
			log::out << log::RED << "ShaderBuffer::getData - object is uninitialized!\n";
			return;
		}
		bind();
		glGetBufferSubData(GL_SHADER_STORAGE_BUFFER, offset, size, outData);
		CHECK();
		unbind();
	}
	void ShaderBuffer::bindBase(int index) const {
		glBindBufferBase(GL_SHADER_STORAGE_BUFFER, index, m_id);
	}
	void ShaderBuffer::drawPoints(int count, int bindingIndex) {
		bind();
		bindBase(bindingIndex);
		glBindVertexArray(m_vaId);
		CHECK();
		glDrawArrays(GL_POINTS, 0, count);
		CHECK();
		unbind();
		glBindVertexArray(0);
	}
	void VertexArray::addAttribute(uint8_t floatSize, uint8_t divisor) {
		if (!initialized())
			glGenVertexArrays(1, &m_id);

		if (location == 8) {
			log::out << log::RED << "VertexArray::addAttribute Limit of 8 locations!\n";
			return;
		}

		locationSizes[totalLocation++] = floatSize + (divisor << 4);
		strides[bufferSection] += floatSize;
	}
	void VertexArray::addAttribute(uint8_t floatSize, uint8_t divisor, VertexBuffer* buffer) {
		if (!buffer) {
			// this is fine incase you use instancing
			//log::out << log::RED << "VertexArray::addAttribute - buffer was nullptr\n";
			//return;
		}
		addAttribute(floatSize, divisor);
		if (location == 8)
			return;

		if (buffer) buffer->bind();
		bind();

		int offset = 0;
		startLocations[bufferSection] = location;
		while (totalLocation > location) {
			glEnableVertexAttribArray(location);

			uint8_t size = locationSizes[location] << 4;
			size = size >> 4;
			uint8_t div = locationSizes[location] >> 4;

			//log::out << location << " " << size << " " << div << " " << strides[bufferSection] << " " << offset << "\n";

			glVertexAttribPointer(location, size, GL_FLOAT, GL_FALSE, strides[bufferSection] * sizeof(float), (void*)(offset * sizeof(float)));
			if (div != 0)
				glVertexAttribDivisor(location, div);

			offset += locationSizes[location];
			location++;
		}
		bufferSection++;
		CHECK();
		if (buffer) buffer->bind();
		unbind();
	}
	void VertexArray::addAttribute(uint8_t floatSize) {
		addAttribute(floatSize, (uint8_t)0);
	}
	void VertexArray::addAttribute(uint8_t floatSize, VertexBuffer* buffer) {
		addAttribute(floatSize, 0u, buffer);
	}
	void VertexArray::selectBuffer(uint8_t location, VertexBuffer* buffer) {
		if (!initialized()) {
			log::out << log::RED << "VertexArray::selectBuffer - object is uninitialized!\n";
			return;
		}
		if (!buffer) {
			log::out << log::RED << "VertexArray::selectBuffer - cannot select nullptr\n";
			return;
		}
		buffer->bind();
		bind();
		uint32_t offset = 0;
		uint32_t section = 0;
		while (section < MAX_BUFFERS) {
			if (startLocations[section] >= location)
				break;
			section++;
		}
		uint8_t index = startLocations[(int)section];
		while (offset == strides[(int)section] - locationSizes[startLocations[section]]) {
			offset += locationSizes[index];
			index++;
		}
		while (offset < strides[section]) {
			uint8_t size = locationSizes[location] << 4;
			size = size >> 4;
			uint8_t div = locationSizes[location] >> 4;

			//log::out << location << " " << size << " " << strides[section] << " " << offset << "\n";
			glVertexAttribPointer(location, size, GL_FLOAT, GL_FALSE, strides[section] * sizeof(float), (void*)(offset * sizeof(float)));
			offset += size;
			location++;
		}
		CHECK();
		buffer->bind(); // buffer is probably an instance buffer which needs to be bound before drawing. I did bind it a few lines up so may not need to bind again.
		unbind();
	}
	void VertexArray::bind() const {
		glBindVertexArray(m_id);
	}
	void VertexArray::unbind() const {
		glBindVertexArray(0);
	}
	void VertexArray::cleanup() {
		if (m_id != 0)
			glDeleteVertexArrays(1, &m_id);
	}
	void VertexArray::drawLines(IndexBuffer* indexBuffer) {
		if (!initialized()) {
			log::out << log::RED << "VertexArray::drawLines - object is uninitialized!\n";
			return;
		}
		bind();

		if (bufferSection == 0) {
			log::out << log::RED << "VertexArray::drawLines - You forgot VBO in addAttribute!\n";
		}

		if (indexBuffer != nullptr) {
			if (indexBuffer->initialized()) {
				glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBuffer->m_id);

				glDrawElements(GL_LINES, indexBuffer->getSize() / sizeof(uint32_t), GL_UNSIGNED_INT, nullptr);
			}
			else {
				log::out << log::RED << "VertexArray::drawLines - buffer is uninitialized!\n";
			}
		}
		else {
			log::out << log::RED << "VertexArray::drawLines - Must have indexBuffer when drawing!\n";
		}
		CHECK();
		unbind();
	}
	void VertexArray::drawPoints(int count) {
		if (!initialized()) {
			log::out << log::RED << "VertexArray::drawLines - object is uninitialized!\n";
			return;
		}
		bind();

		if (bufferSection == 0) {
			log::out << log::RED << "VertexArray::drawPoints - You forgot VBO in addAttribute!\n";
		}
		//if (vertexBuffer != nullptr) {
		//	if (vertexBuffer->id != 0) {
		//		glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer->id);

		glDrawArrays(GL_POINTS, 0, count);
		//	}
		//}
		//else {
		//	log::out << log::RED << "VertexArray::drawPoints Must have vertexBuffer when drawing!\n";
		//}
		CHECK();
		unbind();
	}
	void VertexArray::draw(IndexBuffer* indexBuffer) {
		if (!initialized()) {
			log::out << log::RED << "VertexArray::drawLines - object is uninitialized!\n";
			return;
		}
		bind();

		if (bufferSection == 0) {
			log::out << log::RED << "VertexArray::draw You forgot VBO in addAttribute!\n";
		}

		if (indexBuffer != nullptr) {
			if (indexBuffer->initialized()) {
				indexBuffer->bind();
				//glDrawElements(GL_TRIANGLES, indexBuffer->getSize(), GL_UNSIGNED_INT, nullptr);
				glDrawElements(GL_TRIANGLES, indexBuffer->getSize()/sizeof(uint32_t), GL_UNSIGNED_INT, nullptr);
			} else {
				log::out << log::RED << "VertexArray::draw - buffer is uninitialized!\n";
			}
		}
		else {
			log::out << log::RED << "VertexArray::draw Must have indexBuffer when drawing!\n";
		}
		CHECK();
		unbind();
	}
	void VertexArray::draw(IndexBuffer* indexBuffer, uint32_t instanceAmount) {
		if (!initialized()) {
			log::out << log::RED << "VertexArray::drawLines - object is uninitialized!\n";
			return;
		}
		bind();

		if (indexBuffer != nullptr) {
			if (indexBuffer->initialized()) {
				indexBuffer->bind();
				glDrawElementsInstanced(GL_TRIANGLES, indexBuffer->getSize() / sizeof(uint32_t), GL_UNSIGNED_INT, nullptr, instanceAmount);
			}
			else {
				log::out << log::RED << "VertexArray::draw - buffer is uninitialized!\n";
			}
		}
		else {
			log::out << log::RED << "VertexArray::draw indexBuffer required when drawing instances!\n";
		}
		CHECK();
		unbind();
	}
	void VertexArray::drawTriangleArray(int vertexCount) {
		if (!initialized()) {
			log::out << log::RED << "VertexArray::drawLines - object is uninitialized!\n";
			return;
		}
		bind();

		glDrawArrays(GL_TRIANGLES, 0, vertexCount);

		CHECK();
		unbind();
	}
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

		m_width = width;
		m_height = height;
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
			log::out << log::RED << "Error with framebuffer\n";
		}
		unbind();
	}
	void FrameBuffer::bind() const {
		glBindFramebuffer(GL_DRAW_FRAMEBUFFER, m_id);
	}
	void FrameBuffer::unbind() const {
		glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
	}
	void FrameBuffer::cleanup() {
		log::out << log::RED << "FrameBuffer:cleanup - not complete!\n";
	}
	void FrameBuffer::resize(int width, int height) {
		log::out << log::RED << "FrameBuffer::resize - not implemented\n";
		//bindTexture();
		//m_width = width;
		//m_height = height;
		//glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
		// needs to resize render buffer as well

	}
	void FrameBuffer::blitDepth(int width, int height) {
		glBindFramebuffer(GL_READ_FRAMEBUFFER, m_id);
		glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
		glBlitFramebuffer(0, 0, m_width, m_height,
			0, 0, width, height,
			GL_DEPTH_BUFFER_BIT, GL_NEAREST);
		//CHECK();

		glBindFramebuffer(GL_READ_FRAMEBUFFER, 0);
	}
	void FrameBuffer::bindTexture() {
		glBindTexture(GL_TEXTURE_2D, m_textureId);
	}
	void FrameBuffer::bindRenderbuffer() {
		glBindRenderbuffer(GL_RENDERBUFFER, m_renderBufferId);
	}
}