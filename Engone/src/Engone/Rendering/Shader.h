#pragma once

//#if ENGONE_GLFW

namespace engone {

	struct ShaderProgramSource {
		std::string vert;
		std::string frag;
	};

	class Shader {
	public:
		Shader()=default;
		/*
		@isSource determines whether the string should be interpreted as file path
		or the source for glsl shader.
		*/
		Shader(const std::string& string, bool isSource);

		bool InitFile(const std::string& path);
		void InitSource(const std::string& source);

		void Bind();
		void SetInt(const std::string& name, int i);
		void SetFloat(const std::string& name, float f);
		void SetVec2(const std::string& name, glm::vec2);
		void SetIVec2(const std::string& name, glm::ivec2);
		void SetVec3(const std::string& name, glm::vec3);
		void SetIVec3(const std::string& name, glm::ivec3);
		void SetVec4(const std::string& name, float f0, float f1, float f2, float f3);
		void SetMatrix(const std::string& name, glm::mat4 v);
		
		std::string filePath;

		bool error = false;
	private:
		unsigned int id;
		unsigned int CreateShader(const std::string& vert, const std::string& frag);
		unsigned int CompileShader(const unsigned int, const std::string& src);

		unsigned int GetUniformLocation(const std::string& name);
		std::unordered_map<std::string, unsigned int> uniLocations;
		int section[3]{1,1,1};

		char err;
	};
}
//#endif