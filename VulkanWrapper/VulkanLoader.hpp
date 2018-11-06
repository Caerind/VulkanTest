#ifndef NU_VULKAN_LOADER_HPP
#define NU_VULKAN_LOADER_HPP

#include <vector>

#include "../System/DynamicLibrary.hpp"

#include "VulkanFunctions.hpp"

namespace nu
{
namespace Vulkan
{

class Instance;
class Device;
class Loader
{
	public:
		Loader() = delete;
		~Loader() = delete;

		static bool ensureDynamicLibraryLoaded();
		static bool ensureExportedFunctionLoaded();
		static bool ensureGlobalLevelFunctionsLoaded();
		static bool ensureInstanceLevelFunctionsLoaded(const Instance& instance);
		static bool ensureDeviceLevelFunctionsLoaded(const VkDevice& device, const std::vector<const char*>& enabledExtensions); // TODO : enabledExtensions in Device class and pass a Device

		static bool usingDynamicLinking();

	private:
		static bool loadDynamicLibrary();
		static bool loadExportedFunction();
		static bool loadGlobalLevelFunctions();
		static bool loadInstanceLevelFunctions(const Instance& instance);
		static bool loadDeviceLevelFunctions(const VkDevice& device, const std::vector<const char*>& enabledExtensions); // TODO : enabledExtensions in Device class and pass a Device

	private:
		static DynamicLibrary sDynamicLibrary;
		static bool sExportedFunctionLoaded;
		static bool sGlobalLevelFunctionsLoaded;
		static bool sInstanceLevelFunctionsLoaded;
		static bool sDeviceLevelFunctionsLoaded;
};

} // namespace Vulkan
} // namespace nu

#endif // NU_VULKAN_LOADER_HPP