#include "UniformBuffer.hpp"

namespace nu
{

UniformBuffer::Ptr UniformBuffer::createUniformBuffer(VulkanDevice& device, uint32_t size)
{
	UniformBuffer::Ptr uniformBuffer = UniformBuffer::Ptr(new UniformBuffer());
	if (uniformBuffer != nullptr)
	{
		if (!uniformBuffer->init(device, size))
		{
			uniformBuffer.reset();
		}
	}
	return uniformBuffer;
}

UniformBuffer::~UniformBuffer()
{
}

StagingBuffer::Ptr UniformBuffer::createStagingBuffer()
{
	return StagingBuffer::createStagingBuffer(*mBuffer);
}

void UniformBuffer::updateDescriptor(VulkanDescriptorSet* dstSet, uint32_t dstBinding, uint32_t dstArrayElement)
{
	assert(dstSet != nullptr);

	VulkanBufferDescriptorInfo bufferDescriptorUpdate = {
		dstSet->getHandle(),                        // VkDescriptorSet                      TargetDescriptorSet
		dstBinding,                                 // uint32_t                             TargetDescriptorBinding
		dstArrayElement,                            // uint32_t                             TargetArrayElement
		VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,          // VkDescriptorType                     TargetDescriptorType
		{                                           // std::vector<VkDescriptorBufferInfo>  BufferInfos
			{
				mBuffer->getHandle(),                     // VkBuffer                             buffer
				0,                                        // VkDeviceSize                         offset
				VK_WHOLE_SIZE                             // VkDeviceSize                         range
			}
		}
	};
	// TODO : offset / range

	mBuffer->getDevice().updateDescriptorSets({}, { bufferDescriptorUpdate }, {}, {});
}

UniformBuffer::UniformBuffer()
	: mBuffer(nullptr)
{
}

bool UniformBuffer::init(VulkanDevice& device, uint32_t size)
{
	mBuffer = device.createBuffer(size, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT);
	if (!mBuffer || !mBuffer->allocateMemoryBlock(VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT))
	{
		return false;
	}
	return true;
}

} // namespace nu