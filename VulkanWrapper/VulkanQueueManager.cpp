#include "VulkanQueueManager.hpp"

#include "VulkanDevice.hpp"
#include "VulkanPhysicalDevice.hpp"
#include "VulkanSurface.hpp"
#include "VulkanQueue.hpp"

#include <unordered_map>

VULKAN_NAMESPACE_BEGIN

VulkanQueueParameters::VulkanQueueParameters()
{
}

void VulkanQueueParameters::addQueueInfo(VulkanU32 count, VulkanU32 flags)
{
	if (count > 0)
	{
		mQueueInfos.emplace_back(getCurrentIndex(), count, flags);
	}
	else
	{
		VULKAN_LOG_WARNING("QueueInfo count is 0, so it has been skipped (flags: %d)", flags);
	}
}

VulkanQueueParameters::QueueUserInfo::QueueUserInfo(VulkanU32 pIndex, VulkanU32 pCount, VulkanU32 pFlags)
	: index(pIndex)
	, count(pCount)
	, flags(pFlags)
{
}

VkQueueFlags VulkanQueueParameters::QueueUserInfo::convertFlagsToVkQueueFlags(bool* present)
{
	if (present != nullptr)
	{
		*present = (flags & VulkanQueueParameters::Present) != 0;
	}

	if ((flags & VulkanQueueParameters::Present) != 0)
	{
		return (VkQueueFlags)(flags - VulkanQueueParameters::Present);
	}
	else
	{
		return (VkQueueFlags)flags;
	}
}

VulkanU32 VulkanQueueParameters::getCurrentIndex() const
{
	VulkanU32 index = 0;
	for (const QueueUserInfo& queueInfo : mQueueInfos)
	{
		index += queueInfo.count;
	}
	return index;

}

VulkanQueue* VulkanQueueManager::getQueue(VulkanU32 index)
{
	VULKAN_ASSERT(index < getQueueCount());
	return mQueues[index].get();
}

const VulkanQueue* VulkanQueueManager::getQueue(VulkanU32 index) const
{
	VULKAN_ASSERT(index < getQueueCount());
	return mQueues[index].get();
}

VulkanU32 VulkanQueueManager::getQueueCount() const
{
	return (VulkanU32)mQueues.size();
}

VulkanQueueManager::VulkanQueueManager(VulkanDevice& device)
	: mDevice(device)
{
}

VulkanQueueManager::~VulkanQueueManager()
{
	release();
}

bool VulkanQueueManager::initialize(const VulkanQueueParameters& queueParameters, VulkanSurface* presentationSurface, std::vector<VkDeviceQueueCreateInfo>& queueCreateInfos)
{
	VulkanQueueParameters parameters = queueParameters;

	// Queues with higher flags are much rare (maybe wrong, but I think this will be true in most cases)
	sort(parameters.mQueueInfos.begin(), parameters.mQueueInfos.end(), 
		[](const VulkanQueueParameters::QueueUserInfo& a, const VulkanQueueParameters::QueueUserInfo& b)
		{
			return a.flags > b.flags;
		});

	// Get informations about queueFamilies
	const std::vector<VkQueueFamilyProperties>& queueFamiliesProperties = mDevice.getPhysicalDevice().getQueueFamiliesProperties();
	std::vector<VulkanU32> familySizes;
	familySizes.resize(queueFamiliesProperties.size(), 0);
	VulkanU32 totalQueues = 0;

	// For each queueInfo specified, find in which queueFamily it will be
	std::vector<VulkanU32> queueFamilyIndexes;
	queueFamilyIndexes.reserve(parameters.mQueueInfos.size());
	for (VulkanQueueParameters::QueueUserInfo& queueInfo : parameters.mQueueInfos)
	{
		bool presentToSurface = false;
		VkQueueFlags desiredCapabilities = queueInfo.convertFlagsToVkQueueFlags(&presentToSurface);

		VulkanU32 queueFamilyIndex = VulkanInvalidU32;
		for (VulkanU32 i = 0; i < queueFamiliesProperties.size(); i++)
		{
			if (desiredCapabilities > 0)
			{
				if (queueFamiliesProperties[i].queueCount >= familySizes[i] + queueInfo.count && (queueFamiliesProperties[i].queueFlags & desiredCapabilities) == desiredCapabilities)
				{
					queueFamilyIndex = i;
				}
				else
				{
					queueFamilyIndex = VulkanInvalidU32;
					continue;
				}
			}

			if (presentToSurface)
			{
				VkBool32 presentationSupported = VK_FALSE;
				VkResult result = vkGetPhysicalDeviceSurfaceSupportKHR(mDevice.getPhysicalDeviceHandle(), i, presentationSurface->getHandle(), &presentationSupported);
				if (result == VK_SUCCESS && presentationSupported == VK_TRUE && queueFamiliesProperties[i].queueCount >= familySizes[i] + queueInfo.count)
				{
					queueFamilyIndex = i;
				}
				else
				{
					queueFamilyIndex = VulkanInvalidU32;
					continue;
				}
			}

			if (queueFamilyIndex != VulkanInvalidU32)
			{
				familySizes[i] += queueInfo.count;
				totalQueues += queueInfo.count;
				break;
			}
		}

		if (queueFamilyIndex == VulkanInvalidU32)
		{
			VULKAN_LOG_ERROR("Could not find a valid queue family for every queues specified");
			return false;
		}
		else
		{
			queueFamilyIndexes.push_back(queueFamilyIndex);
		}
	}

	// Sort by index to be in the initial order
	sort(parameters.mQueueInfos.begin(), parameters.mQueueInfos.end(),
		[](const VulkanQueueParameters::QueueUserInfo& a, const VulkanQueueParameters::QueueUserInfo& b)
	{
		return a.index < b.index;
	});

	// Now we need to create QueueInternalInfos and Queues
	mQueues.reserve(totalQueues);
	for (VulkanU32 i = 0; i < parameters.mQueueInfos.size(); i++)
	{
		VULKAN_ASSERT(mQueues.size() == parameters.mQueueInfos[i].index);

		VulkanU32 family = queueFamilyIndexes[i];
		VulkanU32 count = parameters.mQueueInfos[i].count;
		VulkanU32 familyCurrentIndex = 0;

		// QueueInternalInfo
		bool found = false;
		for (QueueInternalInfo& internalInfo : mQueueInternalInfos)
		{
			if (internalInfo.family == family)
			{
				found = true;
				familyCurrentIndex = internalInfo.count;
				internalInfo.count += count;
				break;
			}
		}
		if (!found)
		{
			mQueueInternalInfos.emplace_back(family, count);
		}

		// Queues
		for (VulkanU32 i = 0; i < count; i++)
		{
			mQueues.emplace_back(new VulkanQueue(mDevice, family, familyCurrentIndex + i));
		}
	}

	// Now we need to create the queueCreateInfos
	queueCreateInfos.clear();
	queueCreateInfos.reserve(mQueueInternalInfos.size());
	for (QueueInternalInfo& queueInfo : mQueueInternalInfos)
	{
		queueInfo.priorities.resize(queueInfo.count, 1.0f);

		queueCreateInfos.push_back({
			VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO,       // VkStructureType                  sType
			nullptr,                                          // const void                     * pNext
			0,                                                // VkDeviceQueueCreateFlags         flags
			queueInfo.family,                                 // uint32_t                         queueFamilyIndex
			queueInfo.count,                                  // uint32_t                         queueCount
			queueInfo.priorities.data()                       // const float                    * pQueuePriorities
		});
	}

	return true;
}

bool VulkanQueueManager::initializeQueues()
{
	mQueueInternalInfos.clear(); // Not needed anymore

	for (VulkanQueuePtr& queue : mQueues)
	{
		if (!queue->initialize())
		{
			return false;
		}
	}

	return true;
}

bool VulkanQueueManager::release()
{
	mQueues.clear();
	mQueueInternalInfos.clear();
	return true;
}

VulkanQueueManager::QueueInternalInfo::QueueInternalInfo(VulkanU32 pFamily, VulkanU32 pCount)
	: family(pFamily)
	, count(pCount)
{
}

VULKAN_NAMESPACE_END
