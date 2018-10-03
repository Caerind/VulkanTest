#include "VulkanLoader.hpp"

namespace nu
{
namespace Vulkan
{

bool Loader::loadDynamicExportedGlobal()
{
	if (!isDynamicLibraryLoaded())
	{
		if (!loadDynamicLibrary())
		{
			return false;
		}
	}
	if (!isExportedFunctionLoaded())
	{
		if (!loadExportedFunction())
		{
			return false;
		}
	}
	if (!areGlobalLevelFunctionsLoaded())
	{
		if (!loadGlobalLevelFunctions())
		{
			return false;
		}
	}

	return true;
}

bool Loader::areDynamicExportedGlobalLoaded()
{
	return isDynamicLibraryLoaded() && isExportedFunctionLoaded() && areGlobalLevelFunctionsLoaded();
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

bool Loader::isDynamicLibraryLoaded()
{
	return sDynamicLibrary.isLibraryLoaded();
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

bool Loader::isExportedFunctionLoaded()
{
	return sExportedFunctionLoaded;
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

bool Loader::areGlobalLevelFunctionsLoaded()
{
	return sGlobalLevelFunctionsLoaded;
}

bool Loader::loadInstanceLevelFunctions(const VkInstance& instance, const std::vector<const char*>& enabledExtensions)
{
	// TODO : Use Numea System Log
	#define NU_INSTANCE_LEVEL_VULKAN_FUNCTION(name) \
		name = (PFN_##name)vkGetInstanceProcAddr(instance, #name); \
		if (name == nullptr) \
		{ \
			printf("Could not load instance-level Vulkan function named: %s\n", #name); \
			return false; \
		}

	// TODO : Use Numea System Log
	#define NU_INSTANCE_LEVEL_VULKAN_FUNCTION_FROM_EXTENSION(name, extension) \
		for (auto& enabledExtension : enabledExtensions) \
		{ \
			if (std::string(enabledExtension) == std::string(extension)) \
			{ \
				name = (PFN_##name)vkGetInstanceProcAddr(instance, #name); \
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

bool Loader::areInstanceLevelFunctionsLoaded()
{
	return sInstanceLevelFunctionsLoaded;
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

bool Loader::areDeviceLevelFunctionsLoaded()
{
	return sDeviceLevelFunctionsLoaded;
}

const std::vector<VkExtensionProperties>& Loader::getAvailableInstanceExtensions()
{
	if (sAvailableInstanceExtensions.size() == 0)
	{
		queryAvailableInstanceExtensions();
	}

	return sAvailableInstanceExtensions;
}

const std::vector<VkLayerProperties>& Loader::getAvailableInstanceLayers()
{
	if (sAvailableInstanceLayers.size() == 0)
	{
		queryAvailableInstanceLayers();
	}

	return sAvailableInstanceLayers;
}

bool Loader::isInstanceExtensionSupported(const char* extensionName)
{
	if (sAvailableInstanceExtensions.size() == 0)
	{
		queryAvailableInstanceExtensions();
	}

	for (const VkExtensionProperties& extension : sAvailableInstanceExtensions)
	{
		if (strcmp(extension.extensionName, extensionName) == 0)
		{
			return true;
		}
	}
	return false;
}

bool Loader::isInstanceLayerSupported(const char* layerName)
{
	if (sAvailableInstanceLayers.size() == 0)
	{
		queryAvailableInstanceLayers();
	}

	for (const VkLayerProperties& layer : sAvailableInstanceLayers)
	{
		if (strcmp(layer.layerName, layerName) == 0)
		{
			return true;
		}
	}
	return false;
}

bool Loader::queryAvailableInstanceExtensions()
{
	sAvailableInstanceExtensions.clear();

	uint32_t extensionCount = 0;
	VkResult result = VK_SUCCESS;

	result = vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, nullptr);
	if (result != VK_SUCCESS || extensionCount == 0)
	{
		// TODO : Use Numea System Log
		printf("Could not get the number of instance extensions\n");
		return false;
	}

	sAvailableInstanceExtensions.resize(extensionCount);
	result = vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, sAvailableInstanceExtensions.data());
	if (result != VK_SUCCESS || extensionCount == 0)
	{
		// TODO : Use Numea System Log
		printf("Could not enumerate instance extensions\n");
		return false;
	}

	return true;
}

bool Loader::queryAvailableInstanceLayers()
{
	sAvailableInstanceLayers.clear();

	uint32_t layerCount = 0;
	VkResult result = VK_SUCCESS;

	result = vkEnumerateInstanceLayerProperties(&layerCount, nullptr);
	if (result != VK_SUCCESS || layerCount == 0)
	{
		// TODO : Use Numea System Log
		printf("Could not get the number of instance layers\n");
		return false;
	}

	sAvailableInstanceLayers.resize(layerCount);
	result = vkEnumerateInstanceLayerProperties(&layerCount, sAvailableInstanceLayers.data());
	if (result != VK_SUCCESS || layerCount == 0)
	{
		// TODO : Use Numea System Log
		printf("Could not enumerate instance layers\n");
		return false;
	}

	return true;
}

DynamicLibrary Loader::sDynamicLibrary;
bool Loader::sExportedFunctionLoaded = false;
bool Loader::sGlobalLevelFunctionsLoaded = false;
bool Loader::sInstanceLevelFunctionsLoaded = false;
bool Loader::sDeviceLevelFunctionsLoaded = false;
std::vector<VkExtensionProperties> Loader::sAvailableInstanceExtensions;
std::vector<VkLayerProperties> Loader::sAvailableInstanceLayers;

} // namespace Vulkan
} // namespace nu