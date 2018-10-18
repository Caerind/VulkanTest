#include "VulkanLoader.hpp"

#include "VulkanInstance.hpp"
#include "VulkanDevice.hpp"

namespace nu
{
namespace Vulkan
{

bool Loader::ensureDynamicLibraryLoaded()
{
	#if defined(VK_NO_PROTOTYPES)
		if (!sDynamicLibrary.isLibraryLoaded())
		{
			return loadDynamicLibrary();
		}
	#endif

	return true;
}

bool Loader::ensureExportedFunctionLoaded()
{
	#if defined(VK_NO_PROTOTYPES)
		if (!sExportedFunctionLoaded)
		{
			return loadExportedFunction();
		}
	#endif

	return true;
}

bool Loader::ensureGlobalLevelFunctionsLoaded()
{
	#if defined(VK_NO_PROTOTYPES)
		if (!sGlobalLevelFunctionsLoaded)
		{
			return loadGlobalLevelFunctions();
		}
	#endif

	return true;
}

bool Loader::ensureInstanceLevelFunctionsLoaded(const Instance& instance)
{
	#if defined(VK_NO_PROTOTYPES)
		return loadInstanceLevelFunctions(instance);
	#endif

	return true;
}

bool Loader::ensureDeviceLevelFunctionsLoaded(const VkDevice& device, const std::vector<const char*>& enabledExtensions)
{
	#if defined(VK_NO_PROTOTYPES)
		return loadDeviceLevelFunctions(device, enabledExtensions);
	#endif

	return true;
}

bool Loader::usingDynamicLinking()
{
	#if defined(VK_NO_PROTOTYPES)
		return true;
	#else
		return false;
	#endif
}

bool Loader::loadDynamicLibrary()
{
	#if defined(NU_PLATFORM_WINDOWS)
		sDynamicLibrary.loadLibrary("vulkan-1.dll");
	#elif defined(NU_PLATFORM_LINUX)
		sDynamicLibrary.loadLibrary("libvulkan.so.1");
	#endif

	if (!sDynamicLibrary.isLibraryLoaded())
	{
		// TODO : Numea Log System
		printf("Could not connect with a Vulkan Runtime Library\n");
		return false;
	}

	return true;
}

bool Loader::loadExportedFunction()
{
	#define NU_EXPORTED_VULKAN_FUNCTION(name) \
		name = (PFN_##name)sDynamicLibrary.loadFunction(#name); \
		if (name == nullptr) \
		{ \
			printf("Could not load exported Vulkan function named : %s\n", #name); \
			return false; \
		}

	// TODO : Numea Log System	

	#include "ListOfVulkanFunctions.inl"

	sExportedFunctionLoaded = true;

	return true;
}

bool Loader::loadGlobalLevelFunctions()
{
	#define NU_GLOBAL_LEVEL_VULKAN_FUNCTION(name) \
		name = (PFN_##name)vkGetInstanceProcAddr(nullptr, #name); \
		if (name == nullptr) \
		{ \
			printf("Could not load global level Vulkan function named: %s", #name); \
			return false; \
		}

	// TODO : Numea Log System	

	#include "ListOfVulkanFunctions.inl"

	sGlobalLevelFunctionsLoaded = true;

	return true;
}

bool Loader::loadInstanceLevelFunctions(const Instance& instance)
{
	const VkInstance& instanceHandle = instance.getHandle();

	// TODO : Use Numea System Log
	#define NU_INSTANCE_LEVEL_VULKAN_FUNCTION(name) \
		name = (PFN_##name)vkGetInstanceProcAddr(instanceHandle, #name); \
		if (name == nullptr) \
		{ \
			printf("Could not load instance-level Vulkan function named: %s\n", #name); \
			return false; \
		}

	// TODO : Use Numea System Log
	#define NU_INSTANCE_LEVEL_VULKAN_FUNCTION_FROM_EXTENSION(name, extension) \
		for (auto& enabledExtension : instance.getExtensions()) \
		{ \
			if (std::string(enabledExtension) == std::string(extension)) \
			{ \
				name = (PFN_##name)vkGetInstanceProcAddr(instanceHandle, #name); \
				if (name == nullptr) \
				{ \
					printf("Could not load instance-level Vulkan function named: %s (Extension: %s)\n", #name, #extension); \
					return false; \
				} \
			} \
		}

	#include "ListOfVulkanFunctions.inl"

	sInstanceLevelFunctionsLoaded = true;

	return true;
}

bool Loader::loadDeviceLevelFunctions(const VkDevice& device, const std::vector<const char*>& enabledExtensions)
{
	// TODO : Use Numea System Log
	// Load core Vulkan API deivce-level functions
	#define NU_DEVICE_LEVEL_VULKAN_FUNCTION(name) \
		name = (PFN_##name)vkGetDeviceProcAddr(device, #name); \
		if (name == nullptr) \
		{ \
			printf("Could not load device-level Vulkan function named: %s\n", #name); \
			return false; \
		}

	// TODO : Use Numea System Log
	// Load device-level functions from enabled extensions
	#define NU_DEVICE_LEVEL_VULKAN_FUNCTION_FROM_EXTENSION(name, extension) \
		for (auto& enabledExtension : enabledExtensions) \
		{ \
			if (std::string(enabledExtension) == std::string(extension)) \
			{ \
				name = (PFN_##name)vkGetDeviceProcAddr(device, #name); \
				if (name == nullptr) \
				{ \
					printf("Could not load device-level Vulkan function named: %s (Extension: %s)\n", #name, #extension); \
					return false; \
				} \
			} \
		}

	#include "ListOfVulkanFunctions.inl"

	sDeviceLevelFunctionsLoaded = true;

	return true;
}

DynamicLibrary Loader::sDynamicLibrary;
bool Loader::sExportedFunctionLoaded = false;
bool Loader::sGlobalLevelFunctionsLoaded = false;
bool Loader::sInstanceLevelFunctionsLoaded = false;
bool Loader::sDeviceLevelFunctionsLoaded = false;

} // namespace Vulkan
} // namespace nu