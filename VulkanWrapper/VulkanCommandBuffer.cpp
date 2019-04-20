#include "VulkanCommandBuffer.hpp"

#include "VulkanBuffer.hpp"
#include "VulkanCommandPool.hpp"
#include "VulkanDescriptorSet.hpp"
#include "VulkanImage.hpp"
#include "VulkanGraphicsPipeline.hpp"
#include "VulkanComputePipeline.hpp"
#include "VulkanDevice.hpp"

VULKAN_NAMESPACE_BEGIN

VulkanCommandBuffer::~VulkanCommandBuffer()
{
	release();
	
	VULKAN_OBJECTTRACKER_UNREGISTER();
}

bool VulkanCommandBuffer::beginRecording(VkCommandBufferUsageFlags usage, VkCommandBufferInheritanceInfo* secondaryCommandBufferInfo)
{
	// TODO : Check parameters ?
	
	// TODO : Do something if already recording ?
	
	VkCommandBufferBeginInfo commandBufferBeginInfo = {
		VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO,    // VkStructureType                        sType
		nullptr,                                        // const void                           * pNext
		usage,                                          // VkCommandBufferUsageFlags              flags
		secondaryCommandBufferInfo                      // const VkCommandBufferInheritanceInfo * pInheritanceInfo
	};

	VkResult result = vkBeginCommandBuffer(mCommandBuffer, &commandBufferBeginInfo);
	if (result != VK_SUCCESS)
	{
		// TODO : Log more
		VULKAN_LOG_ERROR("Could not begin command buffer recording operation");
		return false;
	}

	mRecording = true;
	return true;
}

bool VulkanCommandBuffer::endRecording()
{
	// TODO : Do something if not recording ?
	
	VkResult result = vkEndCommandBuffer(mCommandBuffer);
	if (VK_SUCCESS != result)
	{
		// TODO : Log more
		VULKAN_LOG_ERROR("Error occurred during command buffer recording");
		return false;
	}

	mRecording = false;
	return true;
}

void VulkanCommandBuffer::beginRenderPass(VkRenderPass renderPass, VkFramebuffer framebuffer, VkRect2D renderArea, const std::vector<VkClearValue>& clearValues, VkSubpassContents subpassContents)
{
	// TODO : Check parameters ?
	
	VkRenderPassBeginInfo renderPassBeginInfo = {
		VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO,     // VkStructureType        sType
		nullptr,                                      // const void           * pNext
		renderPass,                                   // VkRenderPass           renderPass
		framebuffer,                                  // VkFramebuffer          framebuffer
		renderArea,                                   // VkRect2D               renderArea
		static_cast<VulkanU32>(clearValues.size()),    // uint32_t               clearValueCount
		clearValues.data()                            // const VkClearValue   * pClearValues
	};

	vkCmdBeginRenderPass(mCommandBuffer, &renderPassBeginInfo, subpassContents);
}

void VulkanCommandBuffer::progressToTheNextSubpass(VkSubpassContents subpassContents)
{
	// TODO : Check parameters ?
	vkCmdNextSubpass(mCommandBuffer, subpassContents);
}

void VulkanCommandBuffer::endRenderPass()
{
	// TODO : Check parameters ?
	vkCmdEndRenderPass(mCommandBuffer);
}

void VulkanCommandBuffer::clearColorImage(VkImage image, VkImageLayout imageLayout, const std::vector<VkImageSubresourceRange>& imageSubresourceRanges, VkClearColorValue& clearColor)
{
	// TODO : Check parameters ?
	vkCmdClearColorImage(mCommandBuffer, image, imageLayout, &clearColor, static_cast<VulkanU32>(imageSubresourceRanges.size()), imageSubresourceRanges.data());
}

void VulkanCommandBuffer::clearDepthStencilImage(VkImage image, VkImageLayout imageLayout, const std::vector<VkImageSubresourceRange>& imageSubresourceRanges, VkClearDepthStencilValue& clearValue)
{
	// TODO : Check parameters ?
	vkCmdClearDepthStencilImage(mCommandBuffer, image, imageLayout, &clearValue, static_cast<VulkanU32>(imageSubresourceRanges.size()), imageSubresourceRanges.data());
}

void VulkanCommandBuffer::clearRenderPassAttachments(const std::vector<VkClearAttachment>& attachments, const std::vector<VkClearRect>& rects)
{
	// TODO : Check parameters ?
	vkCmdClearAttachments(mCommandBuffer, static_cast<VulkanU32>(attachments.size()), attachments.data(), static_cast<VulkanU32>(rects.size()), rects.data());
}

void VulkanCommandBuffer::bindVertexBuffers(VulkanU32 firstBinding, const std::vector<VulkanVertexBufferParameters>& buffersParameters)
{
	// TODO : Improve parameters checking ?
	if (buffersParameters.size() > 0)
	{
		std::vector<VkBuffer> buffers;
		std::vector<VkDeviceSize> offsets;
		buffers.reserve(buffersParameters.size());
		offsets.reserve(buffersParameters.size());
		for (auto& bufferParameters : buffersParameters)
		{
			buffers.push_back(bufferParameters.buffer->getHandle());
			offsets.push_back(bufferParameters.memoryOffset);
		}
		vkCmdBindVertexBuffers(mCommandBuffer, firstBinding, static_cast<VulkanU32>(buffersParameters.size()), buffers.data(), offsets.data());
	}
}

void VulkanCommandBuffer::bindIndexBuffer(VulkanBuffer* buffer, VkDeviceSize memoryOffset, VkIndexType indexType)
{
	// TODO : Check parameters ?
	vkCmdBindIndexBuffer(mCommandBuffer, buffer->getHandle(), memoryOffset, indexType);
}

void VulkanCommandBuffer::bindDescriptorSets(VkPipelineBindPoint pipelineType, VkPipelineLayout pipelineLayout, VulkanU32 indexForFirstSet, const std::vector<VulkanDescriptorSet*>& descriptorSets, const std::vector<VulkanU32>& dynamicOffsets)
{
	// TODO : Improve parameters checking ?
	if (descriptorSets.size() > 0)
	{
		std::vector<VkDescriptorSet> descriptorSetHandles;
		descriptorSetHandles.reserve(descriptorSets.size());
		for (auto& descriptorSet : descriptorSets)
		{
			descriptorSetHandles.push_back(descriptorSet->getHandle());
		}

		vkCmdBindDescriptorSets(mCommandBuffer, pipelineType, pipelineLayout, indexForFirstSet, static_cast<VulkanU32>(descriptorSetHandles.size()), descriptorSetHandles.data(), static_cast<VulkanU32>(dynamicOffsets.size()), dynamicOffsets.data());
	}
}

void VulkanCommandBuffer::bindPipeline(VulkanGraphicsPipeline* pipeline)
{
	// TODO : Improve parameters checking ?
	if (pipeline != nullptr && pipeline->isCreated())
	{
		vkCmdBindPipeline(mCommandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, pipeline->getHandle());
	}
	else if (pipeline == nullptr)
	{
		// TODO : Log more
		VULKAN_LOG_ERROR("Null pipeline given to command buffer");
	}
	else
	{
		// TODO : Log more
		VULKAN_LOG_ERROR("Non-created pipeline given to command buffer");
	}
}

void VulkanCommandBuffer::bindPipeline(VulkanComputePipeline* pipeline)
{
	// TODO : Check parameters ?
	vkCmdBindPipeline(mCommandBuffer, VK_PIPELINE_BIND_POINT_COMPUTE, pipeline->getHandle());
}

void VulkanCommandBuffer::provideDataToShadersThroughPushConstants(VkPipelineLayout pipelineLayout, VkShaderStageFlags pipelineStages, VulkanU32 offset, VulkanU32 size, void* data)
{
	// TODO : Check parameters ?
	vkCmdPushConstants(mCommandBuffer, pipelineLayout, pipelineStages, offset, size, data);
}

void VulkanCommandBuffer::setViewportStateDynamically(VulkanU32 firstViewport, const std::vector<VkViewport>& viewports)
{
	// TODO : Check parameters ?
	vkCmdSetViewport(mCommandBuffer, firstViewport, static_cast<VulkanU32>(viewports.size()), viewports.data());
}

void VulkanCommandBuffer::setScissorStateDynamically(VulkanU32 firstScissor, const std::vector<VkRect2D>& scissors)
{
	// TODO : Check parameters ?
	vkCmdSetScissor(mCommandBuffer, firstScissor, static_cast<VulkanU32>(scissors.size()), scissors.data());
}

void VulkanCommandBuffer::setLineWidthStateDynamically(float lineWidth)
{
	// TODO : Check parameters ?
	vkCmdSetLineWidth(mCommandBuffer, lineWidth);
}

void VulkanCommandBuffer::setDepthBiasStateDynamically(float constantFactor, float clampValue, float slopeFactor)
{
	// TODO : Check parameters ?
	vkCmdSetDepthBias(mCommandBuffer, constantFactor, clampValue, slopeFactor);
}

void VulkanCommandBuffer::setBlendConstantsStateDynamically(const float (&blendConstants)[4])
{
	// TODO : Check parameters ?
	vkCmdSetBlendConstants(mCommandBuffer, blendConstants);
}

void VulkanCommandBuffer::drawGeometry(VulkanU32 vertexCount, VulkanU32 instanceCount, VulkanU32 firstVertex,VulkanU32 firstInstance)
{
	// TODO : Check parameters ?
	vkCmdDraw(mCommandBuffer, vertexCount, instanceCount, firstVertex, firstInstance);
}

void VulkanCommandBuffer::drawIndexedGeometry(VulkanU32 indexCount, VulkanU32 instanceCount, VulkanU32 firstIndex, VulkanU32 vertexOffset, VulkanU32 firstInstance)
{
	// TODO : Check parameters ?
	vkCmdDrawIndexed(mCommandBuffer, indexCount, instanceCount, firstIndex, vertexOffset, firstInstance);
}

void VulkanCommandBuffer::dispatchComputeWork(VulkanU32 xSize, VulkanU32 ySize, VulkanU32 zSize)
{
	// TODO : Check parameters ?
	vkCmdDispatch(mCommandBuffer, xSize, ySize, zSize);
}

void VulkanCommandBuffer::executeSecondaryCommandBuffer(const std::vector<VulkanCommandBuffer*>& secondaryCommandBuffers)
{
	// TODO : Improve parameters checking ?
	if (secondaryCommandBuffers.size() > 0)
	{
		std::vector<VkCommandBuffer> commandBuffers;
		commandBuffers.reserve(secondaryCommandBuffers.size());
		for (auto& commandBuffer : secondaryCommandBuffers)
		{
			commandBuffers.push_back(commandBuffer->getHandle());
		}

		vkCmdExecuteCommands(mCommandBuffer, static_cast<VulkanU32>(commandBuffers.size()), commandBuffers.data());
	}
}

void VulkanCommandBuffer::copyDataBetweenBuffers(VulkanBuffer* sourceBuffer, VulkanBuffer* destinationBuffer, std::vector<VkBufferCopy> regions)
{
	// TODO : Improve parameters checking ?
	if (regions.size() > 0)
	{
		vkCmdCopyBuffer(mCommandBuffer, sourceBuffer->getHandle(), destinationBuffer->getHandle(), static_cast<VulkanU32>(regions.size()), regions.data());
	}
}

void VulkanCommandBuffer::copyDataFromBufferToImage(VulkanBuffer* sourceBuffer, VulkanImage* destinationImage, VkImageLayout imageLayout, std::vector<VkBufferImageCopy> regions)
{
	// TODO : Improve parameters checking ?
	if (regions.size() > 0)
	{
		vkCmdCopyBufferToImage(mCommandBuffer, sourceBuffer->getHandle(), destinationImage->getHandle(), imageLayout, static_cast<VulkanU32>(regions.size()), regions.data());
	}
}

void VulkanCommandBuffer::copyDataFromImageToBuffer(VulkanImage* sourceImage, VkImageLayout imageLayout, VulkanBuffer* destinationBuffer, std::vector<VkBufferImageCopy> regions)
{
	// TODO : Improve parameters checking ?
	if (regions.size() > 0)
	{
		vkCmdCopyImageToBuffer(mCommandBuffer, sourceImage->getHandle(), imageLayout, destinationBuffer->getHandle(), static_cast<VulkanU32>(regions.size()), regions.data());
	}
}

void VulkanCommandBuffer::setBufferMemoryBarrier(VkPipelineStageFlags generatingStages, VkPipelineStageFlags consumingStages, const std::vector<VulkanBufferTransition>& bufferTransitions)
{
	// TODO : Improve parameters checking ?
	std::vector<VkBufferMemoryBarrier> bufferMemoryBarriers;
	bufferMemoryBarriers.reserve(bufferTransitions.size());
	for (auto& bufferTransition : bufferTransitions) 
	{
		bufferMemoryBarriers.push_back({
			VK_STRUCTURE_TYPE_BUFFER_MEMORY_BARRIER,    // VkStructureType    sType
			nullptr,                                    // const void       * pNext
			bufferTransition.currentAccess,             // VkAccessFlags      srcAccessMask
			bufferTransition.newAccess,                 // VkAccessFlags      dstAccessMask
			bufferTransition.currentQueueFamily,        // uint32_t           srcQueueFamilyIndex
			bufferTransition.newQueueFamily,            // uint32_t           dstQueueFamilyIndex
			bufferTransition.buffer->getHandle(),       // VkBuffer           buffer
			0,                                          // VkDeviceSize       offset
			VK_WHOLE_SIZE                               // VkDeviceSize       size
		});
		// TODO : Use offset and size ?
	}

	if (bufferMemoryBarriers.size() > 0) 
	{
		// TODO : What are those 0 parameters ?
		vkCmdPipelineBarrier(mCommandBuffer, generatingStages, consumingStages, 0, 0, nullptr, static_cast<VulkanU32>(bufferMemoryBarriers.size()), bufferMemoryBarriers.data(), 0, nullptr);
	}
}

void VulkanCommandBuffer::setImageMemoryBarrier(VkPipelineStageFlags generatingStages, VkPipelineStageFlags consumingStages, const std::vector<VulkanImageTransition>& imageTransitions)
{
	std::vector<VkImageMemoryBarrier> imageMemoryBarriers;
	imageMemoryBarriers.reserve(imageTransitions.size());
	for (auto& imageTransition : imageTransitions) 
	{
		imageMemoryBarriers.push_back({
			VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER,   // VkStructureType            sType
			nullptr,                                  // const void               * pNext
			imageTransition.currentAccess,            // VkAccessFlags              srcAccessMask
			imageTransition.newAccess,                // VkAccessFlags              dstAccessMask
			imageTransition.currentLayout,            // VkImageLayout              oldLayout
			imageTransition.newLayout,                // VkImageLayout              newLayout
			imageTransition.currentQueueFamily,       // uint32_t                   srcQueueFamilyIndex
			imageTransition.newQueueFamily,           // uint32_t                   dstQueueFamilyIndex
			imageTransition.image,                    // VkImage                    image
			{                                         // VkImageSubresourceRange    subresourceRange
				imageTransition.aspect,                   // VkImageAspectFlags         aspectMask
				0,                                        // uint32_t                   baseMipLevel
				VK_REMAINING_MIP_LEVELS,                  // uint32_t                   levelCount
				0,                                        // uint32_t                   baseArrayLayer
				VK_REMAINING_ARRAY_LAYERS                 // uint32_t                   layerCount
			}
			});
			// TODO : Use those 0 parameters ?
	}

	if (imageMemoryBarriers.size() > 0) 
	{
		// TODO : What are those 0 parameters ?
		vkCmdPipelineBarrier(mCommandBuffer, generatingStages, consumingStages, 0, 0, nullptr, 0, nullptr, static_cast<VulkanU32>(imageMemoryBarriers.size()), imageMemoryBarriers.data());
	}
}

bool VulkanCommandBuffer::reset()
{
	// TODO : Release or not ?
	//VkResult result = vkResetCommandBuffer(command_buffer, release_resources ? VK_COMMAND_BUFFER_RESET_RELEASE_RESOURCES_BIT : 0);
	VkResult result = vkResetCommandBuffer(mCommandBuffer, 0);
	if (VK_SUCCESS != result)
	{
		// TODO : Log more
		VULKAN_LOG_ERROR("Error occurred during command buffer reset\n");
		return false;
	}
	return true;
}

bool VulkanCommandBuffer::isRecording() const
{
	return mRecording;
}

const VulkanCommandPool& VulkanCommandBuffer::getCommandPool() const
{
	return mCommandPool;
}

const VulkanCommandBufferType& VulkanCommandBuffer::getType() const
{
	return mType;
}

bool VulkanCommandBuffer::isInitialized() const
{
	return mCommandBuffer != VK_NULL_HANDLE;
}

const VkCommandBuffer& VulkanCommandBuffer::getHandle() const
{
	return mCommandBuffer;
}

const VkCommandPool& VulkanCommandBuffer::getCommandPoolHandle() const
{
	return mCommandPool.getHandle();
}

VulkanCommandBuffer::VulkanCommandBuffer(VulkanCommandPool& commandPool, VulkanCommandBufferType type)
	: mCommandBuffer(VK_NULL_HANDLE)
	, mCommandPool(commandPool)
	, mType(type)
	, mRecording(false)
{
	VULKAN_OBJECTTRACKER_REGISTER();
}

bool VulkanCommandBuffer::init()
{
	VkCommandBufferAllocateInfo commandBufferAllocateInfo = {
		VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO,   // VkStructureType          sType
		nullptr,                                          // const void             * pNext
		mCommandPool.getHandle(),                         // VkCommandPool            commandPool
		(VkCommandBufferLevel)mType,                      // VkCommandBufferLevel     level
		1                                                 // uint32_t                 commandBufferCount
	};
	// TODO : Init more than one a once ?

	VkResult result = vkAllocateCommandBuffers(mCommandPool.getDeviceHandle(), &commandBufferAllocateInfo, &mCommandBuffer);
	if (result != VK_SUCCESS || mCommandBuffer == VK_NULL_HANDLE)
	{
		VULKAN_LOG_ERROR("Could not allocate command buffers\n");
		return false;
	}
	return true;
}

bool VulkanCommandBuffer::release()
{
	if (mCommandBuffer != VK_NULL_HANDLE)
	{
		vkFreeCommandBuffers(mCommandPool.getDeviceHandle(), mCommandPool.getHandle(), 1, &mCommandBuffer);
		mCommandBuffer = VK_NULL_HANDLE;
		return true;
	}
	return false;
}

VULKAN_NAMESPACE_END