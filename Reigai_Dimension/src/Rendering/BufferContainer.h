#pragma once

class BufferContainer {
public:
	/*
	Class for organizing OpenGL functions
	*/
	BufferContainer();
	/*
	Create a buffer with data
	*/
	void Setup(bool dynamic, void* vdata, unsigned int fcount, void* tdata, unsigned int icount);
	/*
	Remove buffer
	*/
	void Clear();
	/*
	*/
	void SetAttrib(unsigned int loc, unsigned int count, unsigned int stride, unsigned int offset);
	/*
	Modifing vertex buffer data
	*/
	void SubVB(unsigned int offset, unsigned int count, void* data);
	/*
	Modifing index buffer data
	*/
	void SubIB(unsigned int offset, unsigned int count, void* data);
	/*
	Draw data in buffer
	*/
	void Draw();
	unsigned int va, vb, ib, count;
	bool dynamic;
	int type=0;// 0 = triangle, 1 = lines
private:
};