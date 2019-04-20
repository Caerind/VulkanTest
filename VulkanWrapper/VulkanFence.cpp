#include "VulkanFence.hpp"

#include "VulkanDevice.hpp"

VULKAN_NAMESPACE_BEGIN

VulkanFence::~VulkanFence()
{
	release();

	VULKAN_OBJECTTRACKER_UNREGISTER();
}

bool VulkanFence::wait(VulkanU64 timeout)
{
	VkResult result = vkWaitForFences(getDeviceHandle(), 1, &mFence, VK_FALSE, timeout);
	if (result != VK_SUCCESS)
	{
		// TODO : Log more
		VULKAN_LOG_ERROR("Waiting on fence failed");
		return false;
	}

	mSignaled = true;

	return true;
}

bool VulkanFence::reset()
{
	VkResult result = vkResetFences(getDeviceHandle(), 1, &mFence);
	if (result != VK_SUCCESS)
	{
		// TODO : Log more
		VULKAN_LOG_ERROR("Error occurred when tried to reset fence");
		return false;
	}

	mSignaled = false;

	return true;
}

const VkFence& VulkanFence::getHandle() const
{
	return mFence;
}

VulkanFencePtr VulkanFence::createFence(bool signaled)
{
	VulkanFencePtr fence(new VulkanFence(signaled));
	if (fence != nullptr)
	{
		if (!fence->init())
		{
			fence.reset();
		}
	}
	return fence;
}

VulkanFence::VulkanFence(bool signaled)
	: mFence(VK_NULL_HANDLE)
	, mSignaled(signaled)
{
	VULKAN_OBJECTTRACKER_REGISTER();
}

bool VulkanFence::init()
{
	VkFenceCreateInfo fenceCreateInfo = {
		VK_STRUCTURE_TYPE_FENCE_CREATE_INFO,          // VkStructureType        sType
		nullptr,                                      // const void           * pNext
		mSignaled ? VK_FENCE_CREATE_SIGNALED_BIT : 0u,// VkFenceCreateFlags     flags
	};

	VkResult result = vkCreateFence(getDeviceHandle(), &fenceCreateInfo, nullptr, &mFence);
	if (result != VK_SUCCESS || mFence == VK_NULL_HANDLE)
	{
		VULKAN_LOG_ERROR("Could not create a fence\n");
		return false;
	}

	return true;
}

void VulkanFence::release()
{
	if (mFence != VK_NULL_HANDLE)
	{
		vkDestroyFence(getDeviceHandle(), mFence, nullptr);
		mFence = VK_NULL_HANDLE;
	}
}