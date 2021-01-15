#include "BufferContainer.h"

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <iostream>;
namespace engine {

	BufferContainer::BufferContainer() {

	}
	void BufferContainer::Setup(bool dyna, void* vdata, unsigned int fcount, void* tdata, unsigned int icount) { // fcount in number of floats
		dynamic = dyna;
		glGenVertexArrays(1, &va);
		glBindVertexArray(va);

		glGenBuffers(1, &vb);
		glBindBuffer(GL_ARRAY_BUFFER, vb);
		if (dynamic) {
			glBufferData(GL_ARRAY_BUFFER, fcount * sizeof(float), vdata, GL_DYNAMIC_DRAW);
		} else {
			glBufferData(GL_ARRAY_BUFFER, fcount * sizeof(float), vdata, GL_STATIC_DRAW);
		}
		count = icount;
		glGenBuffers(1, &ib);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ib);
		if (dynamic) {
			glBufferData(GL_ELEMENT_ARRAY_BUFFER, icount * sizeof(unsigned int), tdata, GL_DYNAMIC_DRAW);
		} else {
			glBufferData(GL_ELEMENT_ARRAY_BUFFER, icount * sizeof(unsigned int), tdata, GL_STATIC_DRAW);
		}
	}
	/*
	This function does not quite work yet. Deletes vertex buffer but what about vertex array?
	*/
	void BufferContainer::Clear() {
		glDeleteBuffers(1, &vb);
	}
	void BufferContainer::SetAttrib(unsigned int loc, unsigned int count, unsigned int stride, unsigned int offset) {
		glEnableVertexArrayAttrib(va, loc);
		glVertexAttribPointer(loc, count, GL_FLOAT, GL_FALSE, stride * sizeof(float), (const void*)(offset * sizeof(float)));
	}
	void BufferContainer::SetAttribI(unsigned int loc, unsigned int count, unsigned int stride, unsigned int offset) {
		glEnableVertexArrayAttrib(va, loc);
		glVertexAttribIPointer(loc, count, GL_UNSIGNED_SHORT, stride * sizeof(int), (const void*)(offset * sizeof(int)));
	}
	void BufferContainer::SubIB(unsigned int offset, unsigned int icount, void* data) {
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ib);
		glBufferSubData(GL_ELEMENT_ARRAY_BUFFER, offset, icount * sizeof(unsigned int), data);
	}
	void BufferContainer::SubVB(unsigned int offset, unsigned int count, void* data) {// count in number of floats
		glBindBuffer(GL_ARRAY_BUFFER, vb);
		glBufferSubData(GL_ARRAY_BUFFER, offset, count * sizeof(float), data);
	}
	void BufferContainer::Draw() {
		glBindVertexArray(va);
		if (type == 0) {
			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ib);
			glDrawElements(GL_TRIANGLES, count, GL_UNSIGNED_INT, nullptr);
		} else if (type == 1) {
			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ib);
			glDrawElements(GL_LINES, count, GL_UNSIGNED_INT, nullptr);
		}
	}
}