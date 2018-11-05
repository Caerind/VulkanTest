#include "VulkanPhysicalDevice.hpp"

#include "VulkanSurface.hpp"

namespace nu
{
namespace Vulkan
{

PhysicalDevice::PhysicalDevice(const VkPhysicalDevice& physicalDevice)
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

const std::vector<VkExtensionProperties>& PhysicalDevice::getAvailableExtensions() const
{
	if (mAvailableExtensions.size() == 0)
	{
		queryAvailableExtensions();
	}

	return mAvailableExtensions;
}

const VkPhysicalDeviceFeatures& PhysicalDevice::getSupportedFeatures() const
{
	if (!mSupportedFeaturesQueried)
	{
		mSupportedFeaturesQueried = true;
		vkGetPhysicalDeviceFeatures(mPhysicalDevice, &mSupportedFeatures);
	}

	return mSupportedFeatures;
}

const VkPhysicalDeviceProperties& PhysicalDevice::getProperties() const
{
	if (!mPropertiesQueried)
	{
		mPropertiesQueried = true;
		vkGetPhysicalDeviceProperties(mPhysicalDevice, &mProperties);
	}

	return mProperties;
}

const std::vector<VkQueueFamilyProperties>& PhysicalDevice::getQueueFamiliesProperties() const
{
	if (mQueueFamiliesProperties.size() == 0)
	{
		queryQueueFamiliesProperties();
	}

	return mQueueFamiliesProperties;
}

const VkPhysicalDeviceMemoryProperties& PhysicalDevice::getMemoryProperties() const
{
	if (!mMemoryPropertiesQueried)
	{
		mMemoryPropertiesQueried = true;
		vkGetPhysicalDeviceMemoryProperties(mPhysicalDevice, &mMemoryProperties);
	}

	return mMemoryProperties;
}

const VkFormatProperties& PhysicalDevice::getFormatProperties(VkFormat format) const
{
	auto itr = mFormatProperties.find(format);
	if (itr == mFormatProperties.end())
	{
		vkGetPhysicalDeviceFormatProperties(mPhysicalDevice, format, &mFormatProperties[format]);
		return mFormatProperties[format];
	}
	else
	{
		return itr->second;
	}
}

bool PhysicalDevice::isExtensionSupported(const char* extensionName) const
{
	if (mAvailableExtensions.size() == 0)
	{
		queryAvailableExtensions();
	}

	for (const VkExtensionProperties& extension : mAvailableExtensions)
	{
		if (strcmp(extension.extensionName, extensionName) == 0)
		{
			return true;
		}
	}
	return false;
}

uint32_t PhysicalDevice::getQueueFamilyIndexWithDesiredCapabilities(VkQueueFlags desiredCapabilities) const
{
	if (mQueueFamiliesProperties.size() == 0)
	{
		queryQueueFamiliesProperties();
	}

	uint32_t size = static_cast<uint32_t>(mQueueFamiliesProperties.size());
	for (uint32_t index = 0; index < size; index++)
	{
		if (mQueueFamiliesProperties[index].queueCount > 0 && (mQueueFamiliesProperties[index].queueFlags & desiredCapabilities) == desiredCapabilities)
		{
			return index;
		}
	}
	return InvalidQueueFamilyIndex;
}

uint32_t PhysicalDevice::getGraphicsQueueFamilyIndex() const
{
	return getQueueFamilyIndexWithDesiredCapabilities(VK_QUEUE_GRAPHICS_BIT);
}

uint32_t PhysicalDevice::getComputeQueueFamilyIndex() const
{
	return getQueueFamilyIndexWithDesiredCapabilities(VK_QUEUE_COMPUTE_BIT);
}

uint32_t PhysicalDevice::getPresentQueueFamilyIndex(Surface* presentationSurface) const
{
	if (mQueueFamiliesProperties.size() == 0)
	{
		queryQueueFamiliesProperties();
	}

	for (uint32_t index = 0; index < static_cast<uint32_t>(mQueueFamiliesProperties.size()); index++)
	{
		VkBool32 presentationSupported = VK_FALSE;
		VkResult result = vkGetPhysicalDeviceSurfaceSupportKHR(mPhysicalDevice, index, presentationSurface->getHandle(), &presentationSupported);
		if ((result == VK_SUCCESS) && (presentationSupported == VK_TRUE))
		{
			return index;
		}
	}

	return InvalidQueueFamilyIndex;
}

const VkPhysicalDevice& PhysicalDevice::getHandle() const
{
	return mPhysicalDevice;
}

bool PhysicalDevice::queryAvailableExtensions() const
{
	mAvailableExtensions.clear();

	uint32_t extensionsCount = 0;
	VkResult result = VK_SUCCESS;

	result = vkEnumerateDeviceExtensionProperties(mPhysicalDevice, nullptr, &extensionsCount, nullptr);
	if (result != VK_SUCCESS || extensionsCount == 0)
	{
		// TODO : Use Numea System Log
		printf("Could not get the number of device extensions\n");
		return false;
	}

	mAvailableExtensions.resize(extensionsCount);
	result = vkEnumerateDeviceExtensionProperties(mPhysicalDevice, nullptr, &extensionsCount, mAvailableExtensions.data());
	if ((result != VK_SUCCESS) || (extensionsCount == 0))
	{
		// TODO : Use Numea System Log
		printf("Could not enumerate device extensions\n");
		return false;
	}

	return true;
}

bool PhysicalDevice::queryQueueFamiliesProperties() const
{
	mQueueFamiliesProperties.clear();

	uint32_t queueFamiliesCount = 0;

	vkGetPhysicalDeviceQueueFamilyProperties(mPhysicalDevice, &queueFamiliesCount, nullptr);
	if (queueFamiliesCount == 0)
	{
		// TODO : Use Numea System Log
		printf("Could not get the number of queue families\n");
		return false;
	}

	mQueueFamiliesProperties.resize(queueFamiliesCount);
	vkGetPhysicalDeviceQueueFamilyProperties(mPhysicalDevice, &queueFamiliesCount, mQueueFamiliesProperties.data());
	if (queueFamiliesCount == 0)
	{
		// TODO : Use Numea System Log
		printf("Could not acquire properties of queue families\n");
		return false;
	}

	return true;
}

} // namespace Vulkan
} // namespace nu