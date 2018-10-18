#ifndef NU_VULKAN_INSTANCE_HPP
#define NU_VULKAN_INSTANCE_HPP

#include "VulkanLoader.hpp"
#include "VulkanPhysicalDevice.hpp"

#include <functional>
#include <memory>
#include <vector>

#include "VulkanSurface.hpp"

namespace nu
{
namespace Vulkan
{

	class Instance
	{
	public:
		typedef std::unique_ptr<Instance> Ptr;

		static Instance::Ptr createInstance(const std::vector<const char*>& desiredExtensions = {}, const std::vector<const char*> desiredLayers = {});

		~Instance();

		PhysicalDevice& getPhysicalDevice(uint32_t index);
		const PhysicalDevice& getPhysicalDevice(uint32_t index) const;
		const std::vector<PhysicalDevice>& getPhysicalDevices() const;
		uint32_t getPhysicalDeviceCount() const;

		Surface::Ptr createSurface(const WindowParameters& windowParameters);

		bool isInitialized() const;
		const VkInstance& getHandle() const;

		const std::vector<const char*>& getExtensions() const;
		const std::vector<const char*>& getLayers() const;

		static bool isInstanceExtensionSupported(const char* extensionName);
		static bool isInstanceLayerSupported(const char* layerName);

	private:
		Instance(const std::vector<const char*>& desiredExtensions, const std::vector<const char*>& desiredLayers);

		bool queryAvailablePhysicalDevices() const;

		// TODO : Maybe register many callbacks with different outputs
		// With each callback may be enabled or not depending on users needs
		static VKAPI_ATTR VkBool32 VKAPI_CALL debugReportCallback(VkDebugReportFlagsEXT flags, VkDebugReportObjectTypeEXT objectType, uint64_t object, size_t location, int32_t messageCode, const char* pLayerPrefix, const char* pMessage, void* pUserData);

		static bool queryAvailableInstanceExtensions();
		static bool queryAvailableInstanceLayers();

	private:
		VkInstance mInstance;
		VkDebugReportCallbackEXT mDebugReportCallback;

		std::vector<const char*> mExtensions;
		std::vector<const char*> mLayers;

		mutable std::vector<PhysicalDevice> mPhysicalDevices;

		static std::vector<VkExtensionProperties> sAvailableInstanceExtensions;
		static std::vector<VkLayerProperties> sAvailableInstanceLayers;
};

} // namespace Vulkan
} // namespace nu

#endif // NU_VULKAN_INSTANCE_HPP