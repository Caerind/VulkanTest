#include "VulkanFence.hpp"

#include "VulkanDevice.hpp"

namespace nu
{
namespace Vulkan
{

Fence::~Fence()
{
	release();

	ObjectTracker::unregisterObject(ObjectType_Fence);
}

bool Fence::wait(uint64_t timeout)
{
	VkResult result = vkWaitForFences(mDevice.getHandle(), 1, &mFence, VK_FALSE, timeout);
	if (result != VK_SUCCESS)
	{
		// TODO : Use Numea System Log
		printf("Waiting on fence failed\n");
		return false;
	}

	mSignaled = true;

	return true;
}

bool Fence::reset()
{
	VkResult result = vkResetFences(mDevice.getHandle(), 1, &mFence);
	if (result != VK_SUCCESS)
	{
		// TODO : Use Numea System Log
		printf("Error occurred when tried to reset fence\n");
		return false;
	}

	mSignaled = false;

	return true;
}

bool Fence::isSignaled() const
{
	return mSignaled;
}

Device& Fence::getDevice()
{
	return mDevice;
}

const Device& Fence::getDevice() const
{
	return mDevice;
}

const VkFence& Fence::getHandle() const
{
	return mFence;
}

const VkDevice& Fence::getDeviceHandle() const
{
	return mDevice.getHandle();
}

Fence::Ptr Fence::createFence(Device& device, bool signaled)
{
	Fence::Ptr fence(new Fence(device, signaled));
	if (fence != nullptr)
	{
		if (!fence->init())
		{
			fence.reset();
		}
	}
	return fence;
}

Fence::Fence(Device& device, bool signaled)
	: mDevice(device)
	, mFence(VK_NULL_HANDLE)
	, mSignaled(signaled)
{
	ObjectTracker::registerObject(ObjectType_Fence);
}

bool Fence::init()
{
	VkFenceCreateInfo fenceCreateInfo = {
		VK_STRUCTURE_TYPE_FENCE_CREATE_INFO,          // VkStructureType        sType
		nullptr,                                      // const void           * pNext
		mSignaled ? VK_FENCE_CREATE_SIGNALED_BIT : 0u,// VkFenceCreateFlags     flags
	};

	VkResult result = vkCreateFence(mDevice.getHandle(), &fenceCreateInfo, nullptr, &mFence);
	if (result != VK_SUCCESS || mFence == VK_NULL_HANDLE)
	{
		// TODO : Use Numea System Log
		printf("Could not create a fence\n");
		return false;
	}

	return true;
}

void Fence::release()
{
	if (mFence != VK_NULL_HANDLE)
	{
		vkDestroyFence(mDevice.getHandle(), mFence, nullptr);
		mFence = VK_NULL_HANDLE;
	}
}

} // namespace Vulkan
} // namespace nu