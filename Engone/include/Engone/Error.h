#pragma once


#ifndef ENGONE_ERROR_GUARD
#define ENGONE_ERROR_GUARD

#include "Logger.h"

namespace engone {
	enum Error : uint32 {
		ErrorNone = 0,
		ErrorMissingFile = 1, // could also mean that the file is used by another process (application/program)
		ErrorCorruptedFile = 2,
		FileNotFound,
		EndOfFile,
		AllocFailure,

	};
	Logger& operator<<(Logger& log, Error value);
	const char* ToString(Error value);
}
#endif // ENGONE_ERROR_GUARD
#ifdef ENGONE_ERROR_IMPL
namespace engone {
	const char* ToString(Error value) {
		switch (value) {
		case ErrorNone: return "None";
		case ErrorMissingFile: return "MissingFile";
		case ErrorCorruptedFile: return "CorruptedFile";
		}
		return "Unknown Error";
	}
	Logger& operator<<(Logger& log, Error value) {
		log << ToString(value);
		return log;
	}
}
#endif // ENGONE_ERROR_IMPL