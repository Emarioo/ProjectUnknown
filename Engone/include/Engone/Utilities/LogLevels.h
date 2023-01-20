#pragma once

namespace engone {
	void SetLogLevel(uint32 type, uint32 level);
	uint32 GetLogLevel(uint32 type);
	// uses &
	bool HasLogLevel(uint32 type, uint32 level);
	// uses ==
	bool IsLogLevel(uint32 type, uint32 level);
}