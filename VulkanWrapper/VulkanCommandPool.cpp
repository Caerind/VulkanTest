#include "VulkanCommandPool.hpp"

#include "VulkanCommandBuffer.hpp"
#include "VulkanDevice.hpp"

#include <algorithm>

VULKAN_NAMESPACE_BEGIN

VulkanCommandPoolPtr VulkanCommandPool::createCommandPool(VkCommandPoolCreateFlags parameters,VulkanU32 queueFamily)
{
	VulkanCommandPoolPtr commandPool(new VulkanCommandPool());
	if (commandPool != nullptr)
	{
		if (!commandPool->init(parameters, queueFamily))
		{
			commandPool.reset();
		}
	}
	return commandPool;
}

VulkanCommandPool::~VulkanCommandPool()
{
	release();
	
	VULKAN_OBJECTTRACKER_UNREGISTER();
}

VulkanCommandBufferPtr VulkanCommandPool::allocatePrimaryCommandBuffer()
{
	VulkanCommandBufferPtr commandBuffer(new VulkanCommandBuffer(*this, VulkanCommandBufferType::Primary));
	if (commandBuffer != nullptr)
	{
		if (!commandBuffer->init())
		{
			commandBuffer.reset();
		}
	}
	return commandBuffer;
}

VulkanCommandBufferPtr VulkanCommandPool::allocateSecondaryCommandBuffer()
{
	VulkanCommandBufferPtr commandBuffer(new VulkanCommandBuffer(*this, VulkanCommandBufferType::Secondary));
	if (commandBuffer != nullptr)
	{
		if (!commandBuffer->init())
		{
			commandBuffer.reset();
		}
	}
	return commandBuffer;
}

bool VulkanCommandPool::reset()
{
	// TODO : What is the 0 parameter ?
	VkResult result = vkResetCommandPool(getDeviceHandle(), mCommandPool, 0);
	if (result != VK_SUCCESS)
	{
		// TODO : Log more
		VULKAN_LOG_ERROR("Error occurred during command pool reset");
		return false;
	}

	return true;
}

bool VulkanCommandPool::isInitialized() const
{
	return mCommandPool != VK_NULL_HANDLE;
}

const VkCommandPool& VulkanCommandPool::getHandle() const
{
	return mCommandPool;
}

VulkanCommandPool::VulkanCommandPool()
	: mCommandPool(VK_NULL_HANDLE)
{
	VULKAN_OBJECTTRACKER_REGISTER();
}

bool VulkanCommandPool::init(VkCommandPoolCreateFlags parameters, VulkanU32 queueFamily)
{
	VkCommandPoolCreateInfo commandPoolCreateInfo = {
		VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO,   // VkStructureType              sType
		nullptr,                                      // const void                 * pNext
		parameters,                                   // VkCommandPoolCreateFlags     flags
		queueFamily                                   // uint32_t                     queueFamilyIndex
	};

	VkResult result = vkCreateCommandPool(getDeviceHandle(), &commandPoolCreateInfo, nullptr, &mCommandPool);
	if (VK_SUCCESS != result || mCommandPool == VK_NULL_HANDLE)
	{
		VULKAN_LOG_ERROR("Could not create command pool");
		return false;
	}
	return true;
}

bool VulkanCommandPool::release()
{
	if (mCommandPool != VK_NULL_HANDLE)
	{
		vkDestroyCommandPool(getDeviceHandle(), mCommandPool, nullptr);
		mCommandPool = VK_NULL_HANDLE;
		return true;
	}
	return false;
}

VULKAN_NAMESPACE_END