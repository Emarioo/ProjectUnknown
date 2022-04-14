
#include "Buffer.h"
#include "../Logger.h"
#include <GL/glew.h>

namespace engone {

	VertexBuffer::VertexBuffer(int _floatCount, float* data) {
		setData(_floatCount, data);
	}
	VertexBuffer::~VertexBuffer() {
		if (id != 0)
			glDeleteBuffers(1,&id);
	}
	void VertexBuffer::setData(int _floatCount, float* data, int offset) {
		if(id==0)
			glGenBuffers(1, &id);

		glBindBuffer(GL_ARRAY_BUFFER, id);
		if (floatCount == 0) {
			glBufferData(GL_ARRAY_BUFFER, _floatCount *sizeof(float), data, GL_DYNAMIC_DRAW);
			floatCount = _floatCount;
		} else if(floatCount >= _floatCount) {
			glBufferSubData(GL_ARRAY_BUFFER, offset, _floatCount*sizeof(float), data);
		} else {
			log::out <<log::RED<< "VertexBuffer: byteSize is limited to "<< _floatCount <<"\n";
			return;
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

	IndexBuffer::IndexBuffer(int _intCount, uint32_t* data) {
		setData(_intCount, data);
	}
	IndexBuffer::~IndexBuffer() {
		if (id != 0)
			glDeleteBuffers(1, &id);
	}
	void IndexBuffer::setData(int _intCount, uint32_t* data, int offset) {
		if (id == 0)
			glGenBuffers(1, &id);

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, id);
		if (intCount==0) {
			glBufferData(GL_ELEMENT_ARRAY_BUFFER, _intCount*sizeof(uint32_t), data, GL_DYNAMIC_DRAW);
			intCount = _intCount;
		} else if (intCount >= _intCount) {
			glBufferSubData(GL_ELEMENT_ARRAY_BUFFER, offset, _intCount * sizeof(uint32_t), data);
		} else {
			log::out << log::RED << "IndexBuffer: byteSize is limited to " << _intCount << "\n";
		}
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
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
		if(id!=0)
			glDeleteVertexArrays(1,&id);
	}
	void VertexArray::addAttribute(int floatSize, int divisor) {
		if (id == 0)
			glGenVertexArrays(1, &id);
		
		if (location == 8) {
			log::out << log::RED << "VertexArray: Limit of 8 locations!\n";
			return;
		}
		
		locationSizes[totalLocation++] = floatSize + (divisor << 4);
		strides[bufferSection] += floatSize;
	}
	void VertexArray::addAttribute(int floatSize, int divisor, VertexBuffer* buffer) {

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
	void VertexArray::addAttribute(int floatSize) {
		addAttribute(floatSize, 0);
	}
	void VertexArray::addAttribute(int floatSize, VertexBuffer* buffer) {
		addAttribute(floatSize, 0, buffer);
	}
	void VertexArray::selectBuffer(int location, VertexBuffer* buffer) {
		buffer->bind();
		glBindVertexArray(id);
		int offset = 0;
		int section = 0;
		while (section < VAO_MAX_BUFFERS) {
			if (startLocations[section] >= location)
				break;
			section++;
		}
		int index = startLocations[section];
		while(offset==strides[section]-locationSizes[startLocations[section]]){
			offset += locationSizes[index];
			index++;
		}
		while(offset<strides[section]) {
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
		if(unbind)
			glBindVertexArray(0);
		else {
			glBindVertexArray(id);
		}
	}
	void VertexArray::draw(IndexBuffer* indexBuffer) {
		glBindVertexArray(id);

		if (indexBuffer != nullptr) {
			if (indexBuffer->id != 0) {
				glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBuffer->id);
				glDrawElements(GL_TRIANGLES, indexBuffer->intCount, GL_UNSIGNED_INT, nullptr);
			}
		} else {
			log::out <<log::RED<< "VertexArray: Must have indexBuffer when drawing!\n";
		}
	}
	void VertexArray::draw(IndexBuffer* indexBuffer, int instanceAmount) {
		glBindVertexArray(id);

		if (indexBuffer != nullptr) {
			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBuffer->id);
			glDrawElementsInstanced(GL_TRIANGLES, indexBuffer->intCount, GL_UNSIGNED_INT, nullptr,instanceAmount);
		} else {
			log::out << log::RED << "VertexArray: indexBuffer required when drawing instances!\n";
		}
	}

	/*
	Only use this if OpenGL has been initialized.
	*/
	Buffer::Buffer(bool dynamic, void* vertices, unsigned int vertexCount, void* indices, unsigned int indexCount)
		: dynamic(dynamic), vertexCount(vertexCount), indexCount(indexCount) {
		GenBuffers(vertices, indices);
	}
	Buffer::~Buffer() {
		Uninit();
	}
	void Buffer::GenBuffers(void* vertices, void* indices) {
		glGenVertexArrays(1, &vertexArray);// vao
		glBindVertexArray(vertexArray);

		glGenBuffers(1, &vertexBuffer);// vbo
		glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer);
		if (dynamic) glBufferData(GL_ARRAY_BUFFER, vertexCount * sizeof(float), vertices, GL_DYNAMIC_DRAW);
		else glBufferData(GL_ARRAY_BUFFER, vertexCount * sizeof(float), vertices, GL_STATIC_DRAW);

		glGenBuffers(1, &indexBuffer);// ibo
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBuffer);
		if (dynamic) glBufferData(GL_ELEMENT_ARRAY_BUFFER, indexCount * sizeof(unsigned int), indices, GL_DYNAMIC_DRAW);
		else glBufferData(GL_ELEMENT_ARRAY_BUFFER, indexCount * sizeof(unsigned int), indices, GL_STATIC_DRAW);

	}
	void Buffer::Init(bool dynamic, void* vertices, unsigned int vertexCount, void* indices, unsigned int indexCount) { // fcount in number of floats
		this->dynamic = dynamic;
		this->vertexCount = vertexCount;
		this->indexCount = indexCount;

		GenBuffers(vertices, indices);
	}
	void Buffer::Uninit() {
		glDeleteBuffers(1, &vertexArray);
		glDeleteBuffers(1, &vertexBuffer);
		glDeleteBuffers(1, &indexBuffer);
	}
	void Buffer::SetAttrib(unsigned int loc, unsigned int count, unsigned int stride, unsigned int offset) {
		glEnableVertexAttribArray(loc);
		//glEnableVertexArrayAttrib(va, loc); old?
		glVertexAttribPointer(loc, count, GL_FLOAT, GL_FALSE, stride * sizeof(float), (const void*)(offset * sizeof(float)));
	}
	void Buffer::SetAttribI(unsigned int loc, unsigned int count, unsigned int stride, unsigned int offset) {
		glEnableVertexAttribArray(loc);
		//glEnableVertexArrayAttrib(va, loc); old?
		glVertexAttribIPointer(loc, count, GL_UNSIGNED_SHORT, stride * sizeof(int), (const void*)(offset * sizeof(int)));
	}
	void Buffer::ModifyVertices(unsigned int offset, unsigned int icount, void* data) {
		glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer);
		glBufferSubData(GL_ARRAY_BUFFER, offset, icount * sizeof(float), data);
	}
	void Buffer::ModifyIndices(unsigned int offset, unsigned int count, void* data) { // count in number of floats
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBuffer);
		glBufferSubData(GL_ELEMENT_ARRAY_BUFFER, offset, count * sizeof(unsigned int), data);
	}
	TriangleBuffer::TriangleBuffer(bool dynamic, void* vertices, unsigned int vertexCount, void* indices, unsigned int indexCount)
		: Buffer(dynamic, vertices, vertexCount, indices, indexCount) {}
	void TriangleBuffer::Draw() {
		if (vertexCount == 0 || indexCount == 0)
			return;
		glBindVertexArray(vertexArray);

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBuffer);
		glDrawElements(GL_TRIANGLES, indexCount, GL_UNSIGNED_INT, nullptr);
	}
	LineBuffer::LineBuffer(bool dynamic, void* vertices, unsigned int vertexCount, void* indices, unsigned int indexCount)
		: Buffer(dynamic, vertices, vertexCount, indices, indexCount) {}
	void LineBuffer::Draw() {
		if (vertexCount == 0 || indexCount == 0)
			return;
		glBindVertexArray(vertexArray);

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBuffer);
		glDrawElements(GL_LINES, indexCount, GL_UNSIGNED_INT, nullptr);
	}
}