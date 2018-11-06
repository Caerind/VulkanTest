#include "VulkanBuffer.hpp"

#include "VulkanDevice.hpp"

namespace nu
{
namespace Vulkan
{

Buffer::~Buffer()
{
	release();

	ObjectTracker::unregisterObject(ObjectType_Buffer);
}

const VkMemoryRequirements& Buffer::getMemoryRequirements() const
{
	if (!mMemoryRequirementsQueried)
	{
		mMemoryRequirementsQueried = true;
		vkGetBufferMemoryRequirements(mDevice.getHandle(), mBuffer, &mMemoryRequirements);
	}

	return mMemoryRequirements;
}

MemoryBlock* Buffer::allocateMemoryBlock(VkMemoryPropertyFlagBits memoryProperties)
{
	mOwnedMemoryBlock = mDevice.createMemoryBlock(getMemoryRequirements(), memoryProperties);
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

bool Buffer::ownMemoryBlock() const
{
	return mOwnedMemoryBlock != nullptr;
}

bool Buffer::isBoundToMemoryBlock() const
{
	return mMemoryBlock != nullptr;
}

MemoryBlock* Buffer::getMemoryBlock()
{
	return mMemoryBlock;
}

VkDeviceSize Buffer::getOffsetInMemoryBlock() const
{
	return mOffsetInMemoryBlock;
}

BufferView* Buffer::createBufferView(VkFormat format, VkDeviceSize memoryOffset, VkDeviceSize memoryRange)
{
	BufferView::Ptr bufferView = BufferView::createBufferView(*this, format, memoryOffset, memoryRange);
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

BufferView* Buffer::getBufferView(uint32_t index)
{
	assert(index < getBufferViewCount());
	return mBufferViews[index].get();
}

const BufferView* Buffer::getBufferView(uint32_t index) const
{
	assert(index < getBufferViewCount());
	return mBufferViews[index].get();
}

uint32_t Buffer::getBufferViewCount() const
{
	return (uint32_t)mBufferViews.size();
}

void Buffer::clearBufferViews()
{
	mBufferViews.clear();
}

VkDeviceSize Buffer::getSize() const
{
	return mSize;
}

VkBufferUsageFlags Buffer::getUsage() const
{
	return mUsage;
}

Device& Buffer::getDevice()
{
	return mDevice;
}

const Device& Buffer::getDevice() const
{
	return mDevice;
}

const VkBuffer& Buffer::getHandle() const
{
	return mBuffer;
}

const VkDevice& Buffer::getDeviceHandle() const
{
	return mDevice.getHandle();
}

Buffer::Ptr Buffer::createBuffer(Device& device, VkDeviceSize size, VkBufferUsageFlags usage)
{
	Buffer::Ptr buffer(new Buffer(device, size, usage));
	if (buffer != nullptr)
	{
		if (!buffer->init())
		{
			buffer.reset();
		}
	}
	return buffer;
}

Buffer::Buffer(Device& device, VkDeviceSize size, VkBufferUsageFlags usage)
	: mDevice(device)
	, mBuffer(VK_NULL_HANDLE)
	, mOwnedMemoryBlock(nullptr)
	, mMemoryBlock(nullptr)
	, mOffsetInMemoryBlock(0)
	, mBufferViews()
	, mSize(size)
	, mUsage(usage)
	, mMemoryRequirementsQueried(false)
	, mMemoryRequirements()
{
	ObjectTracker::registerObject(ObjectType_Buffer);
}

bool Buffer::init()
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

	VkResult result = vkCreateBuffer(mDevice.getHandle(), &bufferCreateInfo, nullptr, &mBuffer);
	if (result != VK_SUCCESS || mBuffer == VK_NULL_HANDLE)
	{
		// TODO : Use Numea Log System
		printf("Could not create buffer\n");
		return false;
	}

	return true;
}

void Buffer::release()
{
	if (mBuffer != VK_NULL_HANDLE)
	{
		vkDestroyBuffer(mDevice.getHandle(), mBuffer, nullptr);
		mBuffer = VK_NULL_HANDLE;
	}
}

void Buffer::bindToMemoryBlock(MemoryBlock* memoryBlock, VkDeviceSize offsetInMemoryBlock)
{
	mMemoryBlock = memoryBlock;
	mOffsetInMemoryBlock = offsetInMemoryBlock;
}

} // namespace Vulkan
} // namespace nu