#ifndef NU_VULKAN_PHYSICAL_DEVICE_HPP
#define NU_VULKAN_PHYSICAL_DEVICE_HPP

#include "VulkanFunctions.hpp"

#include <map>
#include <vector>

namespace nu
{
namespace Vulkan
{

class Surface;
class PhysicalDevice
{
	public:
		PhysicalDevice(const VkPhysicalDevice& physicalDevice);

		const std::vector<VkExtensionProperties>& getAvailableExtensions() const;
		const VkPhysicalDeviceFeatures& getSupportedFeatures() const;
		const VkPhysicalDeviceProperties& getProperties() const;
		const std::vector<VkQueueFamilyProperties>& getQueueFamiliesProperties() const;
		const VkPhysicalDeviceMemoryProperties& getMemoryProperties() const;
		const VkFormatProperties& getFormatProperties(VkFormat format) const;

		bool isExtensionSupported(const char* extensionName) const;

		uint32_t getQueueFamilyIndexWithDesiredCapabilities(VkQueueFlags desiredCapabilities) const;
		uint32_t getGraphicsQueueFamilyIndex() const;
		uint32_t getComputeQueueFamilyIndex() const;
		uint32_t getPresentQueueFamilyIndex(Surface* presentationSurface) const;

		const VkPhysicalDevice& getHandle() const;

		static const uint32_t InvalidQueueFamilyIndex = ~uint32_t();

	private:
		bool queryAvailableExtensions() const;
		bool queryQueueFamiliesProperties() const;

	private:
		VkPhysicalDevice mPhysicalDevice; 

		mutable std::vector<VkExtensionProperties> mAvailableExtensions;

		mutable bool mSupportedFeaturesQueried;
		mutable VkPhysicalDeviceFeatures mSupportedFeatures;

		mutable bool mPropertiesQueried;
		mutable VkPhysicalDeviceProperties mProperties;

		mutable std::vector<VkQueueFamilyProperties> mQueueFamiliesProperties;

		mutable bool mMemoryPropertiesQueried;
		mutable VkPhysicalDeviceMemoryProperties mMemoryProperties;

		mutable std::map<VkFormat, VkFormatProperties> mFormatProperties;
};

} // namespace Vulkan
} // namespace nu

#endif // NU_VULKAN_PHYSICAL_DEVICE_HPP