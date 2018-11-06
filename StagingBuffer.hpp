#ifndef NU_STAGING_BUFFER_HPP
#define NU_STAGING_BUFFER_HPP

#include "VulkanWrapper/VulkanDevice.hpp"
#include "VulkanWrapper/VulkanBuffer.hpp"
#include "VulkanWrapper/VulkanMemoryBlock.hpp"
#include "VulkanWrapper/VulkanBufferView.hpp"

#include <memory>

namespace nu
{

class UniformBuffer;
class StagingBuffer
{
	public:
		typedef std::unique_ptr<StagingBuffer> Ptr;

		static StagingBuffer::Ptr createStagingBuffer(Vulkan::Buffer& dstBuffer);

		~StagingBuffer();

		bool map(uint32_t offset, uint32_t size);
		bool read(void* data);
		bool write(const void* data);
		bool unmap();

		bool mapWriteUnmap(uint32_t offset, uint32_t dataSize, void* data, bool unmap = true, void** pointer = nullptr);

		void send(Vulkan::CommandBuffer* commandBuffer);

		bool needToSend() const;

	private:
		StagingBuffer(Vulkan::Buffer& dstBuffer);

		bool init();

		Vulkan::Buffer& mDstBuffer;
		Vulkan::Buffer::Ptr mBuffer;
		bool mNeedToSend;
};

} // namespace nu

#endif // NU_STAGING_BUFFER_HPP