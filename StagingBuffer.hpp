#pragma once

#include "VulkanWrapper/VulkanDevice.hpp"
#include "VulkanWrapper/VulkanBuffer.hpp"
#include "VulkanWrapper/VulkanMemoryBlock.hpp"
#include "VulkanWrapper/VulkanBufferView.hpp"
#include "VulkanWrapper/VulkanCommandBuffer.hpp"

#include <memory>

namespace nu
{

class UniformBuffer;
class StagingBuffer
{
	public:
		typedef std::unique_ptr<StagingBuffer> Ptr;

		static StagingBuffer::Ptr createStagingBuffer(VulkanBuffer& dstBuffer);

		~StagingBuffer();

		bool map(uint32_t offset, uint32_t size);
		bool read(void* data);
		bool write(const void* data);
		bool unmap();

		bool mapWriteUnmap(uint32_t offset, uint32_t dataSize, void* data, bool unmap = true, void** pointer = nullptr);

		void send(VulkanCommandBuffer* commandBuffer);

		bool needToSend() const;

	private:
		StagingBuffer(VulkanBuffer& dstBuffer);

		bool init();

		VulkanBuffer& mDstBuffer;
		VulkanBufferPtr mBuffer;
		bool mNeedToSend;
};

} // namespace nu