#include "VulkanBufferView.hpp"

#include "VulkanDevice.hpp"
#include "VulkanBuffer.hpp"

namespace nu
{
namespace Vulkan
{

BufferView::~BufferView()
{
	release();

	ObjectTracker::unregisterObject(ObjectType_BufferView);
}

VkFormat BufferView::getFormat() const
{
	return mFormat;
}

VkDeviceSize BufferView::getOffset() const
{
	return mOffset;
}

VkDeviceSize BufferView::getSize() const
{
	return mSize;
}

Buffer& BufferView::getBuffer()
{
	return mBuffer;
}

const Buffer& BufferView::getBuffer() const
{
	return mBuffer;
}

Device& BufferView::getDevice()
{
	return mDevice;
}

const Device& BufferView::getDevice() const
{
	return mDevice;
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

BufferView::Ptr BufferView::createBufferView(Buffer& buffer, VkFormat format, VkDeviceSize offset, VkDeviceSize size)
{
	BufferView::Ptr bufferView(new BufferView(buffer, format, offset, size));
	if (bufferView != nullptr)
	{
		if (!bufferView->init())
		{
			bufferView.reset();
		}
	}
	return bufferView;
}

BufferView::BufferView(Buffer& buffer, VkFormat format, VkDeviceSize offset, VkDeviceSize size)
	: mDevice(buffer.getDevice())
	, mBuffer(buffer)
	, mBufferView(VK_NULL_HANDLE)
	, mFormat(format)
	, mOffset(offset)
	, mSize(size)
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
		mOffset,                                      // VkDeviceSize               offset
		mSize                                         // VkDeviceSize               range
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

void BufferView::release()
{
	if (mBufferView != VK_NULL_HANDLE)
	{
		vkDestroyBufferView(mDevice.getHandle(), mBufferView, nullptr);
		mBufferView = VK_NULL_HANDLE;
	}
}

} // namespace Vulkan
} // namespace nu