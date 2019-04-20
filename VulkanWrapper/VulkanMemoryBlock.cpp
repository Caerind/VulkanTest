#include "VulkanMemoryBlock.hpp"

#include "VulkanDevice.hpp"
#include "VulkanPhysicalDevice.hpp"
#include "VulkanBuffer.hpp"
#include "VulkanImage.hpp"

VULKAN_NAMESPACE_BEGIN

VulkanMemoryBlock::~VulkanMemoryBlock()
{
	unmap();

	release();

	VULKAN_OBJECTTRACKER_UNREGISTER();
}

bool VulkanMemoryBlock::map(VkDeviceSize offset, VkDeviceSize size)
{
	unmap();

	VULKAN_ASSERT(offset < mMemoryRequirements.size, "offset must be less than the size of memory");
	VULKAN_ASSERT(size == VK_WHOLE_SIZE || size > 0, "If size is not equal to VK_WHOLE_SIZE, size must be greater than 0");
	VULKAN_ASSERT(size == VK_WHOLE_SIZE || size <= mMemoryRequirements.size - offset, "If size is not equal to VK_WHOLE_SIZE, size must be less than or equal to the size of the memory minus offset");
	VULKAN_ASSERT(isHostVisible(), "memory must have been created with a memory type that reports VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT");

	VkResult result = vkMapMemory(getDeviceHandle(), mMemoryBlock, offset, size, 0, &mMappedData);
	if (result != VK_SUCCESS)
	{
		// TODO : Log more
		VULKAN_LOG_ERROR("Could not map memory object\n");
		return false;
	}

	mMappedOffset = offset;
	mMappedSize = size;

	return true;
}

bool VulkanMemoryBlock::read(void* data)
{
	VULKAN_ASSERT(isMapped(), "memory must be mapped to be read");

	std::memcpy(data, mMappedData, mMappedSize);

	return true;
}

bool VulkanMemoryBlock::write(const void* data)
{
	VULKAN_ASSERT(isMapped(), "memory must be mapped to be written");

	std::memcpy(mMappedData, data, mMappedSize);

	if (!isHostCoherent())
	{
		std::vector<VkMappedMemoryRange> memoryRanges = {
			{
				VK_STRUCTURE_TYPE_MAPPED_MEMORY_RANGE,  // VkStructureType    sType
				nullptr,                                // const void       * pNext
				mMemoryBlock,                           // VkDeviceMemory     memory
				mMappedOffset,                          // VkDeviceSize       offset
				mMappedSize                             // VkDeviceSize       size
			}
		};
		// TODO : Instead of mMappedSize, there was VK_WHOLE_SIZE. Is mMappedSize better ? (Probably, but need to be sure)
		// TODO : Allow multiple memory ranges flushing
		// TODO : Flush only if wanted ?
		// TODO : Invalidate instead of Flush ?
		VkResult result = vkFlushMappedMemoryRanges(getDeviceHandle(), static_cast<uint32_t>(memoryRanges.size()), memoryRanges.data());
		if (result != VK_SUCCESS)
		{
			// TODO : Use Numea System Log
			printf("Could not flush mapped memory\n");
			return false;
		}
	}

	return true;
}

bool VulkanMemoryBlock::unmap()
{
	if (isMapped())
	{
		vkUnmapMemory(getDeviceHandle(), mMemoryBlock);
		mMappedData = nullptr;
		mMappedOffset = 0;
		mMappedSize = 0;
	}
	return true;
}

bool VulkanMemoryBlock::bind(VulkanBuffer* buffer, VkDeviceSize offsetInMemory)
{
	VULKAN_ASSERT(buffer != nullptr, "Valid buffer must be provided");
	// TODO : Add others assert for valid usage from the specifications

	VkResult result = vkBindBufferMemory(getDeviceHandle(), buffer->getHandle(), mMemoryBlock, offsetInMemory);
	if (result != VK_SUCCESS)
	{
		// TODO : Use Numea Log System
		printf("Could not bind memory block to a buffer\n");
		return false;
	}

	buffer->bindToMemoryBlock(this, offsetInMemory);

	return true;
}

bool VulkanMemoryBlock::bind(VulkanImage* image, VkDeviceSize memoryOffset)
{
	VULKAN_ASSERT(image != nullptr, "Valid image must be provided");
	// TODO : Add others assert for valid usage from the specifications

	VkResult result = vkBindImageMemory(getDeviceHandle(), image->getHandle(), mMemoryBlock, memoryOffset);
	if (result != VK_SUCCESS)
	{
		// TODO : Use Numea Log System
		printf("Could not bind memory block to a buffer\n");
		return false;
	}

	image->bindToMemoryBlock(this, memoryOffset);

	return true;
}

bool VulkanMemoryBlock::isHostVisible() const
{
	return (mMemoryProperties & VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT) != 0;
}

bool VulkanMemoryBlock::isDeviceLocal() const
{
	return (mMemoryProperties & VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT) != 0;
}

bool VulkanMemoryBlock::isHostCoherent() const
{
	return (mMemoryProperties & VK_MEMORY_PROPERTY_HOST_COHERENT_BIT) != 0;
}

VkDeviceSize VulkanMemoryBlock::getSize() const
{
	return mMemoryRequirements.size;
}

VkDeviceSize VulkanMemoryBlock::getAlignment() const
{
	return mMemoryRequirements.alignment;
}

uint32_t VulkanMemoryBlock::getMemoryType() const
{
	return mMemoryRequirements.memoryTypeBits;
}

VkMemoryPropertyFlags VulkanMemoryBlock::getMemoryProperties() const
{
	return mMemoryProperties;
}

bool VulkanMemoryBlock::isMapped() const
{
	return mMappedData != nullptr;
}

VkDeviceSize VulkanMemoryBlock::getMappedOffset() const
{
	return mMappedOffset;
}

VkDeviceSize VulkanMemoryBlock::getMappedSize() const
{
	return mMappedSize;
}

const VkDeviceMemory& VulkanMemoryBlock::getHandle() const
{
	return mMemoryBlock;
}

VulkanMemoryBlockPtr VulkanMemoryBlock::createMemoryBlock(VkMemoryRequirements memoryRequirements, VkMemoryPropertyFlags memoryProperties)
{
	VulkanMemoryBlockPtr memoryBlock(new VulkanMemoryBlock(memoryRequirements, memoryProperties));
	if (memoryBlock != nullptr)
	{
		if (!memoryBlock->init())
		{
			memoryBlock.reset();
		}
	}
	return memoryBlock;
}

VulkanMemoryBlock::VulkanMemoryBlock(VkMemoryRequirements memoryRequirements, VkMemoryPropertyFlags memoryProperties)
	: mMemoryBlock(VK_NULL_HANDLE)
	, mMemoryRequirements(memoryRequirements)
	, mMemoryProperties(memoryProperties)
	, mMappedData(nullptr)
	, mMappedOffset(0)
	, mMappedSize(0)
{
	VULKAN_OBJECTTRACKER_REGISTER();
}

bool VulkanMemoryBlock::init()
{
	const VkPhysicalDeviceMemoryProperties& deviceMemoryProperties = getDevice().getMemoryProperties();
	const VkMemoryRequirements& memoryRequirements = mMemoryRequirements;
		
	for (uint32_t type = 0; type < deviceMemoryProperties.memoryTypeCount; type++) 
	{
		VkMemoryPropertyFlags deviceMemoryPropertiesFlags = deviceMemoryProperties.memoryTypes[type].propertyFlags;
		if ((memoryRequirements.memoryTypeBits & (1 << type)) && ((deviceMemoryPropertiesFlags & mMemoryProperties) == mMemoryProperties)) 
		{
			VkMemoryAllocateInfo bufferMemoryAllocateInfo = {
				VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO,   // VkStructureType    sType
				nullptr,                                  // const void       * pNext
				memoryRequirements.size,                  // VkDeviceSize       allocationSize
				type                                      // uint32_t           memoryTypeIndex
			};

			VkResult result = vkAllocateMemory(getDeviceHandle(), &bufferMemoryAllocateInfo, nullptr, &mMemoryBlock);
			if (result == VK_SUCCESS) 
			{
				mMemoryRequirements.memoryTypeBits = type;
				mMemoryProperties = deviceMemoryPropertiesFlags;
				break;
			}
		}
	}

	if (mMemoryBlock == VK_NULL_HANDLE) 
	{
		VULKAN_LOG_ERROR("Could not allocate memory for a buffer");
		return false;
	}

	return true;
}

void VulkanMemoryBlock::release()
{
	if (mMemoryBlock != VK_NULL_HANDLE)
	{
		vkFreeMemory(getDeviceHandle(), mMemoryBlock, nullptr);
		mMemoryBlock = VK_NULL_HANDLE;
	}
}

VULKAN_NAMESPACE_END