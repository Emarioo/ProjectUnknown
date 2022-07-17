#pragma once

#include "Engone/Application.h"

namespace engone {
	class System {
	private:
	public:
		System() = default;
		~System() = default;

		virtual void update(UpdateInfo& info) {}

	};
}