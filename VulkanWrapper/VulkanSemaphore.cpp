#include "VulkanSemaphore.hpp"

#include "VulkanDevice.hpp"

VULKAN_NAMESPACE_BEGIN

VulkanSemaphore::~VulkanSemaphore()
{
	release();

	VULKAN_OBJECTTRACKER_UNREGISTER();
}

const VkSemaphore& VulkanSemaphore::getHandle() const
{
	return mSemaphore;
}

VulkanSemaphorePtr VulkanSemaphore::createSemaphore()
{
	VulkanSemaphorePtr semaphore(new VulkanSemaphore());
	if (semaphore != nullptr)
	{
		if (!semaphore->init())
		{
			semaphore.reset();
		}
	}
	return semaphore;
}

VulkanSemaphore::VulkanSemaphore()
	: mSemaphore(VK_NULL_HANDLE)
{
	VULKAN_OBJECTTRACKER_REGISTER();
}

bool VulkanSemaphore::init()
{
	VkSemaphoreCreateInfo semaphoreCreateInfo = {
		VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO,    // VkStructureType            sType
		nullptr,                                    // const void               * pNext
		0                                           // VkSemaphoreCreateFlags     flags
	};

	VkResult result = vkCreateSemaphore(getDeviceHandle(), &semaphoreCreateInfo, nullptr, &mSemaphore);
	if (result != VK_SUCCESS || mSemaphore == VK_NULL_HANDLE)
	{
		// TODO : Use Numea System Log
		printf("Could not create a semaphore\n");
		return false;
	}

	return true;
}

void VulkanSemaphore::release()
{
	if (mSemaphore!= VK_NULL_HANDLE)
	{
		vkDestroySemaphore(getDeviceHandle(), mSemaphore, nullptr);
		mSemaphore = VK_NULL_HANDLE;
	}
}

VULKAN_NAMESPACE_END