#include "Engone/Utilities/LogLevels.h"

namespace engone {
	static std::unordered_map<uint32_t, uint32_t> s_map;
	void SetLogLevel(uint32_t type, uint32_t level) {
		s_map[type] = level;
	}
	uint32_t GetLogLevel(uint32_t type) {
		auto find = s_map.find(type);
		if (find == s_map.end())
			return 0;
		return find->second;
	}
	bool HasLogLevel(uint32_t type, uint32_t level) {
		return GetLogLevel(type) & level;
	}
	bool IsLogLevel(uint32_t type, uint32_t level) {
		return GetLogLevel(type) == level;
	}
}