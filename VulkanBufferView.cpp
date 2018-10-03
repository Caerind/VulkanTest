#include "VulkanBuffer.hpp"

#include "VulkanDevice.hpp"
#include "VulkanBufferView.hpp"

namespace nu
{
namespace Vulkan
{

BufferView::Ptr BufferView::createBufferView(Device& device, Buffer& buffer, VkFormat format, VkDeviceSize memoryOffset, VkDeviceSize memoryRange)
{
	BufferView::Ptr bufferView(new BufferView(device, buffer, format, memoryOffset, memoryRange));
	if (bufferView != nullptr)
	{
		if (!bufferView->init())
		{
			bufferView.reset();
		}
	}
	return bufferView;
}

BufferView::~BufferView()
{
	ObjectTracker::unregisterObject(ObjectType_BufferView);

	release();
}

bool BufferView::isInitialized() const
{
	return mBufferView != VK_NULL_HANDLE;
}

const VkBufferView& BufferView::getHandle() const
{
	return mBufferView;
}

const VkBuffer& BufferView::getBufferHandle() const
{
	return mBuffer.getHandle();
}

const VkDevice& BufferView::getDeviceHandle() const
{
	return mDevice.getHandle();
}

BufferView::BufferView(Device& device, Buffer& buffer, VkFormat format, VkDeviceSize memoryOffset, VkDeviceSize memoryRange)
	: mDevice(device)
	, mBuffer(buffer)
	, mBufferView(VK_NULL_HANDLE)
	, mFormat(format)
	, mMemoryOffset(memoryOffset)
	, mMemoryRange(memoryRange)
{
	ObjectTracker::registerObject(ObjectType_BufferView);
}

bool BufferView::init()
{
	VkBufferViewCreateInfo bufferViewCreateInfo = {
		VK_STRUCTURE_TYPE_BUFFER_VIEW_CREATE_INFO,    // VkStructureType            sType
		nullptr,                                      // const void               * pNext
		0,                                            // VkBufferViewCreateFlags    flags
		mBuffer.getHandle(),                          // VkBuffer                   buffer
		mFormat,                                      // VkFormat                   format
		mMemoryOffset,                                // VkDeviceSize               offset
		mMemoryRange                                  // VkDeviceSize               range
	};

	VkResult result = vkCreateBufferView(mDevice.getHandle(), &bufferViewCreateInfo, nullptr, &mBufferView);
	if (result != VK_SUCCESS || mBufferView == VK_NULL_HANDLE) 
	{
		// TODO : Use Numea Log System
		printf("Could not create buffer view\n");
		return false;
	}
	return true;
}

bool BufferView::release()
{
	if (mBufferView != VK_NULL_HANDLE)
	{
		vkDestroyBufferView(mDevice.getHandle(), mBufferView, nullptr);
		mBufferView = VK_NULL_HANDLE;
		return true;
	}
	return false;
}

} // namespace Vulkan
} // namespace nu