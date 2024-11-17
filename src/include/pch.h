#pragma once

// causes issues with std::numeric_limit::max in reactphysics
#define NOMINMAX

// Try to remove some of these libraries
// thread, mutex, chrono, filesystem
// I should use Windows API to implement my own versions. This can be done whenever, no hurry.
#include <iostream>
#include <functional>
#include <fstream>
#include <sstream>
//#include <filesystem>
//#include <sys/stat.h>
#include <chrono>
#include <ctime>
#include <random>
// #include <thread>
// #include <mutex>
#include <vector>
#include <string>
#include <unordered_map>
#include <map>

#include "glm/glm.hpp"
#include "glm/gtx/transform.hpp"
#include "glm/gtx/intersect.hpp"
#include "glm/gtx/matrix_decompose.hpp"
#include "glm/gtc/quaternion.hpp"
#include "glm/gtc/type_ptr.hpp"
#include "glm/gtc/matrix_transform.hpp"

#include <stdint.h>

typedef int64_t i64;
typedef int32_t i32;
typedef int16_t i16;
typedef int8_t i8;

typedef uint64_t u64;
typedef uint32_t u32;
typedef uint16_t u16;
typedef uint8_t u8;
