#include "VulkanBufferView.hpp"

#include "VulkanBuffer.hpp"
#include "VulkanDevice.hpp"

VULKAN_NAMESPACE_BEGIN

VulkanBufferView::~VulkanBufferView()
{
	release();

	VULKAN_OBJECTTRACKER_UNREGISTER();
}

const VkFormat& VulkanBufferView::getFormat() const
{
	return mFormat;
}

const VkDeviceSize& VulkanBufferView::getOffset() const
{
	return mOffset;
}

const VkDeviceSize& VulkanBufferView::getSize() const
{
	return mSize;
}

VulkanBuffer& VulkanBufferView::getBuffer()
{
	return mBuffer;
}

const VulkanBuffer& VulkanBufferView::getBuffer() const
{
	return mBuffer;
}

const VkBufferView& VulkanBufferView::getHandle() const
{
	return mBufferView;
}

const VkBuffer& VulkanBufferView::getBufferHandle() const
{
	return mBuffer.getHandle();
}

VulkanBufferViewPtr VulkanBufferView::createBufferView(VulkanBuffer& buffer, VkFormat format, VkDeviceSize offset, VkDeviceSize size)
{
	VulkanBufferViewPtr bufferView(new VulkanBufferView(buffer, format, offset, size));
	if (bufferView != nullptr)
	{
		if (!bufferView->init())
		{
			bufferView.reset();
		}
	}
	return bufferView;
}

VulkanBufferView::VulkanBufferView(VulkanBuffer& buffer, VkFormat format, VkDeviceSize offset, VkDeviceSize size)
	: mBuffer(buffer)
	, mBufferView(VK_NULL_HANDLE)
	, mFormat(format)
	, mOffset(offset)
	, mSize(size)
{
	VULKAN_OBJECTTRACKER_REGISTER();
}

bool VulkanBufferView::init()
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

	VkResult result = vkCreateBufferView(mBuffer.getDeviceHandle(), &bufferViewCreateInfo, nullptr, &mBufferView);
	if (result != VK_SUCCESS || mBufferView == VK_NULL_HANDLE) 
	{
		VULKAN_LOG_ERROR("Could not create buffer view");
		return false;
	}
	return true;
}

void VulkanBufferView::release()
{
	if (mBufferView != VK_NULL_HANDLE)
	{
		vkDestroyBufferView(mBuffer.getDeviceHandle(), mBufferView, nullptr);
		mBufferView = VK_NULL_HANDLE;
	}
}

VULKAN_NAMESPACE_END