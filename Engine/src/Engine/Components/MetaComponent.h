#pragma once

#include <unordered_map>
#include <string>
#include <glm/glm.hpp>

namespace engine {
	enum class MetaIdentity {
		Vector,
		Float,
		String
	};
	class MetaBase {
	public:
		MetaIdentity identity;

		MetaBase();
		MetaBase(MetaIdentity);

		template <typename T>
		T* Cast();
	};
	class MetaVector : public MetaBase {
	public:
		
		MetaVector(glm::vec3 value);
		glm::vec3 value;
	};
	class MetaFloat : public MetaBase {
	public:
		MetaFloat(float value);
		float value;
	};
	class MetaString : public MetaBase {
	public:
		MetaString(const std::string& value);
		std::string value;
	};
	class MetaComponent{
	public:
		MetaComponent();

		std::unordered_map<std::string,MetaBase*> meta;
		void AddMeta(const std::string& name, MetaBase* base);
		void DelMeta(const std::string& name, MetaBase* base);
		MetaBase* GetMeta(const std::string& name);
	};
}