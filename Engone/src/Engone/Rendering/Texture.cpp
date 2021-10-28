#include "gonpch.h"
#if gone
#include "../Handlers/FileHandler.h"
//#if ENGONE_GLFW
#include "Texture.h"

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image/stb_image.h>

#include "GL/glew.h"


namespace engone
{
	/*
	Remember to check if file exists
	Full path required
	*/
	Texture::Texture(const std::string& path)
	{
		Init(path);
	}
	Texture::~Texture()
	{

		// TODO: Dangerous?
		//glDeleteTextures(1, &m_RendererID); // deletes texture when you add it with texturer[name] = Texture(path)
		// deleting textures is not really neccesarry i think?
	}
	bool Texture::Init(const std::string& path)
	{
		if (!engone::FindFile(path)) {
			error = true;
			return true;
		}
		filepath = path;
		if (filepath.length() > 4) {
			stbi_set_flip_vertically_on_load(1);
			buffer = stbi_load(filepath.c_str(), &width, &height, &BPP, 4);

			glGenTextures(1, &id);
			glBindTexture(GL_TEXTURE_2D, id);

			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, buffer);
			glBindTexture(GL_TEXTURE_2D, 0);

			if (buffer)
				stbi_image_free(buffer);
		}
	}
	void Texture::Init(int w, int h, void* data)
	{
		width = w;
		height = h;
		glGenTextures(1, &id);
		glBindTexture(GL_TEXTURE_2D, id);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
	}
	void Texture::SubData(int x, int y, int w, int h, void* data)
	{
		glBindTexture(GL_TEXTURE_2D, id);
		glTexSubImage2D(GL_TEXTURE_2D, 0, x, y, w, h, GL_RGBA, GL_UNSIGNED_BYTE, data);
	}
	void Texture::Bind(unsigned int slot) const
	{
		glActiveTexture(GL_TEXTURE0 + slot);
		glBindTexture(GL_TEXTURE_2D, id);
	}
	void Texture::Unbind() const
	{
		glBindTexture(GL_TEXTURE_2D, 0);

	}
	int Texture::GetWidth()
	{
		return width;
	}
	int Texture::GetHeight()
	{
		return height;
	}
}
#endif