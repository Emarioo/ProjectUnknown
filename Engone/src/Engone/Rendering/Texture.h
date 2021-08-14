#pragma once

class Texture
{
private:
	unsigned int id;
	unsigned char* buffer=nullptr;
	int width=0, height, BPP=0;
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