#include "Shader.h"

#include <GL/glew.h>

namespace engine {
	Shader::Shader() {

	}
	Shader::Shader(const std::string& path)
		: shaderPath(path + ".shader") {
		ShaderProgramSource source = ParseShader(shaderPath);
		programID = CreateShader(source.vert, source.frag);
		isInitialized = true;
	}
	void Shader::Init(const std::string& path) {
		shaderPath = path + ".shader";
		ShaderProgramSource source = ParseShader(shaderPath);
		programID = CreateShader(source.vert, source.frag);
		isInitialized = true;
	}
	ShaderProgramSource Shader::ParseShader(const std::string& filepath) {
		std::ifstream file(filepath);
		if (!file) {
			bug::out < bug::RED < "Cannot find '" < filepath < "'\n";
			hasError = true;
			return { "", "" };
		}

		enum class ShaderType {
			NONE = -1, VERTEX = 0, FRAGMENT = 1, GEOMETRY = 2
		};

		std::string line;
		std::stringstream ss[2];
		ShaderType type = ShaderType::NONE;
		while (getline(file, line)) {
			if (line.find("#shader") != std::string::npos) {
				if (line.find("vertex") != std::string::npos) {
					type = ShaderType::VERTEX;
				} else if (line.find("fragment") != std::string::npos) {
					type = ShaderType::FRAGMENT;
				}
			} else {
				ss[(int)type] << line << "\n";
				if (type == ShaderType::VERTEX)
					vertexLineCount++;
				else
					fragmentLineCount++;
			}
		}
		return { ss[0].str(),ss[1].str() };
	}
	unsigned int Shader::CreateShader(const std::string& vertexShader, const std::string& fragmentShader) {
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
	unsigned int Shader::CompileShader(unsigned int type, const std::string& source) {
		unsigned int id = glCreateShader(type);
		const char* src = source.c_str();
		glShaderSource(id, 1, &src, nullptr);
		glCompileShader(id);

		int result;
		glGetShaderiv(id, GL_COMPILE_STATUS, &result);
		if (result == GL_FALSE) {
			int length;
			glGetShaderiv(id, GL_INFO_LOG_LENGTH, &length);
			char* message = (char*)alloca(length * sizeof(char));
			glGetShaderInfoLog(id, length, &length, message);
			bug::out < bug::RED < "Compile error with '" < shaderPath < "' (" < (type == GL_VERTEX_SHADER ? "vertex" : "fragment") < ")" < bug::end;
			
			std::string number;
			for (int i = 0; i < length;i++) {
				if (i == 0||message[i]=='\n'&&i!=length-1) {
					if (message[i] == '\n') {
						i++;
						bug::out <'\n'<bug::RED;
					} else
						bug::out < bug::RED;
					i+=2;
					
					while (message[i]!=')') {
						number += message[i];
						i++;
					}
					if (type == GL_VERTEX_SHADER)
						bug::out < "VS "<(std::stoi(number)+1);
					else
						bug::out < "FS" <(std::stoi(number)+vertexLineCount+2);
					number = "";
					i++;
				}
				
				bug::out < message[i];
			}
			bug::out < bug::end;
			
			glDeleteShader(id);
			return 0;
		}

		return id;
	}

	void Shader::Bind() {
		glUseProgram(programID);
	}
	void Shader::SetFloat(const std::string& name, float f) {
		glUniform1f(GetUniformLocation(name), f);
	}
	void Shader::SetVec2(const std::string& name, glm::vec2 v) {
		glUniform2f(GetUniformLocation(name), v.x, v.y);
	}
	void Shader::SetIVec2(const std::string& name, glm::ivec2 v) {
		glUniform2i(GetUniformLocation(name), v.x, v.y);
	}
	void Shader::SetVec3(const std::string& name, glm::vec3 v) {
		glUniform3f(GetUniformLocation(name),v.x, v.y, v.z);
	}
	void Shader::SetIVec3(const std::string& name, glm::ivec3 v) {
		glUniform3i(GetUniformLocation(name), v.x, v.y, v.z);
	}
	void Shader::SetVec4(const std::string& name, float f0, float f1, float f2, float f3) {
		glUniform4f(GetUniformLocation(name), f0, f1, f2, f3);
	}
	void Shader::SetInt(const std::string& name, int v) {
		glUniform1i(GetUniformLocation(name), v);
	}
	void Shader::SetMatrix(const std::string& name, glm::mat4 mat) {
		glUniformMatrix4fv(GetUniformLocation(name), 1, GL_FALSE, glm::value_ptr(mat));
	}

	unsigned int Shader::GetUniformLocation(const std::string& name) {
		if (uniLocations.find(name) != uniLocations.end()) {
			return uniLocations[name];
		}
		unsigned int loc = glGetUniformLocation(programID, name.c_str());
		uniLocations[name] = loc;
		return loc;
	}
}