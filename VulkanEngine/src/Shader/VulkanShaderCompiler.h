#pragma once

#include "../Core/VulkanPCH.h"

class VulkanShaderCompiler
{
public:
	VulkanShaderCompiler();
	~VulkanShaderCompiler();

	static void PreprocessingShaderSource(const std::string& shaderFileName);
	static inline std::string CompileShader(const std::string& shaderFileName)
	{
		std::string spirVShaderName = shaderFileName + ".spv";
		std::string sysCmd = "glslangValidator.exe -V " + shaderFileName;
		system(sysCmd.data());

		return spirVShaderName;
	}
};