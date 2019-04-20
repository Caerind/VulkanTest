#include "VulkanPhysicalDevice.hpp"

#include "VulkanSurface.hpp"

VULKAN_NAMESPACE_BEGIN

VulkanPhysicalDevice::VulkanPhysicalDevice(const VkPhysicalDevice& physicalDevice)
	: mPhysicalDevice(physicalDevice)
	, mAvailableExtensions()
	, mSupportedFeaturesQueried(false)
	, mSupportedFeatures()
	, mPropertiesQueried(false)
	, mProperties()
	, mQueueFamiliesProperties()
	, mMemoryPropertiesQueried(false)
	, mMemoryProperties()
	, mFormatProperties()
{
}

const std::vector<VkExtensionProperties>& VulkanPhysicalDevice::getAvailableExtensions() const
{
	VULKAN_ASSERT(areAvailableExtensionsQueried());
	return mAvailableExtensions;
}

const VkPhysicalDeviceFeatures& VulkanPhysicalDevice::getSupportedFeatures() const
{
	VULKAN_ASSERT(areSupportedFeaturesQueried());
	return mSupportedFeatures;
}

const VkPhysicalDeviceProperties& VulkanPhysicalDevice::getProperties() const
{
	VULKAN_ASSERT(arePropertiesQueried());
	return mProperties;
}

const std::vector<VkQueueFamilyProperties>& VulkanPhysicalDevice::getQueueFamiliesProperties() const
{
	VULKAN_ASSERT(areQueueFamiliesPropertiesQueried());
	return mQueueFamiliesProperties;
}

const VkPhysicalDeviceMemoryProperties& VulkanPhysicalDevice::getMemoryProperties() const
{
	VULKAN_ASSERT(areMemoryPropertiesQueried());
	return mMemoryProperties;
}

const VkFormatProperties& VulkanPhysicalDevice::getFormatProperties(VkFormat format) const
{
	if (!areFormatPropertiesQueried(format))
	{
		// TODO : Can this fail ?
		vkGetPhysicalDeviceFormatProperties(mPhysicalDevice, format, &mFormatProperties[format]);
	}
	auto itr = mFormatProperties.find(format);
	VULKAN_ASSERT(itr != mFormatProperties.end(), "Format has not been queried before : %d", format);
	return itr->second;
}

bool VulkanPhysicalDevice::queryAvailableExtensions()
{
	mAvailableExtensions.clear();
	
	VulkanU32 extensionsCount = 0;
	VkResult result = vkEnumerateDeviceExtensionProperties(mPhysicalDevice, nullptr, &extensionsCount, nullptr);
	if (result != VK_SUCCESS || extensionsCount == 0)
	{
		VULKAN_LOG_ERROR("Could not get the number of device extensions (PhysicalDevice: %d)", mPhysicalDevice);
		return false;
	}

	mAvailableExtensions.resize(extensionsCount);
	result = vkEnumerateDeviceExtensionProperties(mPhysicalDevice, nullptr, &extensionsCount, mAvailableExtensions.data());
	if (result != VK_SUCCESS || extensionsCount == 0)
	{
		VULKAN_LOG_ERROR("Could not enumerate device extensions (PhysicalDevice: %d)", mPhysicalDevice);
		return false;
	}

	return areAvailableExtensionsQueried();
}

bool VulkanPhysicalDevice::querySupportedFeatures()
{
	// TODO : Can this fail ?
	vkGetPhysicalDeviceFeatures(mPhysicalDevice, &mSupportedFeatures);
	mSupportedFeaturesQueried = true;
	return areSupportedFeaturesQueried();
}

bool VulkanPhysicalDevice::queryProperties()
{
	// TODO : Can this fail ?
	vkGetPhysicalDeviceProperties(mPhysicalDevice, &mProperties);
	mPropertiesQueried = true;	
	return arePropertiesQueried();
}

bool VulkanPhysicalDevice::queryQueueFamiliesProperties()
{
	mQueueFamiliesProperties.clear();

	VulkanU32 queueFamiliesCount = 0;
	vkGetPhysicalDeviceQueueFamilyProperties(mPhysicalDevice, &queueFamiliesCount, nullptr);
	if (queueFamiliesCount == 0)
	{
		VULKAN_LOG_ERROR("Could not get the number of queue families (PhysicalDevice: %d)", mPhysicalDevice);
		return false;
	}

	mQueueFamiliesProperties.resize(queueFamiliesCount);
	vkGetPhysicalDeviceQueueFamilyProperties(mPhysicalDevice, &queueFamiliesCount, mQueueFamiliesProperties.data());
	if (queueFamiliesCount == 0)
	{
		VULKAN_LOG_ERROR("Could not acquire properties of queue families (PhysicalDevice: %d)", mPhysicalDevice);
		return false;
	}

	return areQueueFamiliesPropertiesQueried();
}

bool VulkanPhysicalDevice::queryMemoryProperties()
{
	// TODO : Can this fail ?
	vkGetPhysicalDeviceMemoryProperties(mPhysicalDevice, &mMemoryProperties);
	mMemoryPropertiesQueried = true;
	return areMemoryPropertiesQueried();
}

bool VulkanPhysicalDevice::queryFormatProperties(VkFormat format)
{
	// TODO : Can this fail ?
	vkGetPhysicalDeviceFormatProperties(mPhysicalDevice, format, &mFormatProperties[format]);
	return areFormatPropertiesQueried(format);
}

bool VulkanPhysicalDevice::queryAllProperties()
{
	bool everythingWentWell = true;
	everythingWentWell &= queryAvailableExtensions();
	everythingWentWell &= querySupportedFeatures();
	everythingWentWell &= queryProperties();
	everythingWentWell &= queryQueueFamiliesProperties();
	everythingWentWell &= queryMemoryProperties();
	// TODO : Query some/all VkFormat ?
	return everythingWentWell;
}

bool VulkanPhysicalDevice::areAvailableExtensionsQueried() const
{
	return mAvailableExtensions.size() > 0;
}

bool VulkanPhysicalDevice::areSupportedFeaturesQueried() const
{
	return mSupportedFeaturesQueried;
}

bool VulkanPhysicalDevice::arePropertiesQueried() const
{
	return mPropertiesQueried;
}

bool VulkanPhysicalDevice::areQueueFamiliesPropertiesQueried() const
{
	return mQueueFamiliesProperties.size();
}

bool VulkanPhysicalDevice::areMemoryPropertiesQueried() const
{
	return mMemoryPropertiesQueried;
}

bool VulkanPhysicalDevice::areFormatPropertiesQueried(VkFormat format) const
{
	return mFormatProperties.find(format) != mFormatProperties.end();
}

bool VulkanPhysicalDevice::areAllPropertiesQueried() const
{
	return areAvailableExtensionsQueried()
		&& areSupportedFeaturesQueried()
		&& arePropertiesQueried()
		&& areQueueFamiliesPropertiesQueried()
		&& areMemoryPropertiesQueried();
	// TODO : Query some/all VkFormat ?
}

bool VulkanPhysicalDevice::isExtensionSupported(const char* extensionName) const
{
	for (const VkExtensionProperties& extension : mAvailableExtensions)
	{
		if (strcmp(extension.extensionName, extensionName) == 0)
		{
			return true;
		}
	}
	return false;
}

const VkPhysicalDevice& VulkanPhysicalDevice::getHandle() const
{
	return mPhysicalDevice;
}

VULKAN_NAMESPACE_END