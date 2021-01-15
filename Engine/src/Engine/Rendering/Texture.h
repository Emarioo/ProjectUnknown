#pragma once

#include <string>

class Texture
{
private:
	unsigned int m_RendererID;
	unsigned char* m_LocalBuffer=nullptr;
	int m_Width=0, m_Height, m_BPP=0;
public:
	Texture();
	Texture(const std::string& path);
	void LoadTex(const std::string& path);
	~Texture();

	std::string filepath;

	void Bind(unsigned int slot = 0) const;
	void Unbind() const;

	inline int GetWidth();
	inline int GetHeight();
};