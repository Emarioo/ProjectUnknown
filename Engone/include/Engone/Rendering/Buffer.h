#pragma once

namespace engone {

	class VertexBuffer {
	public:
		VertexBuffer() {}
		~VertexBuffer();

		// These make it possible to cause bad behaviour
		VertexBuffer(const VertexBuffer& ib) = delete;
		VertexBuffer& operator=(const VertexBuffer&) = delete;

		void setData(uint32_t floatCount, float* data, uint32_t offset = 0);

		void bind(bool unbind = false);
	private:
		unsigned int id = 0;
		uint32_t floatCount = 0;
	};
	class IndexBuffer {
	public:
		IndexBuffer() {}
		~IndexBuffer();

		// These make it possible to cause bad behaviour
		IndexBuffer(const IndexBuffer& ib) = delete;
		IndexBuffer& operator=(const IndexBuffer&) = delete;

		void setData(uint32_t intCount, uint32_t* data, uint32_t offset = 0);

		void bind(bool unbind = false);

	private:
		uint32_t id = 0;
		uint32_t intCount = 0;

		friend class VertexArray;
	};
#define VAO_MAX_LOCATIONS 8
#define VAO_MAX_BUFFERS 2
	class VertexArray {
	public:
		VertexArray() {}
		~VertexArray();

		// These make it possible to cause bad behaviour
		VertexArray(const VertexArray& ib) = delete;
		VertexArray& operator=(const VertexArray&) = delete;

		void addAttribute(uint8_t floatSize);
		void addAttribute(uint8_t floatSize, VertexBuffer* buffer);
		void addAttribute(uint8_t floatSize, uint8_t divisor);
		void addAttribute(uint8_t floatSize, uint8_t divisor, VertexBuffer* buffer);

		// If you are using instanced bufferes
		void selectBuffer(uint8_t location, VertexBuffer* buffer);

		void bind(bool unbind = false);

		void drawLines(IndexBuffer* indexBuffer);
		void draw(IndexBuffer* indexBuffer);
		void draw(IndexBuffer* indexBuffer, uint32_t instanceAmount);
	private:
		uint32_t id = 0;

		uint8_t totalLocation = 0, location = 0, bufferSection = 0;
		// max locations
		uint8_t locationSizes[VAO_MAX_LOCATIONS]{};
		// Buffer section
		uint8_t strides[VAO_MAX_BUFFERS]{};
		uint8_t startLocations[VAO_MAX_BUFFERS]{};
	};
}