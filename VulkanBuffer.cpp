#include "VulkanBuffer.hpp"

#include "VulkanDevice.hpp"

namespace nu
{
namespace Vulkan
{

Buffer::Ptr Buffer::createBuffer(Device& device, VkDeviceSize size, VkBufferUsageFlags usage)
{
	Buffer::Ptr buffer(new Buffer(device));
	if (buffer != nullptr)
	{
		if (!buffer->init(size, usage))
		{
			buffer.reset();
		}
	}
	return buffer;
}

Buffer::~Buffer()
{
	ObjectTracker::unregisterObject(ObjectType_Buffer);

	release();
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

MemoryBlock::Ptr Buffer::allocateAndBindMemoryBlock(VkMemoryPropertyFlagBits memoryProperties)
{
	return MemoryBlock::createMemoryBlock(this, memoryProperties);
}

BufferView::Ptr Buffer::createBufferView(VkFormat format, VkDeviceSize memoryOffset, VkDeviceSize memoryRange)
{
	return BufferView::createBufferView(mDevice, *this, format, memoryOffset, memoryRange);
}

bool Buffer::isInitialized() const 
{
	return mBuffer != VK_NULL_HANDLE;
}

const VkBuffer& Buffer::getHandle() const
{
	return mBuffer;
}

const VkDevice& Buffer::getDeviceHandle() const
{
	return mDevice.getHandle();
}

Device& Buffer::getDevice()
{
	return mDevice;
}

const Device& Buffer::getDevice() const
{
	return mDevice;
}

Buffer::Buffer(Device& device)
	: mDevice(device)
	, mBuffer(VK_NULL_HANDLE)
	, mMemoryRequirementsQueried(false)
	, mMemoryRequirements()
{
	ObjectTracker::registerObject(ObjectType_Buffer);
}

bool Buffer::init(VkDeviceSize size, VkBufferUsageFlags usage)
{
	VkBufferCreateInfo bufferCreateInfo = {
		VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO,   // VkStructureType        sType
		nullptr,                                // const void           * pNext
		0,                                      // VkBufferCreateFlags    flags
		size,                                   // VkDeviceSize           size
		usage,                                  // VkBufferUsageFlags     usage
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

bool Buffer::release()
{
	if (mBuffer != VK_NULL_HANDLE)
	{
		vkDestroyBuffer(mDevice.getHandle(), mBuffer, nullptr);
		mBuffer = VK_NULL_HANDLE;
		return true;
	}
	return false;
}

} // namespace Vulkan
} // namespace nu