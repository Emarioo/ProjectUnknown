#include "gonpch.h"

#include "MetaComponent.h"

namespace engine {

	MetaBase::MetaBase(MetaIdentity identity)
	: identity(identity) {};
	template <typename T>
	T* MetaBase::Cast() {
		return dynamic_cast<T*>(this);
	};
	MetaVector::MetaVector(glm::vec3 value)
	: MetaBase(MetaIdentity::Vector),value(value) {}
	MetaFloat::MetaFloat(float value)
	: MetaBase(MetaIdentity::Float),value(value) {}
	MetaString::MetaString(const std::string& value)
		: MetaBase(MetaIdentity::String), value(value) {}

	MetaComponent::MetaComponent() {}
	void MetaComponent::AddMeta(const std::string& name, MetaBase* base) {
		meta[name] = base;
	}
	void MetaComponent::DelMeta(const std::string& name, MetaBase* base) {
		meta.erase(name);
	}
	MetaBase* MetaComponent::GetMeta(const std::string& name) {
		if (meta.count(name) > 0)
			return meta[name];
		return nullptr;
	}
}