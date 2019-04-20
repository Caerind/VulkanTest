#pragma once

#include "VulkanFunctions.hpp"

VULKAN_NAMESPACE_BEGIN

class VulkanBufferView : public VulkanObject<VulkanObjectType_BufferView>
{
	public:
		~VulkanBufferView();

		const VkFormat& getFormat() const;
		const VkDeviceSize& getOffset() const;
		const VkDeviceSize& getSize() const;

		VulkanBuffer& getBuffer();
		const VulkanBuffer& getBuffer() const;
		const VkBufferView& getHandle() const;
		const VkBuffer& getBufferHandle() const;

	private:
		friend class VulkanBuffer;
		static VulkanBufferViewPtr createBufferView(VulkanBuffer& buffer, VkFormat format, VkDeviceSize offset, VkDeviceSize size);

		VulkanBufferView(VulkanBuffer& buffer, VkFormat format, VkDeviceSize offset, VkDeviceSize size);

		bool init();
		void release();

	private:
		VulkanBuffer& mBuffer;
		VkBufferView mBufferView;

		VkFormat mFormat;
		VkDeviceSize mOffset;
		VkDeviceSize mSize;
};

VULKAN_NAMESPACE_END