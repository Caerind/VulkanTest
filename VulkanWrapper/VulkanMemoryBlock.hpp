#pragma once

#include "VulkanFunctions.hpp"

// TODO : Can multiple memory range from the same memory block be mapped at the same time ? -> If yes, instead of the variables, use a vector. It will be easier to flush everything at the same time
// TODO : Control over flushing
// TODO : Read/Write only a subpart of mapped memory
// TODO : Binding on MemoryBlock side or Image/Buffer side ?

VULKAN_NAMESPACE_BEGIN

class VulkanMemoryBlock : public VulkanDeviceObject<VulkanObjectType_MemoryBlock>
{
	public:
		~VulkanMemoryBlock();

		bool map(VkDeviceSize offset, VkDeviceSize size);
		bool read(void* data);
		bool write(const void* data);
		bool unmap();

		bool bind(VulkanBuffer* buffer, VkDeviceSize memoryOffset);
		bool bind(VulkanImage* image, VkDeviceSize memoryOffset);

		bool isHostVisible() const;
		bool isDeviceLocal() const;
		bool isHostCoherent() const;

		VkDeviceSize getSize() const;
		VkDeviceSize getAlignment() const;
		uint32_t getMemoryType() const;
		VkMemoryPropertyFlags getMemoryProperties() const;

		bool isMapped() const;
		VkDeviceSize getMappedOffset() const;
		VkDeviceSize getMappedSize() const;

		const VkDeviceMemory& getHandle() const;

	private:
		friend class VulkanDevice;
		static VulkanMemoryBlockPtr createMemoryBlock(VkMemoryRequirements memoryRequirements, VkMemoryPropertyFlags memoryProperties);

		VulkanMemoryBlock(VkMemoryRequirements memoryRequirements, VkMemoryPropertyFlags memoryProperties);

		bool init();
		void release();

	private:
		VkDeviceMemory mMemoryBlock;
		VkMemoryRequirements mMemoryRequirements;
		VkMemoryPropertyFlags mMemoryProperties;

		void* mMappedData;
		VkDeviceSize mMappedOffset;
		VkDeviceSize mMappedSize;
};

VULKAN_NAMESPACE_END