#pragma once

#include <string>
#include "Engine/Engine.h"

namespace Inventory {
	class Item {
	public:
		Item();

		const std::string& GetName();

		void Draw(float x, float y);

		int count;
		std::string name;
	private:
		
	};
}