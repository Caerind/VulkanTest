#pragma once

#include "VulkanDynamicLibrary.hpp"

VULKAN_NAMESPACE_BEGIN

class VulkanLoader
{
	public:
		VulkanLoader() = delete;

		static bool ensureDynamicLibraryLoaded();
		static bool ensureExportedFunctionLoaded();
		static bool ensureGlobalLevelFunctionsLoaded();
		static bool ensureInstanceLevelFunctionsLoaded(const VulkanInstance& instance);
		static bool ensureDeviceLevelFunctionsLoaded(const VkDevice& device, const std::vector<const char*>& enabledExtensions); // TODO : enabledExtensions in Device class and pass a Device

		static bool usingDynamicLinking();

	private:
		static bool loadDynamicLibrary();
		static bool loadExportedFunction();
		static bool loadGlobalLevelFunctions();
		static bool loadInstanceLevelFunctions(const VulkanInstance& instance);
		static bool loadDeviceLevelFunctions(const VkDevice& device, const std::vector<const char*>& enabledExtensions); // TODO : enabledExtensions in Device class and pass a Device

	private:
		static VulkanDynamicLibrary sDynamicLibrary;
		static bool sExportedFunctionLoaded;
		static bool sGlobalLevelFunctionsLoaded;
		static bool sInstanceLevelFunctionsLoaded;
		static bool sDeviceLevelFunctionsLoaded;
};

VULKAN_NAMESPACE_END