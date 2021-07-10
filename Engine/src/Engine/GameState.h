#pragma once

#include <string>

namespace engine {
	void AddState(std::string state);
	void RemoveState(std::string state);
	bool CheckState(std::string state);
}