#pragma once

#include "VulkanWrapper/VulkanDevice.hpp"
#include "VulkanWrapper/VulkanBuffer.hpp"
#include "VulkanWrapper/VulkanCommandBuffer.hpp"

#include <memory>

namespace nu
{

class VertexBuffer
{
	public:
		typedef std::unique_ptr<VertexBuffer> Ptr;

		static VertexBuffer::Ptr createVertexBuffer(VulkanDevice& device, uint32_t size);
		~VertexBuffer();

		bool updateAndWait(uint32_t size, void* data, uint32_t offset, VkAccessFlags currentAccess, VkAccessFlags newAccess, VkPipelineStageFlags generatingStages, VkPipelineStageFlags consumingStages, VulkanCommandBuffer* commandBuffer, VulkanQueue* queue, std::vector<VkSemaphore> signalSemaphores, uint64_t timeout);
		
		void bindTo(VulkanCommandBuffer* commandBuffer, uint32_t firstBinding, uint32_t memoryOffset);

	private:
		VertexBuffer();

		bool init(VulkanDevice& device, uint32_t size);

		VulkanBufferPtr mBuffer;
};

} // namespace nu