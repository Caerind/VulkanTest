#ifndef NU_VULKAN_LOADER_HPP
#define NU_VULKAN_LOADER_HPP

#include "VulkanFunctions.hpp"

#include "System/DynamicLibrary.hpp"

#include <vector>

namespace nu
{
namespace Vulkan
{

class Loader
{
	public:
		Loader() = delete;
		~Loader() = delete;

		static bool loadDynamicExportedGlobal();
		static bool areDynamicExportedGlobalLoaded();

		static bool loadDynamicLibrary();
		static bool isDynamicLibraryLoaded();

		static bool loadExportedFunction();
		static bool isExportedFunctionLoaded();

		static bool loadGlobalLevelFunctions();
		static bool areGlobalLevelFunctionsLoaded();

		static bool loadInstanceLevelFunctions(const VkInstance& instance, const std::vector<const char*>& enabledExtensions);
		static bool areInstanceLevelFunctionsLoaded();

		static bool loadDeviceLevelFunctions(const VkDevice& device, const std::vector<const char*>& enabledExtensions);
		static bool areDeviceLevelFunctionsLoaded();

		static const std::vector<VkExtensionProperties>& getAvailableInstanceExtensions();
		static const std::vector<VkLayerProperties>& getAvailableInstanceLayers();

		static bool isInstanceExtensionSupported(const char* extensionName);
		static bool isInstanceLayerSupported(const char* layerName);

	private:
		static bool queryAvailableInstanceExtensions();
		static bool queryAvailableInstanceLayers();

	private:
		static DynamicLibrary sDynamicLibrary;
		static bool sExportedFunctionLoaded;
		static bool sGlobalLevelFunctionsLoaded;
		static bool sInstanceLevelFunctionsLoaded;
		static bool sDeviceLevelFunctionsLoaded;
		static std::vector<VkExtensionProperties> sAvailableInstanceExtensions;
		static std::vector<VkLayerProperties> sAvailableInstanceLayers;
};

} // namespace Vulkan
} // namespace nu

#endif // NU_VULKAN_LOADER_HPP