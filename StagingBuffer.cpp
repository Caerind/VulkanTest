#include "StagingBuffer.hpp"

#include "UniformBuffer.hpp"

namespace nu
{

StagingBuffer::Ptr StagingBuffer::createStagingBuffer(Vulkan::Buffer& dstBuffer)
{
	StagingBuffer::Ptr stagingBuffer = StagingBuffer::Ptr(new StagingBuffer(dstBuffer));
	if (stagingBuffer != nullptr)
	{
		if (!stagingBuffer->init())
		{
			stagingBuffer.reset();
		}
	}
	return stagingBuffer;
}

StagingBuffer::~StagingBuffer()
{
}

bool StagingBuffer::map(uint32_t offset, uint32_t size)
{
	assert(mBuffer->isBoundToMemoryBlock());
	return mBuffer->getMemoryBlock()->map(offset, size);
}

bool StagingBuffer::read(void* data)
{
	assert(mBuffer->isBoundToMemoryBlock());
	return mBuffer->getMemoryBlock()->read(data);
}

bool StagingBuffer::write(const void* data)
{
	assert(mBuffer->isBoundToMemoryBlock());
	return mBuffer->getMemoryBlock()->write(data);
}

bool StagingBuffer::unmap()
{
	assert(mBuffer->isBoundToMemoryBlock());
	return mBuffer->getMemoryBlock()->unmap();
}

bool StagingBuffer::mapWriteUnmap(uint32_t offset, uint32_t dataSize, void* data, bool unmap, void** readPointer)
{
	assert(mBuffer->isBoundToMemoryBlock());

	Vulkan::MemoryBlock* memoryBlock = mBuffer->getMemoryBlock();
	if (!memoryBlock->map(offset, dataSize))
	{
		return false;
	}
	if (readPointer != nullptr)
	{
		if (!memoryBlock->read(&readPointer))
		{
			return false;
		}
	}
	if (!memoryBlock->write(data))
	{
		return false;
	}
	if (unmap)
	{
		memoryBlock->unmap();
	}

	mNeedToSend = true;

	return true;
}

void StagingBuffer::send(Vulkan::CommandBuffer* commandBuffer)
{
	mNeedToSend = false;

	nu::Vulkan::BufferTransition preTransferTransition = {
		&mDstBuffer,                  // Buffer*          buffer
		VK_ACCESS_UNIFORM_READ_BIT,   // VkAccessFlags    currentAccess
		VK_ACCESS_TRANSFER_WRITE_BIT, // VkAccessFlags    newAccess
		VK_QUEUE_FAMILY_IGNORED,      // uint32_t         currentQueueFamily
		VK_QUEUE_FAMILY_IGNORED       // uint32_t         newQueueFamily
	};
	commandBuffer->setBufferMemoryBarrier(VK_PIPELINE_STAGE_VERTEX_SHADER_BIT, VK_PIPELINE_STAGE_TRANSFER_BIT, { preTransferTransition });

	std::vector<VkBufferCopy> regions = {
		{
			0,                        // VkDeviceSize     srcOffset
			0,                        // VkDeviceSize     dstOffset
			mBuffer->getSize()        // VkDeviceSize     size
		}
	};
	// TODO : Use srcOffset & dstOffset
	commandBuffer->copyDataBetweenBuffers(mBuffer.get(), &mDstBuffer, regions);

	nu::Vulkan::BufferTransition postTransferTransition = {
		&mDstBuffer,                  // Buffer*          buffer
		VK_ACCESS_TRANSFER_WRITE_BIT, // VkAccessFlags    currentAccess
		VK_ACCESS_UNIFORM_READ_BIT,   // VkAccessFlags    newAccess
		VK_QUEUE_FAMILY_IGNORED,      // uint32_t         currentQueueFamily
		VK_QUEUE_FAMILY_IGNORED       // uint32_t         newQueueFamily
	};
	commandBuffer->setBufferMemoryBarrier(VK_PIPELINE_STAGE_TRANSFER_BIT, VK_PIPELINE_STAGE_VERTEX_SHADER_BIT, { postTransferTransition });
}

bool StagingBuffer::needToSend() const
{
	return mNeedToSend;
}

StagingBuffer::StagingBuffer(Vulkan::Buffer& dstBuffer)
	: mDstBuffer(dstBuffer)
	, mBuffer(nullptr)
	, mNeedToSend(false)
{
}

bool StagingBuffer::init()
{
	mBuffer = mDstBuffer.getDevice().createBuffer(mDstBuffer.getSize(), VK_BUFFER_USAGE_TRANSFER_SRC_BIT);
	if (!mBuffer || !mBuffer->allocateMemoryBlock(VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT))
	{
		return false;
	}
	return true;
}

} // namespace nu