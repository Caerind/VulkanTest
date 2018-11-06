#ifndef NU_VULKAN_QUEUE_HPP
#define NU_VULKAN_QUEUE_HPP

#include "VulkanFunctions.hpp"

#include "VulkanCommandBuffer.hpp"
#include "VulkanFence.hpp"

#include <memory>
#include <vector>

#include "../CookBook/Common.hpp" // TODO : WaitSemaphore & PresentInfo

namespace nu
{
namespace Vulkan
{

class Device;
class Queue
{
	public:
		typedef std::unique_ptr<Queue> Ptr;

		~Queue();

		bool useStagingBufferToUpdateBufferAndWait(VkDeviceSize dataSize, void* data, Buffer* buffer, VkDeviceSize offset, VkAccessFlags currentAccess, VkAccessFlags newAccess, VkPipelineStageFlags generatingStages, VkPipelineStageFlags consumingStages, CommandBuffer* commandBuffer, std::vector<VkSemaphore> signalSemaphores, uint64_t timeout);
		bool useStagingBufferToUpdateImageAndWait(VkDeviceSize dataSize, void* data, Image* image, VkImageSubresourceLayers subresource, VkOffset3D offset, VkExtent3D size, VkImageLayout currentLayout, VkImageLayout newLayout, VkAccessFlags currentAccess, VkAccessFlags newAccess, VkImageAspectFlags aspect, VkPipelineStageFlags generatingStages, VkPipelineStageFlags consumingStages, CommandBuffer* commandBuffer, std::vector<VkSemaphore> signalSemaphores, uint64_t timeout);

		bool submitCommandBuffers(std::vector<CommandBuffer*> commandBuffers, std::vector<WaitSemaphoreInfo> waitSemaphoreInfos, std::vector<VkSemaphore> signalSemaphores, Fence* fence);
		bool submitCommandBuffersAndWait(std::vector<CommandBuffer*> commandBuffers, std::vector<WaitSemaphoreInfo> waitSemaphoreInfos, std::vector<VkSemaphore> signalSemaphores, Fence* fence, uint64_t timeout);
		
		bool presentImage(std::vector<VkSemaphore> renderingSemaphores, std::vector<PresentInfo> imagesToPresent);

		uint32_t getFamilyIndex() const;
		uint32_t getIndex() const;

		bool isInitialized() const;
		const VkQueue& getHandle() const;
		const VkDevice& getDeviceHandle() const;

	private:
		friend class Device;
		Queue(Device& device, uint32_t queueFamilyIndex, uint32_t queueIndex);

		Device& mDevice;
		VkQueue mQueue;

		uint32_t mFamilyIndex;
		uint32_t mIndex;
};

} // namespace Vulkan
} // namespace nu

#endif // NU_VULKAN_QUEUE_HPP