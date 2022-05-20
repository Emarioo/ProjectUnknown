#pragma once

namespace engone {

	class VertexBuffer {
	private:
		unsigned int id=0;
		uint32_t floatCount=0;
	public:
		VertexBuffer() = default;
		VertexBuffer(size_t floatCount, float* data);
		~VertexBuffer();

		void setData(size_t floatCount, float* data, size_t offset = 0);

		void bind(bool unbind = false);
	};
	class IndexBuffer {
	private:
		unsigned int id=0;
		uint32_t intCount=0;

		friend class VertexArray;
	public:
		IndexBuffer()=default;
		IndexBuffer(size_t intCount, uint32_t* data);
		~IndexBuffer();

		void setData(size_t intCount, uint32_t* data, size_t offset = 0);

		void bind(bool unbind = false);
	};
#define VAO_MAX_LOCATIONS 8
#define VAO_MAX_BUFFERS 2
	class VertexArray {
	public:
		VertexArray() = default;
		~VertexArray();

		void addAttribute(uint8_t floatSize);
		void addAttribute(uint8_t floatSize, VertexBuffer* buffer);
		void addAttribute(uint8_t floatSize, uint8_t divisor);
		void addAttribute(uint8_t floatSize, uint8_t divisor, VertexBuffer* buffer);

		void selectBuffer(uint8_t location, VertexBuffer* buffer);

		void bind(bool unbind=false);

		void drawLines(IndexBuffer* indexBuffer);
		void draw(IndexBuffer* indexBuffer);
		void draw(IndexBuffer* indexBuffer, size_t instanceAmount);
	private:
		unsigned int id = 0;

		uint8_t totalLocation = 0, location = 0, bufferSection = 0;
		// max locations
		uint8_t locationSizes[VAO_MAX_LOCATIONS];
		// Buffer section
		uint8_t strides[VAO_MAX_BUFFERS];
		uint8_t startLocations[VAO_MAX_BUFFERS];
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