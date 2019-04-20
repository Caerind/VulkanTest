#pragma once

#include "VulkanFunctions.hpp"
#include <unordered_map>

VULKAN_NAMESPACE_BEGIN

class VulkanPhysicalDevice
{
	public:
		VulkanPhysicalDevice(const VkPhysicalDevice& physicalDevice);

		const std::vector<VkExtensionProperties>& getAvailableExtensions() const;
		const VkPhysicalDeviceFeatures& getSupportedFeatures() const;
		const VkPhysicalDeviceProperties& getProperties() const;
		const std::vector<VkQueueFamilyProperties>& getQueueFamiliesProperties() const;
		const VkPhysicalDeviceMemoryProperties& getMemoryProperties() const;
		const VkFormatProperties& getFormatProperties(VkFormat format) const;
		
		bool queryAvailableExtensions();
		bool querySupportedFeatures();
		bool queryProperties();
		bool queryQueueFamiliesProperties();
		bool queryMemoryProperties();
		bool queryFormatProperties(VkFormat format);
		bool queryAllProperties();
		
		bool areAvailableExtensionsQueried() const;
		bool areSupportedFeaturesQueried() const;
		bool arePropertiesQueried() const;
		bool areQueueFamiliesPropertiesQueried() const;
		bool areMemoryPropertiesQueried() const;
		bool areFormatPropertiesQueried(VkFormat format) const;
		bool areAllPropertiesQueried() const;

		bool isExtensionSupported(const char* extensionName) const;

		const VkPhysicalDevice& getHandle() const;
		
	private:
		// NonCopyable & NonMovable
		VulkanPhysicalDevice(const VulkanPhysicalDevice& other) = delete;
		VulkanPhysicalDevice& operator=(const VulkanPhysicalDevice& other) = delete;
		VulkanPhysicalDevice(VulkanPhysicalDevice&& other) = delete;
		VulkanPhysicalDevice& operator=(VulkanPhysicalDevice&& other) = delete;

	private:
		VkPhysicalDevice mPhysicalDevice; 

		std::vector<VkExtensionProperties> mAvailableExtensions;
		bool mSupportedFeaturesQueried;
		VkPhysicalDeviceFeatures mSupportedFeatures;
		bool mPropertiesQueried;
		VkPhysicalDeviceProperties mProperties;
		std::vector<VkQueueFamilyProperties> mQueueFamiliesProperties;
		bool mMemoryPropertiesQueried;
		VkPhysicalDeviceMemoryProperties mMemoryProperties;
		mutable std::unordered_map<VkFormat, VkFormatProperties> mFormatProperties;
};

VULKAN_NAMESPACE_END