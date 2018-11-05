#include "VulkanCommandPool.hpp"

#include "VulkanDevice.hpp"

#include <algorithm>

namespace nu
{
namespace Vulkan
{

CommandPool::Ptr CommandPool::createCommandPool(Device& device, VkCommandPoolCreateFlags parameters, uint32_t queueFamily)
{
	CommandPool::Ptr commandPool(new CommandPool(device));
	if (commandPool != nullptr)
	{
		if (!commandPool->init(parameters, queueFamily))
		{
			commandPool.reset();
		}
	}
	return commandPool;
}

CommandPool::~CommandPool()
{
	ObjectTracker::unregisterObject(ObjectType_CommandPool);

	release();
}

CommandBuffer::Ptr CommandPool::allocatePrimaryCommandBuffer()
{
	CommandBuffer::Ptr commandBuffer(new CommandBuffer(*this, CommandBufferType::Primary));
	if (commandBuffer != nullptr)
	{
		if (!commandBuffer->init())
		{
			commandBuffer.reset();
		}
	}
	return commandBuffer;
}

CommandBuffer::Ptr CommandPool::allocateSecondaryCommandBuffer()
{
	CommandBuffer::Ptr commandBuffer(new CommandBuffer(*this, CommandBufferType::Secondary));
	if (commandBuffer != nullptr)
	{
		if (!commandBuffer->init())
		{
			commandBuffer.reset();
		}
	}
	return commandBuffer;
}

bool CommandPool::reset()
{
	VkResult result = vkResetCommandPool(mDevice.getHandle(), mCommandPool, 0);
	if (result != VK_SUCCESS)
	{
		// TODO : Use Numea System Log
		printf("Error occurred during command pool reset\n");
		return false;
	}

	return true;
}

bool CommandPool::isInitialized() const
{
	return mCommandPool != VK_NULL_HANDLE;
}

const VkCommandPool& CommandPool::getHandle() const
{
	return mCommandPool;
}

const VkDevice& CommandPool::getDeviceHandle() const
{
	return mDevice.getHandle();
}

CommandPool::CommandPool(Device& device)
	: mCommandPool(VK_NULL_HANDLE)
	, mDevice(device)
{
	ObjectTracker::registerObject(ObjectType_CommandPool);
}

bool CommandPool::init(VkCommandPoolCreateFlags parameters, uint32_t queueFamily)
{
	VkCommandPoolCreateInfo commandPoolCreateInfo = {
		VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO,   // VkStructureType              sType
		nullptr,                                      // const void                 * pNext
		parameters,                                   // VkCommandPoolCreateFlags     flags
		queueFamily                                   // uint32_t                     queueFamilyIndex
	};

	VkResult result = vkCreateCommandPool(mDevice.getHandle(), &commandPoolCreateInfo, nullptr, &mCommandPool);
	if (VK_SUCCESS != result || mCommandPool == VK_NULL_HANDLE)
	{
		// TODO : Use Numea System Log
		printf("Could not create command pool\n");
		return false;
	}
	return true;
}

bool CommandPool::release()
{
	if (mCommandPool != VK_NULL_HANDLE)
	{
		vkDestroyCommandPool(mDevice.getHandle(), mCommandPool, nullptr);
		mCommandPool = VK_NULL_HANDLE;
		return true;
	}
	return false;
}

} // namespace Vulkan
} // namespace nu