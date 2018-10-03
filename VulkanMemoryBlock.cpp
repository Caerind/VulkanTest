#include "VulkanMemoryBlock.hpp"

#include "VulkanDevice.hpp"
#include "VulkanBuffer.hpp"
#include "VulkanImage.hpp"

namespace nu
{
namespace Vulkan
{

MemoryBlock::Ptr MemoryBlock::createMemoryBlock(Buffer* buffer, VkMemoryPropertyFlagBits memoryProperties)
{
	MemoryBlock::Ptr memoryBlock(new MemoryBlock(buffer->getDevice(), buffer, memoryProperties));
	if (memoryBlock != nullptr)
	{
		if (!memoryBlock->init())
		{
			memoryBlock.reset();
		}
	}
	return memoryBlock;
}

MemoryBlock::Ptr MemoryBlock::createMemoryBlock(Image* image, VkMemoryPropertyFlagBits memoryProperties)
{
	MemoryBlock::Ptr memoryBlock(new MemoryBlock(image->getDevice(), image, memoryProperties));
	if (memoryBlock != nullptr)
	{
		if (!memoryBlock->init())
		{
			memoryBlock.reset();
		}
	}
	return memoryBlock;
}

MemoryBlock::~MemoryBlock()
{
	ObjectTracker::unregisterObject(ObjectType_MemoryBlock);

	release();
}

bool MemoryBlock::mapUpdateAndUnmapHostVisibleMemory(VkDeviceSize offset, VkDeviceSize dataSize, void* data, bool unmap, void** pointer)
{
	VkResult result;
	void* localPointer;

	result = vkMapMemory(mDevice.getHandle(), mMemoryBlock, offset, dataSize, 0, &localPointer); // TODO : Use offset
	if (result != VK_SUCCESS)
	{
		// TODO : Use Numea System Log
		printf("Could not map memory object\n");
		return false;
	}

	std::memcpy(localPointer, data, static_cast<size_t>(dataSize));

	std::vector<VkMappedMemoryRange> memoryRanges = {
		{
			VK_STRUCTURE_TYPE_MAPPED_MEMORY_RANGE,  // VkStructureType    sType
			nullptr,                                // const void       * pNext
			mMemoryBlock,                           // VkDeviceMemory     memory
			offset,                                 // VkDeviceSize       offset
			VK_WHOLE_SIZE                           // VkDeviceSize       size
		}
	};

	vkFlushMappedMemoryRanges(mDevice.getHandle(), static_cast<uint32_t>(memoryRanges.size()), memoryRanges.data());
	if (result != VK_SUCCESS)
	{
		// TODO : Use Numea System Log
		printf("Could not flush mapped memory\n");
		return false;
	}

	if (unmap)
	{
		vkUnmapMemory(mDevice.getHandle(), mMemoryBlock);
	}
	else if (pointer != nullptr)
	{
		*pointer = localPointer;
	}

	return true;
}

bool MemoryBlock::isInitialized() const
{
	return mMemoryBlock != VK_NULL_HANDLE;
}

const VkDeviceMemory& MemoryBlock::getHandle() const
{
	return mMemoryBlock;
}

const VkDevice& MemoryBlock::getDeviceHandle() const
{
	return mDevice.getHandle();
}

MemoryBlock::MemoryBlock(Device& device, Buffer* buffer, VkMemoryPropertyFlagBits memoryProperties)
	: mDevice(device)
	, mBuffer(buffer)
	, mImage(nullptr)
	, mMemoryBlock(VK_NULL_HANDLE)
	, mMemoryProperties(memoryProperties)
{
	ObjectTracker::registerObject(ObjectType_MemoryBlock);
}

MemoryBlock::MemoryBlock(Device& device, Image* image, VkMemoryPropertyFlagBits memoryProperties)
	: mDevice(device)
	, mBuffer(nullptr)
	, mImage(image)
	, mMemoryBlock(VK_NULL_HANDLE)
	, mMemoryProperties(memoryProperties)
{
	ObjectTracker::registerObject(ObjectType_MemoryBlock);
}

bool MemoryBlock::init()
{
	if (mBuffer != nullptr)
	{
		const VkPhysicalDeviceMemoryProperties& deviceMemoryProperties = mDevice.getPhysicalDevice().getMemoryProperties();
		const VkMemoryRequirements& bufferMemoryRequirements = mBuffer->getMemoryRequirements();
		
		for (uint32_t type = 0; type < deviceMemoryProperties.memoryTypeCount; type++) 
		{
			if ((bufferMemoryRequirements.memoryTypeBits & (1 << type)) && ((deviceMemoryProperties.memoryTypes[type].propertyFlags & mMemoryProperties) == mMemoryProperties)) 
			{
				VkMemoryAllocateInfo bufferMemoryAllocateInfo = {
					VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO,   // VkStructureType    sType
					nullptr,                                  // const void       * pNext
					bufferMemoryRequirements.size,            // VkDeviceSize       allocationSize
					type                                      // uint32_t           memoryTypeIndex
				};

				VkResult result = vkAllocateMemory(mDevice.getHandle(), &bufferMemoryAllocateInfo, nullptr, &mMemoryBlock);
				if (VK_SUCCESS == result) {
					break;
				}
			}
		}

		if (mMemoryBlock == VK_NULL_HANDLE) 
		{
			// TODO : Use Numea Log System
			printf("Could not allocate memory for a buffer\n");
			return false;
		}

		VkResult result = vkBindBufferMemory(mDevice.getHandle(), mBuffer->getHandle(), mMemoryBlock, 0);
		if (result != VK_SUCCESS)
		{
			// TODO : Use Numea Log System
			printf("Could not bind memory block to a buffer\n");
			return false;
		}

		return true;
	}
	else if (mImage != nullptr)
	{
		const VkPhysicalDeviceMemoryProperties& deviceMemoryProperties = mDevice.getPhysicalDevice().getMemoryProperties();
		const VkMemoryRequirements& imageMemoryRequirements = mImage->getMemoryRequirements();

		for (uint32_t type = 0; type < deviceMemoryProperties.memoryTypeCount; type++)
		{
			if ((imageMemoryRequirements.memoryTypeBits & (1 << type)) && ((deviceMemoryProperties.memoryTypes[type].propertyFlags & mMemoryProperties) == mMemoryProperties))
			{
				VkMemoryAllocateInfo imageMemoryAllocateInfo = {
					VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO,   // VkStructureType    sType
					nullptr,                                  // const void       * pNext
					imageMemoryRequirements.size,             // VkDeviceSize       allocationSize
					type                                      // uint32_t           memoryTypeIndex
				};

				VkResult result = vkAllocateMemory(mDevice.getHandle(), &imageMemoryAllocateInfo, nullptr, &mMemoryBlock);
				if (result == VK_SUCCESS)
				{
					break;
				}
			}
		}

		if (mMemoryBlock == VK_NULL_HANDLE)
		{
			// TODO : Use Numea Log System
			printf("Could not allocate memory for an image\n");
			return false;
		}

		VkResult result = vkBindImageMemory(mDevice.getHandle(), mImage->getHandle(), mMemoryBlock, 0);
		if (result != VK_SUCCESS)
		{
			// TODO : Use Numea Log System
			printf("Could not bind memory block to a buffer\n");
			return false;
		}

		return true;
	}
	else
	{
		// TODO : Use Numea Log System
		printf("Not implement yet\n");
	}
	return false;
}

bool MemoryBlock::release()
{
	if (mMemoryBlock != VK_NULL_HANDLE)
	{
		vkFreeMemory(mDevice.getHandle(), mMemoryBlock, nullptr);
		mMemoryBlock = VK_NULL_HANDLE;
		return true;
	}
	return false;
}

} // namespace Vulkan
} // namespace nu