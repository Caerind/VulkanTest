#ifndef NU_STAGING_BUFFER_HPP
#define NU_STAGING_BUFFER_HPP

#include "VulkanWrapper/VulkanDevice.hpp"
#include "VulkanWrapper/VulkanBuffer.hpp"
#include "VulkanWrapper/VulkanMemoryBlock.hpp"
#include "VulkanWrapper/VulkanBufferView.hpp"

#include <memory>

namespace nu
{

class StagingBuffer
{
	public:
		typedef std::unique_ptr<StagingBuffer> Ptr;

		static StagingBuffer::Ptr createStagingBuffer(Vulkan::Device& device);
		~StagingBuffer();

	private:
		StagingBuffer();

		Vulkan::Buffer::Ptr mBuffer;
};

} // namespace nu

#endif // NU_STAGING_BUFFER_HPP