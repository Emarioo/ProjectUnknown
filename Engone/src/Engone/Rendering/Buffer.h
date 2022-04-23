#pragma once

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
#define VAO_MAX_LOCATIONS 8
#define VAO_MAX_BUFFERS 2
	class VertexArray {
	private:
		unsigned int id=0;

		uint8_t totalLocation = 0, location=0, bufferSection=0;
		// max locations
		uint8_t locationSizes[VAO_MAX_LOCATIONS];
		// Buffer section
		uint8_t strides[VAO_MAX_BUFFERS];
		uint8_t startLocations[VAO_MAX_BUFFERS];

	public:
		VertexArray() = default;
		~VertexArray();

		void addAttribute(int floatSize);
		void addAttribute(int floatSize, VertexBuffer* buffer);
		void addAttribute(int floatSize, int divisor);
		void addAttribute(int floatSize, int divisor, VertexBuffer* buffer);

		void selectBuffer(int location, VertexBuffer* buffer);

		void bind(bool unbind=false);

		void drawLines(IndexBuffer* indexBuffer);
		void draw(IndexBuffer* indexBuffer);
		void draw(IndexBuffer* indexBuffer, int instanceAmount);

	};
#if gone
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
#endif
}