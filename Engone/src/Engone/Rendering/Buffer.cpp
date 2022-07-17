#include "Engone/Rendering/Buffer.h"
#include "Engone/Logger.h"
#define GLEW_STATIC
#include <GL/glew.h>

namespace engone {

	VertexBuffer::~VertexBuffer() {
		if (id != 0)
			glDeleteBuffers(1, &id);
	}
	void VertexBuffer::setData(uint32_t _floatCount, float* data, uint32_t offset) {
		if (id == 0)
			glGenBuffers(1, &id);

		glBindBuffer(GL_ARRAY_BUFFER, id);
		if (floatCount == 0) {
			glBufferData(GL_ARRAY_BUFFER, _floatCount * sizeof(float), data, GL_DYNAMIC_DRAW);
			floatCount = _floatCount;
		} else if (floatCount >= _floatCount) {
			glBufferSubData(GL_ARRAY_BUFFER, offset, _floatCount * sizeof(float), data);
		} else {
			log::out << log::RED << "VertexBuffer: byteSize is limited to " << _floatCount << "\n";
		}
		glBindBuffer(GL_ARRAY_BUFFER, 0);
	}
	void VertexBuffer::bind(bool unbind) {
		if (id == 0)
			glGenBuffers(1, &id);
		if (unbind)
			glBindBuffer(GL_ARRAY_BUFFER, 0);
		else
			glBindBuffer(GL_ARRAY_BUFFER, id);
	}

	IndexBuffer::~IndexBuffer() {
		if (id != 0)
			glDeleteBuffers(1, &id);
	}
	void IndexBuffer::setData(uint32_t _intCount, uint32_t* data, uint32_t offset) {
		if (id == 0)
			glGenBuffers(1, &id);

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, id);
		if (intCount == 0) {
			glBufferData(GL_ELEMENT_ARRAY_BUFFER, _intCount * sizeof(uint32_t), data, GL_DYNAMIC_DRAW);
			intCount = _intCount;
		} else if (intCount >= _intCount) {
			glBufferSubData(GL_ELEMENT_ARRAY_BUFFER, offset, _intCount * sizeof(uint32_t), data);
		} else {
			log::out << log::RED << "IndexBuffer: byteSize is limited to " << _intCount << "\n";
		}
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
		//int num = glGetError();
		//const GLubyte* okay = glewGetErrorString(num);
		//std::cout << num << "\n";
	}
	void IndexBuffer::bind(bool unbind) {
		if (id == 0)
			glGenBuffers(1, &id);
		if (unbind)
			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
		else
			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, id);
	}

	VertexArray::~VertexArray() {
		if (id != 0)
			glDeleteVertexArrays(1, &id);
	}
	void VertexArray::addAttribute(uint8_t floatSize, uint8_t divisor) {
		if (id == 0)
			glGenVertexArrays(1, &id);

		if (location == 8) {
			log::out << log::RED << "VertexArray: Limit of 8 locations!\n";
			return;
		}

		locationSizes[totalLocation++] = floatSize + (divisor << 4);
		strides[bufferSection] += floatSize;
	}
	void VertexArray::addAttribute(uint8_t floatSize, uint8_t divisor, VertexBuffer* buffer) {
		addAttribute(floatSize, divisor);
		if (location == 8)
			return;

		if (buffer) buffer->bind();
		glBindVertexArray(id);

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

		if (buffer) buffer->bind(0);
		glBindVertexArray(0);
	}
	void VertexArray::addAttribute(uint8_t floatSize) {
		addAttribute(floatSize, (uint8_t)0);
	}
	void VertexArray::addAttribute(uint8_t floatSize, VertexBuffer* buffer) {
		addAttribute(floatSize, 0u, buffer);
	}
	void VertexArray::selectBuffer(uint8_t location, VertexBuffer* buffer) {
		buffer->bind();
		glBindVertexArray(id);
		uint32_t offset = 0;
		uint32_t section = 0;
		while (section < VAO_MAX_BUFFERS) {
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

		buffer->bind(0);
		glBindVertexArray(0);
	}
	void VertexArray::bind(bool unbind) {
		if (unbind)
			glBindVertexArray(0);
		else {
			glBindVertexArray(id);
		}
	}
	void VertexArray::drawLines(IndexBuffer* indexBuffer) {
		glBindVertexArray(id);

		if (bufferSection == 0) {
			log::out << log::RED << "VertexArray: You forgot VBO in addAttribute!\n";
		}

		if (indexBuffer != nullptr) {
			if (indexBuffer->id != 0) {
				glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBuffer->id);

				glDrawElements(GL_LINES, indexBuffer->intCount, GL_UNSIGNED_INT, nullptr);
			}
		} else {
			log::out << log::RED << "VertexArray: Must have indexBuffer when drawing!\n";
		}
	}
	void VertexArray::draw(IndexBuffer* indexBuffer) {
		glBindVertexArray(id);

		if (bufferSection == 0) {
			log::out << log::RED << "VertexArray: You forgot VBO in addAttribute!\n";
		}

		if (indexBuffer != nullptr) {
			if (indexBuffer->id != 0) {
				glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBuffer->id);
				glDrawElements(GL_TRIANGLES, indexBuffer->intCount, GL_UNSIGNED_INT, nullptr);
				//int num = glGetError();
				//const GLubyte* okay = glewGetErrorString(num);
				//std::cout << okay << "\n";
			}
		} else {
			log::out << log::RED << "VertexArray: Must have indexBuffer when drawing!\n";
		}
	}
	void VertexArray::draw(IndexBuffer* indexBuffer, uint32_t instanceAmount) {
		glBindVertexArray(id);

		if (indexBuffer != nullptr) {
			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBuffer->id);
			glDrawElementsInstanced(GL_TRIANGLES, indexBuffer->intCount, GL_UNSIGNED_INT, nullptr, instanceAmount);
		} else {
			log::out << log::RED << "VertexArray: indexBuffer required when drawing instances!\n";
		}
	}
}