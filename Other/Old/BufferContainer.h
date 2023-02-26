#pragma once

namespace engine {

	enum class BufferType : unsigned char {
		Triangle, Lines
	};

	class BufferContainer {
	public:
		unsigned int vertexCount, indexCount;
		BufferType bufferType=BufferType::Triangle;

		/*
		Class for organizing OpenGL functions
		*/
		BufferContainer() = default;
		BufferContainer(bool dynamic, void* vertices, unsigned int vertexCount, void* indices, unsigned int indexCount);
		~BufferContainer();
		/*
		Manual initialization. Useful if you have a global BufferContainer class
		which requires a initialization of OpenGL first.
		*/
		void Init(bool dynamic, void* vertices, unsigned int vertexCount, void* indices, unsigned int indexCount);

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
		void Draw();
	private:
		unsigned int vertexArray, vertexBuffer, indexBuffer;
		bool dynamic;
		/*
		Used in constructor and Init
		*/
		void GenBuffers(void* vertices, void* indices);
	};
}