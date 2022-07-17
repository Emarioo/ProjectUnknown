#pragma once

#include <iostream>
#include <functional>
#include <fstream>
#include <sstream>
#include <filesystem>
#include <sys/stat.h>
#include <chrono>
#include <ctime>
#include <random>
#include <thread>

#include <vector>
#include <string>
#include <unordered_map>
#include <map>

#include <Engone/vendor/glm/glm.hpp>
#include <Engone/vendor/glm/gtx/transform.hpp>
#include <Engone/vendor/glm/gtx/intersect.hpp>
#include <Engone/vendor/glm/gtx/matrix_decompose.hpp>
#include <Engone/vendor/glm/gtc/quaternion.hpp>
#include <Engone/vendor/glm/gtc/type_ptr.hpp>
#include <Engone/vendor/glm/gtc/matrix_transform.hpp>

// causes issues with std::numeric_limit::max in reactphysics
#define NOMINMAX
// don't include winsock
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>