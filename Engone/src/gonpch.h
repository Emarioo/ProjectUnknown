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

#include <vector>
#include <string>
#include <unordered_map>
#include <map>

#include <glm/glm.hpp>
#include <glm/gtx/transform.hpp>
#include <glm/gtx/intersect.hpp>
#include <glm/gtx/matrix_decompose.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>

#define WIN32_LEAN_AND_MEAN // don't include winsock
#include <Windows.h>