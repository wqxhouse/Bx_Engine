//=================================================================================================
//
//  Bx Engine
//  bxs3514 (Xiangshun Bei) @ 2016 - 2018
//
//  All code licensed under the MIT license
//
//================================================================================================

#pragma once

//C++ basic 
#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <functional>
#include <chrono>
#include <math.h>
#include <memory>
#include <algorithm>

//GLFW
#ifdef _WIN32
// TODO: Redefine warnings
// #define VK_USE_PLATFORM_WIN32_KHR
#endif
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include <Math/BitUtils.h>

#include "../Core/VulkanTypeDef.h"
#define DEFAULT_VULKAN_SHADER_PATH "../Resources/shaders/SPIR-V/"

#include "VDeleter.h"