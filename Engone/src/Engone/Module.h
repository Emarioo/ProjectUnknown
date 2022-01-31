#pragma once

namespace engone {
	class Module {
	public:
		Module(int priority=0) : priority(priority) {}
		virtual ~Module();

		virtual void Init();
		virtual void Update(float delta);
		virtual void Render();

		int priority = 0;
	};
}