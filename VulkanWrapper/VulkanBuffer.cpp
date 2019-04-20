#include "VulkanBuffer.hpp"

#include "VulkanBufferView.hpp"
#include "VulkanDevice.hpp"
#include "VulkanMemoryBlock.hpp"

VULKAN_NAMESPACE_BEGIN

VulkanBuffer::~VulkanBuffer()
{
	release();
	
	VULKAN_OBJECTTRACKER_REGISTER();
}

const VkMemoryRequirements& VulkanBuffer::getMemoryRequirements() const
{
	// TODO : Move this elsewhere and query on loading
	if (!mMemoryRequirementsQueried)
	{
		mMemoryRequirementsQueried = true;
		vkGetBufferMemoryRequirements(getDeviceHandle(), mBuffer, &mMemoryRequirements);
	}

	return mMemoryRequirements;
}

VulkanMemoryBlock* VulkanBuffer::allocateMemoryBlock(VkMemoryPropertyFlagBits memoryProperties)
{
	mOwnedMemoryBlock = getDevice().createMemoryBlock(getMemoryRequirements(), memoryProperties);
	if (mOwnedMemoryBlock != nullptr)
	{
		mOwnedMemoryBlock->bind(this, 0);
	}
	else
	{
		mMemoryBlock = nullptr;
	}
	return mMemoryBlock;
}

bool VulkanBuffer::ownMemoryBlock() const
{
	return mOwnedMemoryBlock != nullptr;
}

bool VulkanBuffer::isBoundToMemoryBlock() const
{
	return mMemoryBlock != nullptr;
}

VulkanMemoryBlock* VulkanBuffer::getMemoryBlock()
{
	return mMemoryBlock;
}

VkDeviceSize VulkanBuffer::getOffsetInMemoryBlock() const
{
	return mOffsetInMemoryBlock;
}

VulkanBufferView* VulkanBuffer::createBufferView(VkFormat format, VkDeviceSize memoryOffset, VkDeviceSize memoryRange)
{
	VulkanBufferViewPtr bufferView = VulkanBufferView::createBufferView(*this, format, memoryOffset, memoryRange);
	if (bufferView != nullptr)
	{
		mBufferViews.emplace_back(std::move(bufferView));
		return mBufferViews.back().get();
	}
	else
	{
		return nullptr;
	}
}

VulkanBufferView* VulkanBuffer::getBufferView(VulkanU32 index)
{
	VULKAN_ASSERT(index < getBufferViewCount());
	return mBufferViews[index].get();
}

const VulkanBufferView* VulkanBuffer::getBufferView(VulkanU32 index) const
{
	VULKAN_ASSERT(index < getBufferViewCount());
	return mBufferViews[index].get();
}

VulkanU32 VulkanBuffer::getBufferViewCount() const
{
	return (VulkanU32)mBufferViews.size();
}

void VulkanBuffer::clearBufferViews()
{
	mBufferViews.clear();
}

VkDeviceSize VulkanBuffer::getSize() const
{
	return mSize;
}

VkBufferUsageFlags VulkanBuffer::getUsage() const
{
	return mUsage;
}

const VkBuffer& VulkanBuffer::getHandle() const
{
	return mBuffer;
}

VulkanBufferPtr VulkanBuffer::createBuffer(VkDeviceSize size, VkBufferUsageFlags usage)
{
	VulkanBufferPtr buffer(new VulkanBuffer(size, usage));
	if (buffer != nullptr)
	{
		if (!buffer->init())
		{
			buffer.reset();
		}
	}
	return buffer;
}

VulkanBuffer::VulkanBuffer(VkDeviceSize size, VkBufferUsageFlags usage)
	: mBuffer(VK_NULL_HANDLE)
	, mOwnedMemoryBlock(nullptr)
	, mMemoryBlock(nullptr)
	, mOffsetInMemoryBlock(0)
	, mBufferViews()
	, mSize(size)
	, mUsage(usage)
	, mMemoryRequirementsQueried(false)
	, mMemoryRequirements()
{
	VULKAN_OBJECTTRACKER_REGISTER();
}

bool VulkanBuffer::init()
{
	VkBufferCreateInfo bufferCreateInfo = {
		VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO,   // VkStructureType        sType
		nullptr,                                // const void           * pNext
		0,                                      // VkBufferCreateFlags    flags
		mSize,                                  // VkDeviceSize           size
		mUsage,                                 // VkBufferUsageFlags     usage
		VK_SHARING_MODE_EXCLUSIVE,              // VkSharingMode          sharingMode
		0,                                      // uint32_t               queueFamilyIndexCount
		nullptr                                 // const uint32_t       * pQueueFamilyIndices
	};

	VkResult result = vkCreateBuffer(getDeviceHandle(), &bufferCreateInfo, nullptr, &mBuffer);
	if (result != VK_SUCCESS || mBuffer == VK_NULL_HANDLE)
	{
		VULKAN_LOG_ERROR("Could not create buffer");
		return false;
	}

	return true;
}

void VulkanBuffer::release()
{
	if (mBuffer != VK_NULL_HANDLE)
	{
		vkDestroyBuffer(getDeviceHandle(), mBuffer, nullptr);
		mBuffer = VK_NULL_HANDLE;
	}
}

void VulkanBuffer::bindToMemoryBlock(VulkanMemoryBlock* memoryBlock, VkDeviceSize offsetInMemoryBlock)
{
	mMemoryBlock = memoryBlock;
	mOffsetInMemoryBlock = offsetInMemoryBlock;
}

VULKAN_NAMESPACE_END