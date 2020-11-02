#include "Texture.h"


#define STB_IMAGE_IMPLEMENTATION
#include <stb_image/stb_image.h>
#include <GL/glew.h>

#include <iostream>

Texture::Texture() {

}
/*
Remember to check if file exists
*/
Texture::Texture(const std::string& path) {
	LoadTex(path);
}
void Texture::LoadTex(const std::string& path)
{
	filepath = path+".png";
	if (filepath.length() > 4) {
		stbi_set_flip_vertically_on_load(1);
		m_LocalBuffer = stbi_load(filepath.c_str(), &m_Width, &m_Height, &m_BPP, 4);

		glGenTextures(1, &m_RendererID);
		glBindTexture(GL_TEXTURE_2D, m_RendererID);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, m_Width, m_Height, 0, GL_RGBA, GL_UNSIGNED_BYTE, m_LocalBuffer);
		glBindTexture(GL_TEXTURE_2D, 0);

		if (m_LocalBuffer)
			stbi_image_free(m_LocalBuffer);
	}
}
Texture::~Texture()
{
	glDeleteTextures(1, &m_RendererID);
}
void Texture::Bind(unsigned int slot) const
{
	glActiveTexture(GL_TEXTURE0+slot);
	glBindTexture(GL_TEXTURE_2D,m_RendererID);
}
void Texture::Unbind() const
{

}
int Texture::GetWidth()
{
	return m_Width;
}
int Texture::GetHeight()
{
	return m_Height;
}