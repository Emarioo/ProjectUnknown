#include "Engone/Utilities/LogLevels.h"

namespace engone {
	static std::unordered_map<uint32, uint32> s_map;
	void SetLogLevel(uint32 type, uint32 level) {
		s_map[type] = level;
	}
	uint32 GetLogLevel(uint32 type) {
		auto find = s_map.find(type);
		if (find == s_map.end())
			return 0;
		return find->second;
	}
	bool HasLogLevel(uint32 type, uint32 level) {
		return GetLogLevel(type) & level;
	}
	bool IsLogLevel(uint32 type, uint32 level) {
		return GetLogLevel(type) == level;
	}
}