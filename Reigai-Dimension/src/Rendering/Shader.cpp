#include "Shader.h"

#include <GL/glew.h>

Shader::Shader(){

}
Shader::Shader(const std::string& path)
	: shaderPath(path+".shader") {
	ShaderProgramSource source = ParseShader(shaderPath);
	programID = CreateShader(source.vert, source.frag);
}
ShaderProgramSource Shader::ParseShader(const std::string& filepath){
	std::ifstream file(filepath);
	if (!file) {
		std::cout << "Class Shader - File not found: " << filepath << std::endl;
		return { "", "" };
	}

	enum class ShaderType
	{
		NONE = -1, VERTEX = 0, FRAGMENT = 1, GEOMETRY=2
	};

	std::string line;
	std::stringstream ss[2];
	ShaderType type = ShaderType::NONE;
	while (getline(file, line)) {
		if (line.find("#shader") != std::string::npos)
		{
			if (line.find("vertex") != std::string::npos)
			{
				type = ShaderType::VERTEX;
			}
			else if (line.find("fragment") != std::string::npos)
			{
				type = ShaderType::FRAGMENT;
			}
		}
		else
		{
			ss[(int)type] << line << "\n";
		}
	}
	return { ss[0].str(),ss[1].str() };
}
unsigned int Shader::CreateShader(const std::string& vertexShader, const std::string& fragmentShader){
	unsigned int program = glCreateProgram();
	unsigned int vs = CompileShader(GL_VERTEX_SHADER, vertexShader);
	unsigned int fs = CompileShader(GL_FRAGMENT_SHADER, fragmentShader);

	glAttachShader(program, vs);
	glAttachShader(program, fs);
	glLinkProgram(program);
	glValidateProgram(program);

	glDeleteShader(vs);
	glDeleteShader(fs);

	return program;
}
unsigned int Shader::CompileShader(unsigned int type, const std::string& source){
	unsigned int id = glCreateShader(type);
	const char* src = source.c_str();
	glShaderSource(id, 1, &src, nullptr);
	glCompileShader(id);

	int result;
	glGetShaderiv(id, GL_COMPILE_STATUS, &result);
	if (result == GL_FALSE)
	{
		int length;
		glGetShaderiv(id, GL_INFO_LOG_LENGTH, &length);
		char* message = (char*)alloca(length * sizeof(char));
		glGetShaderInfoLog(id, length, &length, message);
		std::cout << "Failed to compile " << (type == GL_VERTEX_SHADER ? "vertex" : "fragment") << "Shader!" << std::endl;
		std::cout << message << std::endl;
		glDeleteShader(id);
		return 0;
	}

	return id;
}

void Shader::Bind(){
	glUseProgram(programID);
}

void Shader::SetUniform1iv(const std::string& name, int v[]){
	glUniform1iv(GetUniformLocation(name),2,v);
}
void Shader::SetUniform1f(const std::string& name, float f) {
	glUniform1f(GetUniformLocation(name), f);
}
void Shader::SetUniform2f(const std::string& name, float f0,float f1){
	glUniform2f(GetUniformLocation(name),f0,f1);
}
void Shader::SetUniform3f(const std::string& name, float f0, float f1, float f2) {
	glUniform3f(GetUniformLocation(name), f0, f1, f2);
}
void Shader::SetUniform4f(const std::string& name, float f0, float f1,float f2,float f3){
	glUniform4f(GetUniformLocation(name), f0, f1,f2,f3);
}
void Shader::SetUniform1i(const std::string& name, int v){
	glUniform1i(GetUniformLocation(name), v);
}
void Shader::SetUniformMat4fv(const std::string& name,glm::mat4 mat){
	glUniformMatrix4fv(GetUniformLocation(name), 1, GL_FALSE, glm::value_ptr(mat));
}

unsigned int Shader::GetUniformLocation(const std::string& name) {
	if (uniLocations.find(name) != uniLocations.end())
	{
		return uniLocations[name];
	}
	unsigned int loc = glGetUniformLocation(programID, name.c_str());
	uniLocations[name] = loc;
	return loc;
}
