#pragma once

//#if ENGONE_GLFW

namespace engone {

	class VertexBuffer {
	private:
		unsigned int id=0;
		uint32_t floatCount=0;
	public:
		VertexBuffer() = default;
		VertexBuffer(int floatCount, float* data);
		~VertexBuffer();

		void setData(int floatCount, float* data, int offset = 0);

		void bind(bool unbind = false);
	};
	class IndexBuffer {
	private:
		unsigned int id=0;
		uint32_t intCount=0;

		friend class VertexArray;
	public:
		IndexBuffer()=default;
		IndexBuffer(int intCount, uint32_t* data);
		~IndexBuffer();

		void setData(int intCount, uint32_t* data, int offset = 0);

		void bind(bool unbind = false);
	};
	class VertexArray {
	private:
		unsigned int id=0;
		uint8_t location=0, attribLocation=0, stride=0;
		uint8_t attribSizes[8];
	public:
		VertexArray() = default;
		~VertexArray();

		void addAttribute(int floatSize, VertexBuffer* buffer =nullptr);
		void addAttribute(int floatSize, int divisor, VertexBuffer* buffer = nullptr);

		void selectBuffer(int location, VertexBuffer* buffer);

		void bind(bool unbind=false);

		void draw(IndexBuffer* indexBuffer=nullptr);
		void draw(IndexBuffer* indexBuffer, int instanceAmount);

	};
	
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
		void Uninit();

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