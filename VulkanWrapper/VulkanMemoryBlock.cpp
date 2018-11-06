#include "VulkanMemoryBlock.hpp"

#include "VulkanDevice.hpp"
#include "VulkanBuffer.hpp"
#include "VulkanImage.hpp"

namespace nu
{
namespace Vulkan
{

MemoryBlock::~MemoryBlock()
{
	unmap();

	release();

	ObjectTracker::unregisterObject(ObjectType_MemoryBlock);
}

bool MemoryBlock::map(VkDeviceSize offset, VkDeviceSize size)
{
	unmap();

	assert(offset < mMemoryRequirements.size, "offset must be less than the size of memory");
	assert(size == VK_WHOLE_SIZE || size > 0, "If size is not equal to VK_WHOLE_SIZE, size must be greater than 0");
	assert(size == VK_WHOLE_SIZE || size <= mMemoryRequirements.size - offset, "If size is not equal to VK_WHOLE_SIZE, size must be less than or equal to the size of the memory minus offset");
	assert(isHostVisible(), "memory must have been created with a memory type that reports VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT");

	VkResult result = vkMapMemory(mDevice.getHandle(), mMemoryBlock, offset, size, 0, &mMappedData);
	if (result != VK_SUCCESS)
	{
		// TODO : Use Numea System Log
		printf("Could not map memory object\n");
		return false;
	}

	mMappedOffset = offset;
	mMappedSize = size;

	return true;
}

bool MemoryBlock::read(void* data)
{
	assert(isMapped(), "memory must be mapped to be read");

	std::memcpy(data, mMappedData, mMappedSize);

	return true;
}

bool MemoryBlock::write(const void* data)
{
	assert(isMapped(), "memory must be mapped to be written");

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
		VkResult result = vkFlushMappedMemoryRanges(mDevice.getHandle(), static_cast<uint32_t>(memoryRanges.size()), memoryRanges.data());
		if (result != VK_SUCCESS)
		{
			// TODO : Use Numea System Log
			printf("Could not flush mapped memory\n");
			return false;
		}
	}

	return true;
}

bool MemoryBlock::unmap()
{
	if (isMapped())
	{
		vkUnmapMemory(mDevice.getHandle(), mMemoryBlock);
		mMappedData = nullptr;
		mMappedOffset = 0;
		mMappedSize = 0;
	}
	return true;
}

bool MemoryBlock::bind(Buffer* buffer, VkDeviceSize offsetInMemory)
{
	assert(buffer != nullptr, "Valid buffer must be provided");
	// TODO : Add others assert for valid usage from the specifications

	VkResult result = vkBindBufferMemory(mDevice.getHandle(), buffer->getHandle(), mMemoryBlock, offsetInMemory);
	if (result != VK_SUCCESS)
	{
		// TODO : Use Numea Log System
		printf("Could not bind memory block to a buffer\n");
		return false;
	}

	buffer->bindToMemoryBlock(this, offsetInMemory);

	return true;
}

bool MemoryBlock::bind(Image* image, VkDeviceSize memoryOffset)
{
	assert(image != nullptr, "Valid image must be provided");
	// TODO : Add others assert for valid usage from the specifications

	VkResult result = vkBindImageMemory(mDevice.getHandle(), image->getHandle(), mMemoryBlock, memoryOffset);
	if (result != VK_SUCCESS)
	{
		// TODO : Use Numea Log System
		printf("Could not bind memory block to a buffer\n");
		return false;
	}

	image->bindToMemoryBlock(this, memoryOffset);

	return true;
}

bool MemoryBlock::isHostVisible() const
{
	return (mMemoryProperties & VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT) != 0;
}

bool MemoryBlock::isDeviceLocal() const
{
	return (mMemoryProperties & VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT) != 0;
}

bool MemoryBlock::isHostCoherent() const
{
	return (mMemoryProperties & VK_MEMORY_PROPERTY_HOST_COHERENT_BIT) != 0;
}

VkDeviceSize MemoryBlock::getSize() const
{
	return mMemoryRequirements.size;
}

VkDeviceSize MemoryBlock::getAlignment() const
{
	return mMemoryRequirements.alignment;
}

uint32_t MemoryBlock::getMemoryType() const
{
	return mMemoryRequirements.memoryTypeBits;
}

VkMemoryPropertyFlags MemoryBlock::getMemoryProperties() const
{
	return mMemoryProperties;
}

bool MemoryBlock::isMapped() const
{
	return mMappedData != nullptr;
}

VkDeviceSize MemoryBlock::getMappedOffset() const
{
	return mMappedOffset;
}

VkDeviceSize MemoryBlock::getMappedSize() const
{
	return mMappedSize;
}

Device& MemoryBlock::getDevice()
{
	return mDevice;
}

const Device& MemoryBlock::getDevice() const
{
	return mDevice;
}

const VkDeviceMemory& MemoryBlock::getHandle() const
{
	return mMemoryBlock;
}

const VkDevice& MemoryBlock::getDeviceHandle() const
{
	return mDevice.getHandle();
}

MemoryBlock::Ptr MemoryBlock::createMemoryBlock(Device& device, VkMemoryRequirements memoryRequirements, VkMemoryPropertyFlags memoryProperties)
{
	MemoryBlock::Ptr memoryBlock(new MemoryBlock(device, memoryRequirements, memoryProperties));
	if (memoryBlock != nullptr)
	{
		if (!memoryBlock->init())
		{
			memoryBlock.reset();
		}
	}
	return memoryBlock;
}

MemoryBlock::MemoryBlock(Device& device, VkMemoryRequirements memoryRequirements, VkMemoryPropertyFlags memoryProperties)
	: mDevice(device)
	, mMemoryBlock(VK_NULL_HANDLE)
	, mMemoryRequirements(memoryRequirements)
	, mMemoryProperties(memoryProperties)
	, mMappedData(nullptr)
	, mMappedOffset(0)
	, mMappedSize(0)
{
	ObjectTracker::registerObject(ObjectType_MemoryBlock);
}

bool MemoryBlock::init()
{
	const VkPhysicalDeviceMemoryProperties& deviceMemoryProperties = mDevice.getPhysicalDevice().getMemoryProperties();
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

			VkResult result = vkAllocateMemory(mDevice.getHandle(), &bufferMemoryAllocateInfo, nullptr, &mMemoryBlock);
			if (result == VK_SUCCESS) 
			{
				// Store the properties
				mMemoryRequirements.memoryTypeBits = type;
				mMemoryProperties = deviceMemoryPropertiesFlags;
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

	return true;
}

void MemoryBlock::release()
{
	if (mMemoryBlock != VK_NULL_HANDLE)
	{
		vkFreeMemory(mDevice.getHandle(), mMemoryBlock, nullptr);
		mMemoryBlock = VK_NULL_HANDLE;
	}
}

} // namespace Vulkan
} // namespace nu