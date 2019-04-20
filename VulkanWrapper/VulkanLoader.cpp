#include "VulkanLoader.hpp"

#include "VulkanCore.hpp"
#include "VulkanInstance.hpp"
#include "VulkanDevice.hpp"

VULKAN_NAMESPACE_BEGIN

bool VulkanLoader::ensureDynamicLibraryLoaded()
{
	#if defined(VULKAN_LINKING_DYNAMIC)
		if (!sDynamicLibrary.isLibraryLoaded())
		{
			return loadDynamicLibrary();
		}
	#endif

	return true;
}

bool VulkanLoader::ensureExportedFunctionLoaded()
{
	#if defined(VULKAN_LINKING_DYNAMIC)
		if (!sExportedFunctionLoaded)
		{
			return loadExportedFunction();
		}
	#endif

	return true;
}

bool VulkanLoader::ensureGlobalLevelFunctionsLoaded()
{
	#if defined(VULKAN_LINKING_DYNAMIC)
		if (!sGlobalLevelFunctionsLoaded)
		{
			return loadGlobalLevelFunctions();
		}
	#endif

	return true;
}

bool VulkanLoader::ensureInstanceLevelFunctionsLoaded(const VulkanInstance& instance)
{
	#if defined(VULKAN_LINKING_DYNAMIC)
		return loadInstanceLevelFunctions(instance);
	#endif

	return true;
}

bool VulkanLoader::ensureDeviceLevelFunctionsLoaded(const VkDevice& device, const std::vector<const char*>& enabledExtensions)
{
	#if defined(VULKAN_LINKING_DYNAMIC)
		return loadDeviceLevelFunctions(device, enabledExtensions);
	#endif

	return true;
}

bool VulkanLoader::usingDynamicLinking()
{
	#if defined(VULKAN_LINKING_DYNAMIC)
		return true;
	#else
		return false;
	#endif
}

bool VulkanLoader::loadDynamicLibrary()
{
	if (!sDynamicLibrary.loadLibrary())
	{
		VULKAN_LOG_ERROR("Could not connect with a Vulkan Runtime Library");
		return false;
	}

	return true;
}

bool VulkanLoader::loadExportedFunction()
{
	#define VULKAN_EXPORTED_FUNCTION(name) \
		name = (PFN_##name)sDynamicLibrary.loadFunction(#name); \
		if (name == nullptr) \
		{ \
			VULKAN_LOG_ERROR("Could not load exported Vulkan function named : %s\n", #name); \
			return false; \
		}

	#include "ListOfVulkanFunctions.inl"

	sExportedFunctionLoaded = true;

	return true;
}

bool VulkanLoader::loadGlobalLevelFunctions()
{
	#define VULKAN_GLOBAL_LEVEL_FUNCTION(name) \
		name = (PFN_##name)vkGetInstanceProcAddr(nullptr, #name); \
		if (name == nullptr) \
		{ \
			VULKAN_LOG_ERROR("Could not load global level Vulkan function named: %s", #name); \
			return false; \
		}

	#include "ListOfVulkanFunctions.inl"

	sGlobalLevelFunctionsLoaded = true;

	return true;
}

bool VulkanLoader::loadInstanceLevelFunctions(const VulkanInstance& instance)
{
	const VkInstance& instanceHandle = instance.getHandle();

	#define VULKAN_INSTANCE_LEVEL_FUNCTION(name) \
		name = (PFN_##name)vkGetInstanceProcAddr(instanceHandle, #name); \
		if (name == nullptr) \
		{ \
			VULKAN_LOG_ERROR("Could not load instance-level Vulkan function named: %s\n", #name); \
			return false; \
		}

	#define VULKAN_INSTANCE_LEVEL_FUNCTION_FROM_EXTENSION(name, extension) \
		for (auto& enabledExtension : instance.getExtensions()) \
		{ \
			if (std::string(enabledExtension) == std::string(extension)) \
			{ \
				name = (PFN_##name)vkGetInstanceProcAddr(instanceHandle, #name); \
				if (name == nullptr) \
				{ \
					VULKAN_LOG_ERROR("Could not load instance-level Vulkan function named: %s (Extension: %s)\n", #name, #extension); \
					return false; \
				} \
			} \
		}

	#include "ListOfVulkanFunctions.inl"

	sInstanceLevelFunctionsLoaded = true;

	return true;
}

bool VulkanLoader::loadDeviceLevelFunctions(const VkDevice& device, const std::vector<const char*>& enabledExtensions)
{
	// Load core Vulkan API device-level functions
	#define VULKAN_DEVICE_LEVEL_FUNCTION(name) \
		name = (PFN_##name)vkGetDeviceProcAddr(device, #name); \
		if (name == nullptr) \
		{ \
			VULKAN_LOG_ERROR("Could not load device-level Vulkan function named: %s\n", #name); \
			return false; \
		}

	// Load device-level functions from enabled extensions
	#define VULKAN_DEVICE_LEVEL_FUNCTION_FROM_EXTENSION(name, extension) \
		for (auto& enabledExtension : enabledExtensions) \
		{ \
			if (std::string(enabledExtension) == std::string(extension)) \
			{ \
				name = (PFN_##name)vkGetDeviceProcAddr(device, #name); \
				if (name == nullptr) \
				{ \
					VULKAN_LOG_ERROR("Could not load device-level Vulkan function named: %s (Extension: %s)\n", #name, #extension); \
					return false; \
				} \
			} \
		}

	#include "ListOfVulkanFunctions.inl"

	sDeviceLevelFunctionsLoaded = true;

	return true;
}

VulkanDynamicLibrary VulkanLoader::sDynamicLibrary;
bool VulkanLoader::sExportedFunctionLoaded = false;
bool VulkanLoader::sGlobalLevelFunctionsLoaded = false;
bool VulkanLoader::sInstanceLevelFunctionsLoaded = false;
bool VulkanLoader::sDeviceLevelFunctionsLoaded = false;

VULKAN_NAMESPACE_END