#pragma once

#include "Collider.h"

namespace engone {
	class System {
	private:
	public:
		System() = default;
		~System() = default;

		virtual void OnUpdate(float delta) {}
		virtual void OnCollision(Collider& c1, Collider& c2) {};

	};
}