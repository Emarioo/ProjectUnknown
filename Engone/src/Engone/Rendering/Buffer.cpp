#include "gonpch.h"

#include "Buffer.h"
#include "../Logger.h"
#include <GL/glew.h>

namespace engone {

	VertexBuffer::VertexBuffer(int _floatCount, float* data) {
		setData(_floatCount, data);
	}
	VertexBuffer::~VertexBuffer() {
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
			glBufferSubData(GL_ARRAY_BUFFER, 0, _floatCount*sizeof(float), data);
		} else {
			log::out <<log::RED<< "VertexBuffer: byteSize is limited to "<< _floatCount <<"\n";
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
			glBufferSubData(GL_ELEMENT_ARRAY_BUFFER, 0, _intCount * sizeof(uint32_t), data);
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
		glDeleteVertexArrays(1,&id);
	}
	void VertexArray::addAttribute(int floatSize, int divisor, VertexBuffer* buffer) {
		if (id == 0)
			glGenVertexArrays(1, &id);

		attribSizes[attribLocation++] = floatSize + (divisor << 4);
		stride += floatSize;
		if (buffer){
			buffer->bind();
			glBindVertexArray(id);

			int offset = 0;
			for (int i = 0; i < attribLocation; i++) {
				glEnableVertexAttribArray(location);
				
				uint8_t size = attribSizes[i] << 4;
				size = size >> 4;
				uint8_t div = attribSizes[i] >> 4;
				
				//log::out << location << " " << size << " " << div << " " << stride << " " << offset << "\n";

				glVertexAttribPointer(location, size, GL_FLOAT, GL_FALSE, stride*sizeof(float), (void*)(offset * sizeof(float)));
				if(div!=0)
					glVertexAttribDivisor(location, div);
				
				offset += attribSizes[i];
				location++;
			}
			attribLocation = 0;
			stride = 0;
			
			buffer->bind(0);
			glBindVertexArray(0);
		}
	}
	void VertexArray::addAttribute(int floatSize, VertexBuffer* buffer) {
		addAttribute(floatSize, 0, buffer);
	}
	void VertexArray::selectBuffer(int location, VertexBuffer* buffer) {
		buffer->bind();
		glBindVertexArray(id);
		glEnableVertexAttribArray(location);
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