#pragma once

#include <iostream>
#include <fstream>
#include <string>
#include <sstream>

#include <unordered_map>

#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

struct ShaderProgramSource
{
	std::string vert;
	std::string frag;
};

class Shader
{
public:
	Shader();
	Shader(const std::string& path);
	unsigned int CreateShader(const std::string& vert, const std::string& frag);
	unsigned int CompileShader(const unsigned int, const std::string& src);
	ShaderProgramSource ParseShader(const std::string& filepath);
	void Bind();
	void SetUniform1iv(const std::string& name, int v[]);
	void SetUniform1i(const std::string& name, int v);
	void SetUniform1f(const std::string& name, float f);
	void SetUniform2f(const std::string& name, float f0,float f1);
	void SetUniform3f(const std::string& name, float f0,float f1,float f2);
	void SetUniform4f(const std::string& name, float f0,float f1,float f2,float f3);
	void SetUniformMat4fv(const std::string& name,glm::mat4 v);
	unsigned int GetUniformLocation(const std::string& name);
	std::string shaderPath;
	unsigned int programID;
private:
	char err;
	std::unordered_map<std::string, unsigned int> uniLocations;
};