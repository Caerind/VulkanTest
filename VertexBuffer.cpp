#include "VertexBuffer.hpp"

namespace nu
{

VertexBuffer::Ptr VertexBuffer::createVertexBuffer(Vulkan::Device& device, uint32_t size)
{
	VertexBuffer::Ptr vertexBuffer = VertexBuffer::Ptr(new VertexBuffer());
	if (vertexBuffer != nullptr)
	{
		if (!vertexBuffer->init(device, size))
		{
			vertexBuffer.reset();
		}
	}
	return vertexBuffer;
}

VertexBuffer::~VertexBuffer()
{
}

bool VertexBuffer::updateAndWait(uint32_t size, void* data, uint32_t offset, VkAccessFlags currentAccess, VkAccessFlags newAccess, VkPipelineStageFlags generatingStages, VkPipelineStageFlags consumingStages, Vulkan::CommandBuffer* commandBuffer, Vulkan::Queue* queue, std::vector<VkSemaphore> signalSemaphores, uint64_t timeout)
{
	return queue->useStagingBufferToUpdateBufferAndWait(size, data, mBuffer.get(), offset, currentAccess, newAccess, generatingStages, consumingStages, commandBuffer, signalSemaphores, timeout);
}

void VertexBuffer::bindTo(Vulkan::CommandBuffer* commandBuffer, uint32_t firstBinding, uint32_t memoryOffset)
{
	commandBuffer->bindVertexBuffers(0, { { mBuffer.get(), memoryOffset } });
}

VertexBuffer::VertexBuffer()
	: mBuffer(nullptr)
{
}

bool VertexBuffer::init(Vulkan::Device& device, uint32_t size)
{
	mBuffer = device.createBuffer(size, VK_BUFFER_USAGE_VERTEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT);
	if (!mBuffer || !mBuffer->allocateMemoryBlock(VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT))
	{
		return false;
	}
	return true;
}

} // namespace nu