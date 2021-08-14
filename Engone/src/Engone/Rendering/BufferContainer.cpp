#include "gonpch.h"

#include "BufferContainer.h"

#include <GL/glew.h>

namespace engine {

	/*
	Only use this if OpenGL has been initialized.
	*/
	BufferContainer::BufferContainer(bool dynamic, void* vertices, unsigned int vertexCount, void* indices, unsigned int indexCount)
		: bufferType(BufferType::Triangle), dynamic(dynamic), vertexCount(vertexCount), indexCount(indexCount) {
		GenBuffers(vertices, indices);
	}
	BufferContainer::~BufferContainer() {
		glDeleteBuffers(1,&vertexArray);
		glDeleteBuffers(1,&vertexBuffer);
		glDeleteBuffers(1,&indexBuffer);
	}
	void BufferContainer::GenBuffers(void* vertices, void* indices) {
		glGenVertexArrays(1, &vertexArray);
		glBindVertexArray(vertexArray);

		glGenBuffers(1, &vertexBuffer);
		glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer);
		if (dynamic) glBufferData(GL_ARRAY_BUFFER, vertexCount * sizeof(float), vertices, GL_DYNAMIC_DRAW);
		else glBufferData(GL_ARRAY_BUFFER, vertexCount * sizeof(float), vertices, GL_STATIC_DRAW);
		
		glGenBuffers(1, &indexBuffer);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBuffer);
		if (dynamic) glBufferData(GL_ELEMENT_ARRAY_BUFFER, indexCount * sizeof(unsigned int), indices, GL_DYNAMIC_DRAW);
		else glBufferData(GL_ELEMENT_ARRAY_BUFFER, indexCount * sizeof(unsigned int), indices, GL_STATIC_DRAW);
	
	}
	void BufferContainer::Init(bool dynamic, void* vertices, unsigned int vertexCount, void* indices, unsigned int indexCount) { // fcount in number of floats
		this->dynamic = dynamic;
		this->vertexCount = vertexCount;
		this->indexCount = indexCount;

		GenBuffers(vertices, indices);
	}
	void BufferContainer::SetAttrib(unsigned int loc, unsigned int count, unsigned int stride, unsigned int offset) {
		glEnableVertexAttribArray(loc);
		//glEnableVertexArrayAttrib(va, loc); old?
		glVertexAttribPointer(loc, count, GL_FLOAT, GL_FALSE, stride * sizeof(float), (const void*)(offset * sizeof(float)));
	}
	void BufferContainer::SetAttribI(unsigned int loc, unsigned int count, unsigned int stride, unsigned int offset) {
		glEnableVertexAttribArray(loc);
		//glEnableVertexArrayAttrib(va, loc); old?
		glVertexAttribIPointer(loc, count, GL_UNSIGNED_SHORT, stride * sizeof(int), (const void*)(offset * sizeof(int)));
	}
	void BufferContainer::ModifyVertices(unsigned int offset, unsigned int icount, void* data) {
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBuffer);
		glBufferSubData(GL_ELEMENT_ARRAY_BUFFER, offset, icount * sizeof(unsigned int), data);
	}
	void BufferContainer::ModifyIndices(unsigned int offset, unsigned int count, void* data) {// count in number of floats
		glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer);
		glBufferSubData(GL_ARRAY_BUFFER, offset, count * sizeof(float), data);
	}
	void BufferContainer::Draw() {
		glBindVertexArray(vertexArray);
		if (bufferType == BufferType::Triangle) {
			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBuffer);
			glDrawElements(GL_TRIANGLES, indexCount, GL_UNSIGNED_INT, nullptr);
		} else if (bufferType == BufferType::Lines) {
			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBuffer);
			glDrawElements(GL_LINES, indexCount, GL_UNSIGNED_INT, nullptr);
		}
	}
}