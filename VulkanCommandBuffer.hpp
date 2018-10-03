#ifndef NU_VULKAN_COMMAND_BUFFER_HPP
#define NU_VULKAN_COMMAND_BUFFER_HPP

#include "VulkanFunctions.hpp"

#include <array>
#include <memory>
#include <vector>

#include "VulkanBuffer.hpp"
#include "VulkanImage.hpp"
#include "VulkanDescriptorSet.hpp"

namespace nu
{
namespace Vulkan
{

enum CommandBufferType
{
	Primary = 0,
	Secondary = 1
};

struct BufferTransition 
{
	Buffer* buffer;
	VkAccessFlags currentAccess;
	VkAccessFlags newAccess;
	uint32_t currentQueueFamily;
	uint32_t newQueueFamily;
};

struct ImageTransition 
{
	VkImage image; // TODO : Use Image* instead
	VkAccessFlags currentAccess;
	VkAccessFlags newAccess;
	VkImageLayout currentLayout;
	VkImageLayout newLayout;
	uint32_t currentQueueFamily;
	uint32_t newQueueFamily;
	VkImageAspectFlags aspect;
};

struct VertexBufferParameters
{
	Buffer* buffer;
	VkDeviceSize memoryOffset;
};

class CommandPool;
class CommandBuffer
{
	public:
		typedef std::unique_ptr<CommandBuffer> Ptr;

		~CommandBuffer();

		bool beginRecording(VkCommandBufferUsageFlags usage, VkCommandBufferInheritanceInfo* secondaryCommandBufferInfo);
		bool endRecording();

		// TODO : nu::Vulkan::...* instead
		void beginRenderPass(VkRenderPass renderPass, VkFramebuffer framebuffer, VkRect2D renderArea, const std::vector<VkClearValue>& clearsValues, VkSubpassContents subpassContents);
		void progressToTheNextSubpass(VkSubpassContents subpassContents);
		void endRenderPass();
		
		// TODO : Pass nu::Vulkan::...* instead
		// TODO : Save layout in image
		void clearColorImage(VkImage image, VkImageLayout imageLayout, const std::vector<VkImageSubresourceRange>& imageSubresourceRanges, VkClearColorValue& clearColor);
		void clearDepthStencilImage(VkImage image, VkImageLayout imageLayout, const std::vector<VkImageSubresourceRange>& imageSubresourceRanges, VkClearDepthStencilValue& clearValue);
		void clearRenderPassAttachments(const std::vector<VkClearAttachment>& attachments, const std::vector<VkClearRect>& rects);

		// TODO : Pass nu::Vulkan::...* instead
		void bindVertexBuffers(uint32_t firstBinding, const std::vector<VertexBufferParameters>& buffersParameters);
		void bindIndexBuffer(Buffer* buffer, VkDeviceSize memoryOffset, VkIndexType indexType);
		void bindDescriptorSets(VkPipelineBindPoint pipelineType, VkPipelineLayout pipelineLayout, uint32_t indexForFirstSet, const std::vector<DescriptorSet*>& descriptorsSets, const std::vector<uint32_t>& dynamicOffsets);
		void bindPipelineObject(VkPipelineBindPoint pipelineType, VkPipeline pipeline);

		// TODO : Pass nu::Vulkan::...* instead
		void provideDataToShadersThroughPushConstants(VkPipelineLayout pipelineLayout, VkShaderStageFlags pipelineStages, uint32_t offset, uint32_t size, void* data);

		void setViewportStateDynamically(uint32_t firstViewport, const std::vector<VkViewport>& viewports);
		void setScissorStateDynamically(uint32_t firstScissor, const std::vector<VkRect2D>& scissors);
		void setLineWidthStateDynamically(float lineWidth);
		void setDepthBiasStateDynamically(float constantFactor, float clampValue, float slopeFactor);
		void setBlendConstantsStateDynamically(const std::array<float, 4>& blendConstants);

		// TODO : Pass nu::Vulkan::...* instead
		void drawGeometry(uint32_t vertexCount, uint32_t instanceCount, uint32_t firstVertex, uint32_t firstInstance);
		void drawIndexedGeometry(uint32_t indexCount, uint32_t instanceCount, uint32_t firstIndex, uint32_t vertexOffset, uint32_t firstInstance);
		
		void dispatchComputeWork(uint32_t xSize, uint32_t ySize, uint32_t zSize);

		// TODO : Pass nu::Vulkan::...* instead
		void executeSecondaryCommandBuffer(const std::vector<CommandBuffer*>& secondaryCommandBuffers);

		void copyDataBetweenBuffers(Buffer* sourceBuffer, Buffer* destinationBuffer, std::vector<VkBufferCopy> regions);
		void copyDataFromBufferToImage(Buffer* sourceBuffer, Image* destinationImage, VkImageLayout imageLayout, std::vector<VkBufferImageCopy> regions);
		void copyDataFromImageToBuffer(Image* sourceImage, VkImageLayout imageLayout, Buffer* destinationBuffer, std::vector<VkBufferImageCopy> regions);

		void setBufferMemoryBarrier(VkPipelineStageFlags generatingStages, VkPipelineStageFlags consumingStages, std::vector<BufferTransition> bufferTransitions);
		void setImageMemoryBarrier(VkPipelineStageFlags generatingStages, VkPipelineStageFlags consumingStages, std::vector<ImageTransition> imageTransitions);

		bool reset();

		bool isRecording() const;

		const CommandPool& getCommandPool() const;

		const CommandBufferType& getType() const;

		bool isInitialized() const;
		const VkCommandBuffer& getHandle() const;
		const VkCommandPool& getCommandPoolHandle() const;

	private:
		friend class CommandPool;
		CommandBuffer(CommandPool& commandPool, CommandBufferType type);

		bool init();
		bool release();

		VkCommandBuffer mCommandBuffer;
		CommandPool& mCommandPool;
		CommandBufferType mType;
		bool mRecording;

		friend class CommandPool;
};

} // namespace Vulkan
} // namespace nu

#endif // NU_VULKAN_COMMAND_BUFFER_HPP