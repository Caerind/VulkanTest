#pragma once

#include "VulkanFunctions.hpp"

// TODO : Binding on MemoryBlock side or Image/Buffer side ?

VULKAN_NAMESPACE_BEGIN

class VulkanBuffer : public VulkanDeviceObject<VulkanObjectType_Buffer>
{
	public:
		~VulkanBuffer();

		const VkMemoryRequirements& getMemoryRequirements() const;

        // TODO : Split each members of memoryProperties ?
		VulkanMemoryBlock* allocateMemoryBlock(VkMemoryPropertyFlagBits memoryProperties);
		bool ownMemoryBlock() const;
		bool isBoundToMemoryBlock() const;
		VulkanMemoryBlock* getMemoryBlock();
		VkDeviceSize getOffsetInMemoryBlock() const;

		VulkanBufferView* createBufferView(VkFormat format, VkDeviceSize memoryOffset, VkDeviceSize memoryRange);
		VulkanBufferView* getBufferView(VulkanU32 index);
		const VulkanBufferView* getBufferView(VulkanU32 index) const;
		VulkanU32 getBufferViewCount() const;
		void clearBufferViews();

		VkDeviceSize getSize() const;
		VkBufferUsageFlags getUsage() const;

		const VkBuffer& getHandle() const;
		
	private:
		friend class VulkanDevice;
		static VulkanBufferPtr createBuffer(VkDeviceSize size, VkBufferUsageFlags usage);

		VulkanBuffer(VkDeviceSize size, VkBufferUsageFlags usage);

		bool init();
		void release();

		friend class VulkanMemoryBlock;
		void bindToMemoryBlock(VulkanMemoryBlock* memoryBlock, VkDeviceSize offsetInMemoryBlock);

	private:
		VkBuffer mBuffer;

		VulkanMemoryBlockPtr mOwnedMemoryBlock;
		VulkanMemoryBlock* mMemoryBlock;
		VkDeviceSize mOffsetInMemoryBlock;

		std::vector<VulkanBufferViewPtr> mBufferViews;

		VkDeviceSize mSize;
		VkBufferUsageFlags mUsage;

		mutable bool mMemoryRequirementsQueried;
		mutable VkMemoryRequirements mMemoryRequirements;
};

VULKAN_NAMESPACE_END