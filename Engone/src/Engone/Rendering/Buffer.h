#pragma once

//#if ENGONE_GLFW

namespace engone {

	class Buffer {
	public:
		unsigned int vertexCount, indexCount;

		Buffer() = default;
		Buffer(bool dynamic, void* vertices, unsigned int vertexCount, void* indices, unsigned int indexCount);
		~Buffer();
		/*
		Manual initialization. Useful if you have a global BufferContainer class
		which requires an initialization of OpenGL first.
		*/
		void Init(bool dynamic, void* vertices, unsigned int vertexCount, void* indices, unsigned int indexCount);
		void Deinit();

		void SetAttrib(unsigned int location, unsigned int count, unsigned int stride, unsigned int offset);
		void SetAttribI(unsigned int location, unsigned int count, unsigned int stride, unsigned int offset);
		/*

		*/
		void ModifyVertices(unsigned int offset, unsigned int count, void* data);
		/*

		*/
		void ModifyIndices(unsigned int offset, unsigned int count, void* data);
		/*

		*/
		virtual void Draw() = 0;
	protected:
		unsigned int vertexArray, vertexBuffer, indexBuffer;
		bool dynamic;
		/*
		Used in constructor and Init
		*/
		void GenBuffers(void* vertices, void* indices);
	};
	class LineBuffer : public Buffer {
	public:
		LineBuffer() = default;
		LineBuffer(bool dynamic, void* vertices, unsigned int vertexCount, void* indices, unsigned int indexCount);

		void Draw() override;
	};
	class TriangleBuffer : public Buffer {
	public:
		TriangleBuffer() = default;
		TriangleBuffer(bool dynamic, void* vertices, unsigned int vertexCount, void* indices, unsigned int indexCount);
		
		void Draw() override;
	};
}
//#endif