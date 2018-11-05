#include "VulkanSemaphore.hpp"

#include "VulkanDevice.hpp"

namespace nu
{
namespace Vulkan
{

Semaphore::Ptr Semaphore::createSemaphore(Device& device)
{
	Semaphore::Ptr semaphore(new Semaphore(device));
	if (semaphore != nullptr)
	{
		if (!semaphore->init())
		{
			semaphore.reset();
		}
	}
	return semaphore;
}

Semaphore::~Semaphore()
{
	ObjectTracker::unregisterObject(ObjectType_Semaphore);

	release();
}

bool Semaphore::isInitialized() const
{
	return mSemaphore != VK_NULL_HANDLE;
}

const VkSemaphore& Semaphore::getHandle() const
{
	return mSemaphore;
}

const Device& Semaphore::getDeviceHandle() const
{
	return mDevice;
}

Semaphore::Semaphore(Device& device)
	: mDevice(device)
	, mSemaphore(VK_NULL_HANDLE)
{
	ObjectTracker::registerObject(ObjectType_Semaphore);
}

bool Semaphore::init()
{
	VkSemaphoreCreateInfo semaphoreCreateInfo = {
		VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO,    // VkStructureType            sType
		nullptr,                                    // const void               * pNext
		0                                           // VkSemaphoreCreateFlags     flags
	};

	VkResult result = vkCreateSemaphore(mDevice.getHandle(), &semaphoreCreateInfo, nullptr, &mSemaphore);
	if (result != VK_SUCCESS || mSemaphore == VK_NULL_HANDLE)
	{
		// TODO : Use Numea System Log
		printf("Could not create a semaphore\n");
		return false;
	}

	return true;
}

bool Semaphore::release()
{
	if (mSemaphore!= VK_NULL_HANDLE)
	{
		vkDestroySemaphore(mDevice.getHandle(), mSemaphore, nullptr);
		mSemaphore = VK_NULL_HANDLE;
		return true;
	}
	return false;
}

} // namespace Vulkan
} // namespace nu