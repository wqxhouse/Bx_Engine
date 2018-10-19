#pragma once

//C++ basic 
#include <iostream>
#include <string>
#include <vector>
#include <functional>
#include <chrono>
#include <math.h>
#include <memory>

//GLFW
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

//GLM
#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "VDeleter.h"