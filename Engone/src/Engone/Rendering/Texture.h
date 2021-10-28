#pragma once

//#if ENGONE_GLFW
#if gone
namespace engone
{
	class Texture
	{
	private:
		unsigned int id;
		unsigned char* buffer = nullptr;
		int width = 0, height, BPP = 0;
	public:
		Texture()=default;
		/*
		Check if the texture's error variable is true
		*/
		Texture(const std::string& path);
		~Texture();
		/*
		@return true if file isn't found
		*/
		bool Init(const std::string& path);
		void Init(int w, int h, void* data);

		std::string filepath;

		void SubData(int x, int y, int w, int h, void* data);

		void Bind(unsigned int slot = 0) const;
		void Unbind() const;

		int GetWidth();
		int GetHeight();

		bool error=false;
	};
}
#endif