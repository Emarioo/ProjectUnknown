#include "Engone/Utilities/LogLevels.h"

namespace engone {
	static std::unordered_map<u32, u32> s_map;
	void SetLogLevel(u32 type, u32 level) {
		s_map[type] = level;
	}
	u32 GetLogLevel(u32 type) {
		auto find = s_map.find(type);
		if (find == s_map.end())
			return 0;
		return find->second;
	}
	bool HasLogLevel(u32 type, u32 level) {
		return GetLogLevel(type) & level;
	}
	bool IsLogLevel(u32 type, u32 level) {
		return GetLogLevel(type) == level;
	}
}