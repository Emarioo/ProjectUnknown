#pragma once

#include "../DebugTool/DebugHandler.h"

#define MAX_ENGINE_SHADERS 8
#define MAX_CUSTOM_SHADERS 10

/*
This is the standard value for custom shaders.
#define MAX_CUSTOM_SHADERS 10
*/
namespace ShaderType {
	enum EngineShaderType : unsigned char {
		NONE = MAX_CUSTOM_SHADERS,
		Color,
		ColorBone,
		UV,
		UVBone,
		Interface,
		Outline,
		Depth
	};
}

namespace engine {

	struct ShaderProgramSource {
		std::string vert;
		std::string frag;
	};

	class Shader {
	public:
		Shader();

		Shader(const std::string& path);

		void Init(const std::string& path);
		unsigned int CreateShader(const std::string& vert, const std::string& frag);
		unsigned int CompileShader(const unsigned int, const std::string& src);
		ShaderProgramSource ParseShader(const std::string& filepath);
		void Bind();
		void SetInt(const std::string& name, int i);
		void SetFloat(const std::string& name, float f);
		void SetVec2(const std::string& name, glm::vec2);
		void SetIVec2(const std::string& name, glm::ivec2);
		void SetVec3(const std::string& name, glm::vec3);
		void SetIVec3(const std::string& name, glm::ivec3);
		void SetVec4(const std::string& name, float f0, float f1, float f2, float f3);
		void SetMatrix(const std::string& name, glm::mat4 v);
		
		unsigned int GetUniformLocation(const std::string& name);
		std::string shaderPath;
		unsigned int programID;
		bool hasError = false;
		bool isInitialized = false;
	private:
		char err;
		std::unordered_map<std::string, unsigned int> uniLocations;
		int vertexLineCount,fragmentLineCount;
	};
}