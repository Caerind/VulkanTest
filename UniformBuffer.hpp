#pragma once

#include <memory>

#include "VulkanWrapper/VulkanDevice.hpp"
#include "VulkanWrapper/VulkanBuffer.hpp"
#include "VulkanWrapper/VulkanMemoryBlock.hpp"
#include "VulkanWrapper/VulkanBufferView.hpp"
#include "VulkanWrapper/VulkanDescriptorSet.hpp"

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

		static UniformBuffer::Ptr createUniformBuffer(VulkanDevice& device, uint32_t size);
		~UniformBuffer();

		StagingBuffer::Ptr createStagingBuffer();

		void updateDescriptor(VulkanDescriptorSet* dstSet, uint32_t dstBinding, uint32_t dstArrayElement);

	private:
		friend class StagingBuffer;

		UniformBuffer();

		bool init(VulkanDevice& device, uint32_t size);

		VulkanBufferPtr mBuffer;
};

} // namespace nu