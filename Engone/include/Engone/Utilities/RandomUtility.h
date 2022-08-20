
// requires std::hash which is included in precompiled header

#ifndef ENGONE_RANDOMUTIL_GUARD
#define ENGONE_RANDOMUTIL_GUARD

#ifdef ENGONE_LOGGER
#include "Engone/Utilities/LoggingModule.h"
#endif

namespace engone {
	// A random seet is set by default.
	void SetRandomSeed(uint32_t seed);
	uint32_t GetRandomSeed();
	// between 0 and 1
	double GetRandom();
	uint32_t Random32();
	uint64_t Random64();

	// random uuid of 16 bytes in total, use UUID::New to create one.
	class UUID {
	public:
		// memory is not initialized.
		UUID() = default;
		static UUID New();
		bool operator==(const UUID& uuid) const;
		bool operator!=(const UUID& uuid) const;

		// only intended for 0.
		UUID(const int num);

		// fullVersion as true will return the whole 16 bytes
		// otherwise the first 8 will be returned
		std::string toString(bool fullVersion = false) const;

	private:
		uint64_t data[2];

		friend struct std::hash<engone::UUID>;
	};
#ifdef ENGONE_LOGGER
	Logger& operator<<(Logger& log, UUID value);
#endif
}
template<>
struct std::hash<engone::UUID> {
	std::size_t operator()(const engone::UUID& u) const {
		return std::hash<uint64_t>{}(u.data[0]) ^ (std::hash<uint64_t>{}(u.data[1]) << 1);
	}
};
#endif // ENGONE_RANDOMUTIL_GUARD

#ifdef ENGONE_RANDOMUTIL_IMPL
#undef ENGONE_RANDOMUTIL_IMPL

#ifdef ENGONE_LOGGER
#include "Engone/Utilities/LoggingModule.h"
#endif

namespace engone {
	static std::mt19937 randomGenerator;
	static uint32_t randomSeed = 0;
	static bool seedonce = false;
	static void initGenerator() {
		if (!seedonce) {
			SetRandomSeed((uint32_t)GetSystemTime());
		}
	}
	static std::uniform_real_distribution<double> random_standard(0.0, 1.0);
	static std::uniform_int_distribution<uint32_t> random_32(0, -1);
	void SetRandomSeed(uint32_t seed) {
		seedonce = true;
		randomSeed = seed;
		randomGenerator.seed(seed);
	}
	uint32_t GetRandomSeed() {
		return randomSeed;
	}
	double GetRandom() {
		initGenerator();
		return random_standard(randomGenerator);
	}
	uint32_t Random32() {
		initGenerator();
		return random_32(randomGenerator);
	}
	uint64_t Random64() {
		initGenerator();
		return ((uint64_t)random_32(randomGenerator) << 32)
			| (uint64_t)random_32(randomGenerator);
	}
	UUID UUID::New() {
		uint64_t out[2];
		for (int i = 0; i < 2; i++) {
			out[i] = Random64();
		}
		return *(UUID*)&out;
	}
	UUID::UUID(const int num) {
		// Uncomment debugbreak, run program, check stackframe.
		if (num != 0) DebugBreak();
		assert(("UUID was created from non 0 int which is not allowed. Put a breakpoint here and check stackframe.", num == 0));
		memset(this, 0, sizeof(UUID));
	}
	bool UUID::operator==(const UUID& uuid) const {
		for (int i = 0; i < 2; i++) {
			if (data[i] != uuid.data[i])
				return false;
		}
		return true;
	}
	bool UUID::operator!=(const UUID& uuid) const {
		return !(*this == uuid);
	}
	char toHex(uint8_t num) {
		if (num < 10) return '0' + num;
		else return 'A' + num - 10;
	}
	std::string UUID::toString(bool fullVersion) const {
		char out[2 * sizeof(UUID) + 4 + 1]; // maximum hash string, four dashes, 1 null

		uint8_t* bytes = (uint8_t*)this;

		int write = 0;
		for (int i = 0; i < sizeof(UUID); i++) {
			if (i == sizeof(UUID) / 2 && !fullVersion)
				break;
			if (i == 4 || i == 6 || i == 8 || i == 10)
				out[write++] = '-';
			out[write++] = toHex(bytes[i] & 15);
			out[write++] = toHex(bytes[i] >> 4);
		}
		out[write] = 0; // finish with null terminated char
		return out;
	}
#ifdef ENGONE_LOGGER
	Logger& operator<<(Logger& log, UUID value) {
		return log << value.toString();
	}
#endif
}
#endif // ENGONE_RANDOMUTIL_IMPL