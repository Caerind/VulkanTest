#pragma once

#include "VulkanFunctions.hpp"

VULKAN_NAMESPACE_BEGIN

enum VulkanCommandBufferType
{
	Primary = 0,
	Secondary = 1
};

struct VulkanBufferTransition 
{
	VulkanBuffer* buffer;
	VkAccessFlags currentAccess;
	VkAccessFlags newAccess;
	VulkanU32 currentQueueFamily;
	VulkanU32 newQueueFamily;
};

struct VulkanImageTransition 
{
	VkImage image; // TODO : Use Image* instead
	VkAccessFlags currentAccess;
	VkAccessFlags newAccess;
	VkImageLayout currentLayout;
	VkImageLayout newLayout;
	VulkanU32 currentQueueFamily;
	VulkanU32 newQueueFamily;
	VkImageAspectFlags aspect;
};

struct VulkanVertexBufferParameters
{
	VulkanBuffer* buffer;
	VkDeviceSize memoryOffset;
};

class VulkanCommandBuffer : public VulkanObject<VulkanObjectType_CommandBuffer>
{
	public:
		~VulkanCommandBuffer();

		bool beginRecording(VkCommandBufferUsageFlags usage, VkCommandBufferInheritanceInfo* secondaryCommandBufferInfo);
		bool endRecording();

		// TODO : Pass Vulkan...* instead
		void beginRenderPass(VkRenderPass renderPass, VkFramebuffer framebuffer, VkRect2D renderArea, const std::vector<VkClearValue>& clearsValues, VkSubpassContents subpassContents);
		void progressToTheNextSubpass(VkSubpassContents subpassContents);
		void endRenderPass();
		
		// TODO : Pass Vulkan...* instead
		// TODO : Save layout in image
		void clearColorImage(VkImage image, VkImageLayout imageLayout, const std::vector<VkImageSubresourceRange>& imageSubresourceRanges, VkClearColorValue& clearColor);
		void clearDepthStencilImage(VkImage image, VkImageLayout imageLayout, const std::vector<VkImageSubresourceRange>& imageSubresourceRanges, VkClearDepthStencilValue& clearValue);
		void clearRenderPassAttachments(const std::vector<VkClearAttachment>& attachments, const std::vector<VkClearRect>& rects);

		// TODO : Pass Vulkan...* instead
		void bindVertexBuffers(VulkanU32 firstBinding, const std::vector<VulkanVertexBufferParameters>& buffersParameters);
		void bindIndexBuffer(VulkanBuffer* buffer, VkDeviceSize memoryOffset, VkIndexType indexType);
		void bindDescriptorSets(VkPipelineBindPoint pipelineType, VkPipelineLayout pipelineLayout, VulkanU32 indexForFirstSet, const std::vector<VulkanDescriptorSet*>& descriptorsSets, const std::vector<VulkanU32>& dynamicOffsets);
		void bindPipeline(VulkanGraphicsPipeline* pipeline);
		void bindPipeline(VulkanComputePipeline* pipeline);

		// TODO : Pass Vulkan...* instead
		void provideDataToShadersThroughPushConstants(VkPipelineLayout pipelineLayout, VkShaderStageFlags pipelineStages, VulkanU32 offset, VulkanU32 size, void* data);

		// TODO : Create our own Viewport/Scissor class
		void setViewportStateDynamically(VulkanU32 firstViewport, const std::vector<VkViewport>& viewports);
		void setScissorStateDynamically(VulkanU32 firstScissor, const std::vector<VkRect2D>& scissors);
		void setLineWidthStateDynamically(float lineWidth);
		void setDepthBiasStateDynamically(float constantFactor, float clampValue, float slopeFactor);
		void setBlendConstantsStateDynamically(const float (&blendConstants)[4]);

		// TODO : Pass Vulkan...* instead
		void drawGeometry(VulkanU32 vertexCount, VulkanU32 instanceCount, VulkanU32 firstVertex, VulkanU32 firstInstance);
		void drawIndexedGeometry(VulkanU32 indexCount, VulkanU32 instanceCount, VulkanU32 firstIndex, VulkanU32 vertexOffset, VulkanU32 firstInstance);
		
		void dispatchComputeWork(VulkanU32 xSize, VulkanU32 ySize, VulkanU32 zSize);

		// TODO : Pass Vulkan...* instead
		// TODO : Store this as members ? Allow creation of secondary from primary ?
		void executeSecondaryCommandBuffer(const std::vector<VulkanCommandBuffer*>& secondaryCommandBuffers);

		// TODO : Save ImageLayout in Image ?
		void copyDataBetweenBuffers(VulkanBuffer* sourceBuffer, VulkanBuffer* destinationBuffer, std::vector<VkBufferCopy> regions);
		void copyDataFromBufferToImage(VulkanBuffer* sourceBuffer, VulkanImage* destinationImage, VkImageLayout imageLayout, std::vector<VkBufferImageCopy> regions);
		void copyDataFromImageToBuffer(VulkanImage* sourceImage, VkImageLayout imageLayout, VulkanBuffer* destinationBuffer, std::vector<VkBufferImageCopy> regions);

		void setBufferMemoryBarrier(VkPipelineStageFlags generatingStages, VkPipelineStageFlags consumingStages, const std::vector<VulkanBufferTransition>& bufferTransitions);
		void setImageMemoryBarrier(VkPipelineStageFlags generatingStages, VkPipelineStageFlags consumingStages, const std::vector<VulkanImageTransition>& imageTransitions);

		bool reset();

		bool isRecording() const;

		const VulkanCommandPool& getCommandPool() const;

		const VulkanCommandBufferType& getType() const;

		bool isInitialized() const;
		const VkCommandBuffer& getHandle() const;
		const VkCommandPool& getCommandPoolHandle() const;
		// TODO : getCommandPool() ?

	private:
		friend class VulkanCommandPool;
		VulkanCommandBuffer(VulkanCommandPool& commandPool, VulkanCommandBufferType type);

		bool init();
		bool release();

		VkCommandBuffer mCommandBuffer;
		VulkanCommandPool& mCommandPool;
		VulkanCommandBufferType mType;
		bool mRecording;
};

VULKAN_NAMESPACE_END