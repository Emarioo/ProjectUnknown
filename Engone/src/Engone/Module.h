#pragma once

namespace engone {
	class Module {
	public:
		int id;

		Module(int id,int priority=0) : id(id), priority(priority) {}
		virtual ~Module();

		virtual void Init();
		virtual void Update(float delta);
		virtual void Render();

		int priority = 0;

		Module* parent=nullptr;
		
		template<class T>
		T* cast() {
			return reinterpret_cast<T*>(this);
		}
	};
}