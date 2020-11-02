#pragma once

class BufferContainer {
public:
	BufferContainer();
	void Setup(bool dynamic, void* vdata, unsigned int fcount, void* tdata, unsigned int icount);
	void Setup(bool dynamic, void* vdata, unsigned int fcount);
	void SetAttrib(unsigned int loc, unsigned int count, unsigned int stride, unsigned int offset);
	void SubVB(unsigned int offset, unsigned int count, void* data);
	void SubIB(unsigned int offset, unsigned int count, void* data);
	void Draw();
	unsigned int va, vb, ib, count;
	bool dynamic;
	int type=0;// 0 = triangle, 1 = lines
private:
};