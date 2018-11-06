#ifndef NU_UNIFORM_BUFFER_HPP
#define NU_UNIFORM_BUFFER_HPP

#include <memory>

#include "VulkanWrapper/VulkanDevice.hpp"
#include "VulkanWrapper/VulkanBuffer.hpp"
#include "VulkanWrapper/VulkanMemoryBlock.hpp"
#include "VulkanWrapper/VulkanBufferView.hpp"

#include "StagingBuffer.hpp"

// TODO : Improve update descriptor
// TODO : Update more than one descriptor at once
// TODO : Memory Block
// TODO : Offset/Size/Range of Buffer
// TODO : Add getters to some of the buffer's data ?

namespace nu
{

class UniformBuffer
{
	public:
		typedef std::unique_ptr<UniformBuffer> Ptr;

		static UniformBuffer::Ptr createUniformBuffer(Vulkan::Device& device, uint32_t size);
		~UniformBuffer();

		StagingBuffer::Ptr createStagingBuffer();

		void updateDescriptor(Vulkan::DescriptorSet* dstSet, uint32_t dstBinding, uint32_t dstArrayElement);

	private:
		friend class StagingBuffer;

		UniformBuffer();

		bool init(Vulkan::Device& device, uint32_t size);

		Vulkan::Buffer::Ptr mBuffer;
};

} // namespace nu

#endif // NU_UNIFORM_BUFFER_HPP