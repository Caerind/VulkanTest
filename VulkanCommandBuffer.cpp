#include "VulkanCommandBuffer.hpp"

#include "VulkanCommandPool.hpp"

namespace nu
{
namespace Vulkan
{

CommandBuffer::~CommandBuffer()
{
	ObjectTracker::unregisterObject(ObjectType_CommandBuffer);

	release();
}

bool CommandBuffer::beginRecording(VkCommandBufferUsageFlags usage, VkCommandBufferInheritanceInfo* secondaryCommandBufferInfo)
{
	VkCommandBufferBeginInfo commandBufferBeginInfo = {
		VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO,    // VkStructureType                        sType
		nullptr,                                        // const void                           * pNext
		usage,                                          // VkCommandBufferUsageFlags              flags
		secondaryCommandBufferInfo                      // const VkCommandBufferInheritanceInfo * pInheritanceInfo
	};

	VkResult result = vkBeginCommandBuffer(mCommandBuffer, &commandBufferBeginInfo);
	if (result != VK_SUCCESS)
	{
		// TODO : Use Numea System Log
		printf("Could not begin command buffer recording operation\n");
		return false;
	}

	mRecording = true;
	return true;
}

bool CommandBuffer::endRecording()
{
	VkResult result = vkEndCommandBuffer(mCommandBuffer);
	if (VK_SUCCESS != result)
	{
		// TODO : Use Numea System Log
		printf("Error occurred during command buffer recording\n");
		return false;
	}

	mRecording = false;
	return true;
}

void CommandBuffer::beginRenderPass(VkRenderPass renderPass, VkFramebuffer framebuffer, VkRect2D renderArea, const std::vector<VkClearValue>& clearValues, VkSubpassContents subpassContents)
{
	VkRenderPassBeginInfo renderPassBeginInfo = {
		VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO,     // VkStructureType        sType
		nullptr,                                      // const void           * pNext
		renderPass,                                   // VkRenderPass           renderPass
		framebuffer,                                  // VkFramebuffer          framebuffer
		renderArea,                                   // VkRect2D               renderArea
		static_cast<uint32_t>(clearValues.size()),    // uint32_t               clearValueCount
		clearValues.data()                            // const VkClearValue   * pClearValues
	};

	vkCmdBeginRenderPass(mCommandBuffer, &renderPassBeginInfo, subpassContents);
}

void CommandBuffer::progressToTheNextSubpass(VkSubpassContents subpassContents)
{
	vkCmdNextSubpass(mCommandBuffer, subpassContents);
}

void CommandBuffer::endRenderPass()
{
	vkCmdEndRenderPass(mCommandBuffer);
}

void CommandBuffer::clearColorImage(VkImage image, VkImageLayout imageLayout, const std::vector<VkImageSubresourceRange>& imageSubresourceRanges, VkClearColorValue& clearColor)
{
	vkCmdClearColorImage(mCommandBuffer, image, imageLayout, &clearColor, static_cast<uint32_t>(imageSubresourceRanges.size()), imageSubresourceRanges.data());
}

void CommandBuffer::clearDepthStencilImage(VkImage image, VkImageLayout imageLayout, const std::vector<VkImageSubresourceRange>& imageSubresourceRanges, VkClearDepthStencilValue& clearValue)
{
	vkCmdClearDepthStencilImage(mCommandBuffer, image, imageLayout, &clearValue, static_cast<uint32_t>(imageSubresourceRanges.size()), imageSubresourceRanges.data());
}

void CommandBuffer::clearRenderPassAttachments(const std::vector<VkClearAttachment>& attachments, const std::vector<VkClearRect>& rects)
{
	vkCmdClearAttachments(mCommandBuffer, static_cast<uint32_t>(attachments.size()), attachments.data(), static_cast<uint32_t>(rects.size()), rects.data());
}

void CommandBuffer::bindVertexBuffers(uint32_t firstBinding, const std::vector<VertexBufferParameters>& buffersParameters)
{
	if (buffersParameters.size() > 0)
	{
		std::vector<VkBuffer> buffers;
		std::vector<VkDeviceSize> offsets;
		for (auto& bufferParameters : buffersParameters)
		{
			buffers.push_back(bufferParameters.buffer->getHandle());
			offsets.push_back(bufferParameters.memoryOffset);
		}
		vkCmdBindVertexBuffers(mCommandBuffer, firstBinding, static_cast<uint32_t>(buffersParameters.size()), buffers.data(), offsets.data());
	}
}

void CommandBuffer::bindIndexBuffer(Buffer* buffer, VkDeviceSize memoryOffset, VkIndexType indexType)
{
	vkCmdBindIndexBuffer(mCommandBuffer, buffer->getHandle(), memoryOffset, indexType);
}

void CommandBuffer::bindDescriptorSets(VkPipelineBindPoint pipelineType, VkPipelineLayout pipelineLayout, uint32_t indexForFirstSet, const std::vector<DescriptorSet*>& descriptorSets, const std::vector<uint32_t>& dynamicOffsets)
{
	if (descriptorSets.size() > 0)
	{
		std::vector<VkDescriptorSet> descriptorSetHandles;
		descriptorSetHandles.reserve(descriptorSets.size());
		for (auto& descriptorSet : descriptorSets)
		{
			descriptorSetHandles.push_back(descriptorSet->getHandle());
		}

		vkCmdBindDescriptorSets(mCommandBuffer, pipelineType, pipelineLayout, indexForFirstSet, static_cast<uint32_t>(descriptorSetHandles.size()), descriptorSetHandles.data(), static_cast<uint32_t>(dynamicOffsets.size()), dynamicOffsets.data());
	}
}

void CommandBuffer::bindPipeline(GraphicsPipeline* pipeline)
{
	if (pipeline != nullptr && pipeline->isCreated())
	{
		vkCmdBindPipeline(mCommandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, pipeline->getHandle());
	}
	else if (pipeline == nullptr)
	{
		// TODO : Use Numea Log System
		printf("Null pipeline given to command buffer\n");
	}
	else
	{
		// TODO : Use Numea Log System
		printf("Non-created pipeline given to command buffer\n");
	}
}

void CommandBuffer::bindPipeline(ComputePipeline* pipeline)
{
	// TODO : Handle nullptr
	vkCmdBindPipeline(mCommandBuffer, VK_PIPELINE_BIND_POINT_COMPUTE, pipeline->getHandle());
}

void CommandBuffer::provideDataToShadersThroughPushConstants(VkPipelineLayout pipelineLayout, VkShaderStageFlags pipelineStages, uint32_t offset, uint32_t size, void* data)
{
	vkCmdPushConstants(mCommandBuffer, pipelineLayout, pipelineStages, offset, size, data);
}

void CommandBuffer::setViewportStateDynamically(uint32_t firstViewport, const std::vector<VkViewport>& viewports)
{
	vkCmdSetViewport(mCommandBuffer, firstViewport, static_cast<uint32_t>(viewports.size()), viewports.data());
}

void CommandBuffer::setScissorStateDynamically(uint32_t firstScissor, const std::vector<VkRect2D>& scissors)
{
	vkCmdSetScissor(mCommandBuffer, firstScissor, static_cast<uint32_t>(scissors.size()), scissors.data());
}

void CommandBuffer::setLineWidthStateDynamically(float lineWidth)
{
	vkCmdSetLineWidth(mCommandBuffer, lineWidth);
}

void CommandBuffer::setDepthBiasStateDynamically(float constantFactor, float clampValue, float slopeFactor)
{
	vkCmdSetDepthBias(mCommandBuffer, constantFactor, clampValue, slopeFactor);
}

void CommandBuffer::setBlendConstantsStateDynamically(const std::array<float, 4>& blendConstants)
{
	vkCmdSetBlendConstants(mCommandBuffer, blendConstants.data());
}

void CommandBuffer::drawGeometry(uint32_t vertexCount, uint32_t instanceCount, uint32_t firstVertex, uint32_t firstInstance)
{
	vkCmdDraw(mCommandBuffer, vertexCount, instanceCount, firstVertex, firstInstance);
}

void CommandBuffer::drawIndexedGeometry(uint32_t indexCount, uint32_t instanceCount, uint32_t firstIndex, uint32_t vertexOffset, uint32_t firstInstance)
{
	vkCmdDrawIndexed(mCommandBuffer, indexCount, instanceCount, firstIndex, vertexOffset, firstInstance);
}

void CommandBuffer::dispatchComputeWork(uint32_t xSize, uint32_t ySize, uint32_t zSize)
{
	vkCmdDispatch(mCommandBuffer, xSize, ySize, zSize);
}

void CommandBuffer::executeSecondaryCommandBuffer(const std::vector<CommandBuffer*>& secondaryCommandBuffers)
{
	if (secondaryCommandBuffers.size() > 0)
	{
		std::vector<VkCommandBuffer> commandBuffers;
		for (auto& commandBuffer : secondaryCommandBuffers)
		{
			commandBuffers.push_back(commandBuffer->getHandle());
		}

		vkCmdExecuteCommands(mCommandBuffer, static_cast<uint32_t>(commandBuffers.size()), commandBuffers.data());
	}
}

void CommandBuffer::copyDataBetweenBuffers(Buffer* sourceBuffer, Buffer* destinationBuffer, std::vector<VkBufferCopy> regions)
{
	if (regions.size() > 0)
	{
		vkCmdCopyBuffer(mCommandBuffer, sourceBuffer->getHandle(), destinationBuffer->getHandle(), static_cast<uint32_t>(regions.size()), regions.data());
	}
}

void CommandBuffer::copyDataFromBufferToImage(Buffer* sourceBuffer, Image* destinationImage, VkImageLayout imageLayout, std::vector<VkBufferImageCopy> regions)
{
	if (regions.size() > 0)
	{
		vkCmdCopyBufferToImage(mCommandBuffer, sourceBuffer->getHandle(), destinationImage->getHandle(), imageLayout, static_cast<uint32_t>(regions.size()), regions.data());
	}
}

void CommandBuffer::copyDataFromImageToBuffer(Image* sourceImage, VkImageLayout imageLayout, Buffer* destinationBuffer, std::vector<VkBufferImageCopy> regions)
{
	if (regions.size() > 0)
	{
		vkCmdCopyImageToBuffer(mCommandBuffer, sourceImage->getHandle(), imageLayout, destinationBuffer->getHandle(), static_cast<uint32_t>(regions.size()), regions.data());
	}
}

void CommandBuffer::setBufferMemoryBarrier(VkPipelineStageFlags generatingStages, VkPipelineStageFlags consumingStages, std::vector<BufferTransition> bufferTransitions)
{
	std::vector<VkBufferMemoryBarrier> bufferMemoryBarriers;

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
	}

	if (bufferMemoryBarriers.size() > 0) 
	{
		vkCmdPipelineBarrier(mCommandBuffer, generatingStages, consumingStages, 0, 0, nullptr, static_cast<uint32_t>(bufferMemoryBarriers.size()), bufferMemoryBarriers.data(), 0, nullptr);
	}
}

void CommandBuffer::setImageMemoryBarrier(VkPipelineStageFlags generatingStages, VkPipelineStageFlags consumingStages, std::vector<ImageTransition> imageTransitions)
{
	std::vector<VkImageMemoryBarrier> imageMemoryBarriers;

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
	}

	if (imageMemoryBarriers.size() > 0) 
	{
		vkCmdPipelineBarrier(mCommandBuffer, generatingStages, consumingStages, 0, 0, nullptr, 0, nullptr, static_cast<uint32_t>(imageMemoryBarriers.size()), imageMemoryBarriers.data());
	}
}

bool CommandBuffer::reset()
{
	//VkResult result = vkResetCommandBuffer(command_buffer, release_resources ? VK_COMMAND_BUFFER_RESET_RELEASE_RESOURCES_BIT : 0);
	VkResult result = vkResetCommandBuffer(mCommandBuffer, 0);
	if (VK_SUCCESS != result)
	{
		// TODO : Use Numea System Log
		printf("Error occurred during command buffer reset\n");
		return false;
	}
	return true;
}

bool CommandBuffer::isRecording() const
{
	return mRecording;
}

const CommandPool& CommandBuffer::getCommandPool() const
{
	return mCommandPool;
}

const CommandBufferType& CommandBuffer::getType() const
{
	return mType;
}

bool CommandBuffer::isInitialized() const
{
	return mCommandBuffer != VK_NULL_HANDLE;
}

const VkCommandBuffer& CommandBuffer::getHandle() const
{
	return mCommandBuffer;
}

const VkCommandPool& CommandBuffer::getCommandPoolHandle() const
{
	return mCommandPool.getHandle();
}

CommandBuffer::CommandBuffer(CommandPool& commandPool, CommandBufferType type)
	: mCommandBuffer(VK_NULL_HANDLE)
	, mCommandPool(commandPool)
	, mType(type)
	, mRecording(false)
{
	ObjectTracker::registerObject(ObjectType_CommandBuffer);
}

bool CommandBuffer::init()
{
	VkCommandBufferAllocateInfo commandBufferAllocateInfo = {
		VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO,   // VkStructureType          sType
		nullptr,                                          // const void             * pNext
		mCommandPool.getHandle(),                         // VkCommandPool            commandPool
		(VkCommandBufferLevel)mType,                      // VkCommandBufferLevel     level
		1                                                 // uint32_t                 commandBufferCount
	};

	VkResult result = vkAllocateCommandBuffers(mCommandPool.getDeviceHandle(), &commandBufferAllocateInfo, &mCommandBuffer);
	if (result != VK_SUCCESS || mCommandBuffer == VK_NULL_HANDLE)
	{
		// TODO : Use Numea System Log
		printf("Could not allocate command buffers\n");
		return false;
	}
	return true;
}

bool CommandBuffer::release()
{
	if (mCommandBuffer != VK_NULL_HANDLE)
	{
		vkFreeCommandBuffers(mCommandPool.getDeviceHandle(), mCommandPool.getHandle(), 1, &mCommandBuffer);
		mCommandBuffer = VK_NULL_HANDLE;
		return true;
	}
	return false;
}

} // namespace Vulkan
} // namespace nu