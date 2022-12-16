#pragma once

namespace engone {
	void SetLogLevel(uint32_t type, uint32_t level);
	uint32_t GetLogLevel(uint32_t type);
	// uses &
	bool HasLogLevel(uint32_t type, uint32_t level);
	// uses ==
	bool IsLogLevel(uint32_t type, uint32_t level);
}