#pragma once

namespace engone {
	enum GLObjectType : uint8_t {
		TypeGLObject=1,
		TypeGLBuffer=2,
		TypeVertexBuffer=4,
		TypeIndexBuffer=8,
		TypeShaderBuffer=16,
		TypeVertexArray=32,
	};
	typedef uint8_t GLObjectTypes;
	class GLObject {
	public:
		GLObject() : type(TYPE) {}
		~GLObject() { cleanup(); }

		GLObject(const GLObject&) = delete;
		GLObject& operator=(const GLObject&) = delete;

		virtual void bind() const = 0;
		virtual void unbind() const = 0;
		virtual void cleanup() const = 0;

		bool initialized() const { return m_id != 0; }

		GLObjectTypes type;
		static const GLObjectTypes TYPE = TypeGLObject;
	protected:
		uint32_t m_id = 0;

	};
	// Everything that has to do with memory is in bytes. sizeof(char)
	class GLBuffer : public GLObject {
	public:
		GLBuffer() : type(TYPE) {}
		~GLBuffer() { cleanup(); }

		//virtual void bind() const = 0;
		//virtual void unbind() const = 0;
		//virtual void cleanup() const = 0;

		// Will allocate/reallocate buffer if buffer doesn't exist or size is bigger than previous size.
		// Will replace data in buffer with new data if size is equal or less then previous size.
		// size in bytes, offset in bytes
		virtual void setData(uint32_t size, void* data, uint32_t offset = 0) = 0;
		// Will fill data with data from opengl
		// size in bytes, offset in bytes
		virtual void getData(uint32_t size, void* data, uint32_t offset = 0) = 0;

		void setData(uint32_t glTarget, uint32_t size, void* data, uint32_t offset);
		void setData(uint32_t glTarget, uint32_t size, void* data, uint32_t offset);

		// size in bytes
		uint32_t getSize() const { return m_size; }

		GLObjectTypes type;
		static const GLObjectTypes TYPE = GLObject::TYPE|TypeGLBuffer;
	protected:
		uint32_t m_size;

		friend class VertexArray;
	};

	class VertexBuffer : public GLBuffer {
	public:
		VertexBuffer() : type(TYPE) {}
		//~VertexBuffer() { cleanup(); }

		// These make it possible to cause bad behaviour
		//VertexBuffer(const VertexBuffer& ib) = delete;
		//VertexBuffer& operator=(const VertexBuffer&) = delete;

		void bind() const override;
		void unbind() const override;
		void cleanup() const override;

		void setData(uint32_t size, void* data, uint32_t offset = 0) override;
		void getData(uint32_t size, void* data, uint32_t offset = 0) override;

		GLObjectTypes type;
		static const GLObjectTypes TYPE = GLBuffer::TYPE|TypeVertexBuffer;
	private:

		//friend class VertexArray;
	};
	class IndexBuffer : public GLBuffer {
	public:
		IndexBuffer() : type(TYPE) {}
		//~IndexBuffer() { cleanup(); };

		// These make it possible to cause bad behaviour
		//IndexBuffer(const IndexBuffer& ib) = delete;
		//IndexBuffer& operator=(const IndexBuffer&) = delete;
		void bind() const override;
		void unbind() const override;
		void cleanup() const override;

		void setData(uint32_t size, void* data, uint32_t offset = 0) override;
		void getData(uint32_t size, void* data, uint32_t offset = 0) override;

		GLObjectTypes type;
		static const GLObjectTypes TYPE = GLBuffer::TYPE|TypeIndexBuffer;
	private:

		//friend class VertexArray;
	};
	// Shader Storage Buffer Object
	class ShaderBuffer : public GLBuffer {
	public:
		ShaderBuffer() : type(TYPE) {}
		//~ShaderBuffer();

		// These make it possible to cause bad behaviour
		//ShaderBuffer(const ShaderBuffer&) = delete;
		//ShaderBuffer& operator=(const ShaderBuffer&) = delete;

		void bind() const override;
		void unbind() const override;
		void cleanup() const override;

		void setData(uint32_t size, void* data, uint32_t offset = 0) override;
		void getData(uint32_t size, void* data, uint32_t offset = 0) override;

		void bindBase(int index) const;

		void drawPoints(int count, int bindingIndex);

		GLObjectTypes type;
		static const GLObjectTypes TYPE = GLBuffer::TYPE | TypeShaderBuffer;
	private:

	};
	class VertexArray : public GLObject {
	public:
		VertexArray() = default;
		//~VertexArray();

		// These make it possible to cause bad behaviour
		//VertexArray(const VertexArray& ib) = delete;
		//VertexArray& operator=(const VertexArray&) = delete;

		void bind() const override;
		void unbind() const override;
		void cleanup() const override;

		void addAttribute(uint8_t floatSize);
		void addAttribute(uint8_t floatSize, VertexBuffer* buffer);
		void addAttribute(uint8_t floatSize, uint8_t divisor);
		void addAttribute(uint8_t floatSize, uint8_t divisor, VertexBuffer* buffer);

		// If you are using instanced bufferes
		void selectBuffer(uint8_t location, VertexBuffer* buffer);

		void drawPoints(int count);
		void drawLines(IndexBuffer* indexBuffer);
		void draw(IndexBuffer* indexBuffer);
		void draw(IndexBuffer* indexBuffer, uint32_t instanceAmount);

		static const int MAX_LOCATIONS =  8;
		static const int MAX_BUFFERS =  2;

		GLObjectTypes type;
		static const GLObjectTypes TYPE = GLObject::TYPE|TypeVertexArray;
	private:

		uint8_t totalLocation = 0, location = 0, bufferSection = 0;
		// max locations
		uint8_t locationSizes[MAX_LOCATIONS]{};
		// Buffer section
		uint8_t strides[MAX_BUFFERS]{};
		uint8_t startLocations[MAX_BUFFERS]{};
	};
}