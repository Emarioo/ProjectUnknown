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

#include <vendor/glm/glm.hpp>
#include <vendor/glm/gtx/transform.hpp>
#include <vendor/glm/gtx/intersect.hpp>
#include <vendor/glm/gtx/matrix_decompose.hpp>
#include <vendor/glm/gtc/quaternion.hpp>
#include <vendor/glm/gtc/type_ptr.hpp>
#include <vendor/glm/gtc/matrix_transform.hpp>

#define WIN32_LEAN_AND_MEAN // don't include winsock
#include <Windows.h>