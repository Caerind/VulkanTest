#ifndef NU_VERTEX_BUFFER_HPP
#define NU_VERTEX_BUFFER_HPP

#include "VulkanWrapper/VulkanDevice.hpp"
#include "VulkanWrapper/VulkanBuffer.hpp"

#include <memory>

namespace nu
{

class VertexBuffer
{
	public:
		typedef std::unique_ptr<VertexBuffer> Ptr;

		static VertexBuffer::Ptr createVertexBuffer(Vulkan::Device& device, uint32_t size);
		~VertexBuffer();

		bool updateAndWait(uint32_t size, void* data, uint32_t offset, VkAccessFlags currentAccess, VkAccessFlags newAccess, VkPipelineStageFlags generatingStages, VkPipelineStageFlags consumingStages, Vulkan::CommandBuffer* commandBuffer, Vulkan::Queue* queue, std::vector<VkSemaphore> signalSemaphores, uint64_t timeout);
		
		void bindTo(Vulkan::CommandBuffer* commandBuffer, uint32_t firstBinding, uint32_t memoryOffset);

	private:
		VertexBuffer();

		bool init(Vulkan::Device& device, uint32_t size);

		Vulkan::Buffer::Ptr mBuffer;
};

} // namespace nu

#endif // NU_VERTEX_BUFFER_HPP