#pragma once

#include "VulkanFunctions.hpp"

#include "../CookBook/Common.hpp" // TODO : WaitSemaphore & PresentInfo

VULKAN_NAMESPACE_BEGIN

// TODO : Create a specific handle

class VulkanQueue : public VulkanDeviceObject<VulkanObjectType_Queue>
{
	public:
		~VulkanQueue();

		bool useStagingBufferToUpdateBufferAndWait(VkDeviceSize dataSize, void* data, VulkanBuffer* buffer, VkDeviceSize offset, VkAccessFlags currentAccess, VkAccessFlags newAccess, VkPipelineStageFlags generatingStages, VkPipelineStageFlags consumingStages, VulkanCommandBuffer* commandBuffer, std::vector<VkSemaphore> signalSemaphores, uint64_t timeout);
		bool useStagingBufferToUpdateImageAndWait(VkDeviceSize dataSize, void* data, VulkanImage* image, VkImageSubresourceLayers subresource, VkOffset3D offset, VkExtent3D size, VkImageLayout currentLayout, VkImageLayout newLayout, VkAccessFlags currentAccess, VkAccessFlags newAccess, VkImageAspectFlags aspect, VkPipelineStageFlags generatingStages, VkPipelineStageFlags consumingStages, VulkanCommandBuffer* commandBuffer, std::vector<VkSemaphore> signalSemaphores, uint64_t timeout);

		bool submitCommandBuffers(std::vector<VulkanCommandBuffer*> commandBuffers, std::vector<WaitSemaphoreInfo> waitSemaphoreInfos, std::vector<VkSemaphore> signalSemaphores, VulkanFence* fence);
		bool submitCommandBuffersAndWait(std::vector<VulkanCommandBuffer*> commandBuffers, std::vector<WaitSemaphoreInfo> waitSemaphoreInfos, std::vector<VkSemaphore> signalSemaphores, VulkanFence* fence, uint64_t timeout);
		
		bool presentImage(std::vector<VkSemaphore> renderingSemaphores, std::vector<PresentInfo> imagesToPresent);

		VulkanU32 getFamilyIndex() const;
		VulkanU32 getIndex() const;
		const VkQueueFamilyProperties& getFamilyProperties() const;
		const VkQueueFlags& getFlags() const;
		bool hasGraphicsFlag() const;
		bool hasComputeFlag() const;
		bool hasTransferFlag() const;
		bool hasSparseBindingFlag() const;

		bool isInitialized() const;
		const VkQueue& getHandle() const;

	private:
		friend class VulkanQueueManager;
		VulkanQueue(VulkanDevice& device, VulkanU32 familyIndex, VulkanU32 index);
		bool initialize();

		VkQueue mQueue;

		VulkanU32 mFamilyIndex;
		VulkanU32 mIndex;
};

VULKAN_NAMESPACE_END