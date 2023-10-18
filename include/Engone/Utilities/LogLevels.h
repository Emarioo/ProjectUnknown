#pragma once

#define ENGONE_IF_LEVEL(LEVEL_TYPE,LEVEL) if (HasLogLevel(LEVEL_TYPE, LEVEL))


namespace engone {
	void SetLogLevel(u32 type, u32 level);
	u32 GetLogLevel(u32 type);
	// uses &
	bool HasLogLevel(u32 type, u32 level);
	// uses ==
	bool IsLogLevel(u32 type, u32 level);
}