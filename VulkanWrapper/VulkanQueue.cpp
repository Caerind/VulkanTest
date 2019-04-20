#include "VulkanQueue.hpp"

#include "VulkanDevice.hpp"
#include "VulkanBuffer.hpp"
#include "VulkanImage.hpp"
#include "VulkanMemoryBlock.hpp"
#include "VulkanFence.hpp"

VULKAN_NAMESPACE_BEGIN

VulkanQueue::~VulkanQueue()
{
	VULKAN_OBJECTTRACKER_UNREGISTER();
}

bool VulkanQueue::useStagingBufferToUpdateBufferAndWait(VkDeviceSize dataSize, void* data, VulkanBuffer* buffer, VkDeviceSize offset, VkAccessFlags currentAccess, VkAccessFlags newAccess, VkPipelineStageFlags generatingStages, VkPipelineStageFlags consumingStages, VulkanCommandBuffer* commandBuffer, std::vector<VkSemaphore> signalSemaphores, uint64_t timeout)
{
	VulkanFencePtr fence = getDevice().createFence(false);
	if (fence == nullptr)
	{
		return false;
	}

	VulkanBufferPtr stagingBuffer = getDevice().createBuffer(dataSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT);
	if (stagingBuffer == nullptr)
	{
		return false;
	}
	VulkanMemoryBlock* stagingBufferMemory = stagingBuffer->allocateMemoryBlock(VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT);
	if (stagingBufferMemory == nullptr)
	{
		return false;
	}

	if (!stagingBufferMemory->map(0, dataSize))
	{
		return false;
	}
	if (!stagingBufferMemory->write(data))
	{
		return false;
	}
	if (!stagingBufferMemory->unmap())
	{
		return false;
	}

	if (!commandBuffer->beginRecording(VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT, nullptr))
	{
		return false;
	}

	commandBuffer->setBufferMemoryBarrier(generatingStages, VK_PIPELINE_STAGE_TRANSFER_BIT, { { buffer, currentAccess, VK_ACCESS_TRANSFER_WRITE_BIT, VK_QUEUE_FAMILY_IGNORED, VK_QUEUE_FAMILY_IGNORED } });
	commandBuffer->copyDataBetweenBuffers(stagingBuffer.get(), buffer, { { 0, offset, dataSize } });
	commandBuffer->setBufferMemoryBarrier(VK_PIPELINE_STAGE_TRANSFER_BIT, consumingStages, { { buffer, VK_ACCESS_TRANSFER_WRITE_BIT, newAccess, VK_QUEUE_FAMILY_IGNORED, VK_QUEUE_FAMILY_IGNORED } });

	if (!commandBuffer->endRecording())
	{
		return false;
	}

	if (!submitCommandBuffers({ commandBuffer }, {}, signalSemaphores, fence.get()))
	{
		return false;
	}

	return fence->wait(timeout);
}

bool VulkanQueue::useStagingBufferToUpdateImageAndWait(VkDeviceSize dataSize, void* data, VulkanImage* image, VkImageSubresourceLayers subresource, VkOffset3D offset, VkExtent3D size, VkImageLayout currentLayout, VkImageLayout newLayout, VkAccessFlags currentAccess, VkAccessFlags newAccess, VkImageAspectFlags aspect, VkPipelineStageFlags generatingStages, VkPipelineStageFlags consumingStages, VulkanCommandBuffer* commandBuffer, std::vector<VkSemaphore> signalSemaphores, uint64_t timeout)
{
	VulkanFencePtr fence = getDevice().createFence(false);
	if (fence == nullptr)
	{
		return false;
	}

	VulkanBufferPtr stagingBuffer = getDevice().createBuffer(dataSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT);
	if (stagingBuffer == nullptr)
	{
		return false;
	}
	VulkanMemoryBlock* stagingBufferMemory = stagingBuffer->allocateMemoryBlock(VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT);
	if (stagingBufferMemory == nullptr)
	{
		return false;
	}

	if (!stagingBufferMemory->map(0, dataSize))
	{
		return false;
	}
	if (!stagingBufferMemory->write(data))
	{
		return false;
	}
	if (!stagingBufferMemory->unmap())
	{
		return false;
	}

	if (!commandBuffer->beginRecording(VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT, nullptr))
	{
		return false;
	}

	commandBuffer->setImageMemoryBarrier(generatingStages, VK_PIPELINE_STAGE_TRANSFER_BIT,
		{
			{
				image->getHandle(),                       // VkImage            image
				currentAccess,                            // VkAccessFlags      currentAccess
				VK_ACCESS_TRANSFER_WRITE_BIT,             // VkAccessFlags      newAccess
				currentLayout,                            // VkImageLayout      currentLayout
				VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,     // VkImageLayout      newLayout
				VK_QUEUE_FAMILY_IGNORED,                  // uint32_t           currentQueueFamily
				VK_QUEUE_FAMILY_IGNORED,                  // uint32_t           newQueueFamily
				aspect                                    // VkImageAspectFlags aspect
			}
		}
	);

	commandBuffer->copyDataFromBufferToImage(stagingBuffer.get(), image, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
		{
			{
				0,                                        // VkDeviceSize               bufferOffset
				0,                                        // uint32_t                   bufferRowLength
				0,                                        // uint32_t                   bufferImageHeight
				subresource,                              // VkImageSubresourceLayers   imageSubresource
				offset,                                   // VkOffset3D                 imageOffset
				size,                                     // VkExtent3D                 imageExtent
			}
		}
	);

	commandBuffer->setImageMemoryBarrier(VK_PIPELINE_STAGE_TRANSFER_BIT, consumingStages,
		{
			{
				image->getHandle(),                       // VkImage            image
				VK_ACCESS_TRANSFER_WRITE_BIT,             // VkAccessFlags      currentAccess
				newAccess,                                // VkAccessFlags      newAccess
				VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,     // VkImageLayout      currentLayout
				newLayout,                                // VkImageLayout      newLayout
				VK_QUEUE_FAMILY_IGNORED,                  // uint32_t           currentQueueFamily
				VK_QUEUE_FAMILY_IGNORED,                  // uint32_t           newQueueFamily
				aspect                                    // VkImageAspectFlags aspect
			}
		}
	);

	if (!commandBuffer->endRecording())
	{
		return false;
	}

	if (!submitCommandBuffers({ commandBuffer }, {}, signalSemaphores, fence.get()))
	{
		return false;
	}

	return fence->wait(timeout);
}

bool VulkanQueue::submitCommandBuffers(std::vector<VulkanCommandBuffer*> commandBuffers, std::vector<WaitSemaphoreInfo> waitSemaphoreInfos, std::vector<VkSemaphore> signalSemaphores, VulkanFence* fence)
{
	std::vector<VkSemaphore> waitSemaphoreHandles;
	std::vector<VkPipelineStageFlags> waitSemaphoreStages;
	for (auto& waitSemaphoreInfo : waitSemaphoreInfos) 
	{
		waitSemaphoreHandles.emplace_back(waitSemaphoreInfo.Semaphore);
		waitSemaphoreStages.emplace_back(waitSemaphoreInfo.WaitingStage);
	}

	std::vector<VkCommandBuffer> commandBufferHandles;
	for (auto& commandBuffer : commandBuffers)
	{
		commandBufferHandles.emplace_back(commandBuffer->getHandle());
	}

	VkSubmitInfo submitInfo = {
		VK_STRUCTURE_TYPE_SUBMIT_INFO,                        // VkStructureType                sType
		nullptr,                                              // const void                   * pNext
		static_cast<uint32_t>(waitSemaphoreInfos.size()),     // uint32_t                       waitSemaphoreCount
		waitSemaphoreHandles.data(),                          // const VkSemaphore            * pWaitSemaphores
		waitSemaphoreStages.data(),                           // const VkPipelineStageFlags   * pWaitDstStageMask
		static_cast<uint32_t>(commandBufferHandles.size()),   // uint32_t                       commandBufferCount
		commandBufferHandles.data(),                          // const VkCommandBuffer        * pCommandBuffers
		static_cast<uint32_t>(signalSemaphores.size()),       // uint32_t                       signalSemaphoreCount
		signalSemaphores.data()                               // const VkSemaphore            * pSignalSemaphores
	};

	VkResult result = vkQueueSubmit(mQueue, 1, &submitInfo, fence->getHandle());
	if (result != VK_SUCCESS)
	{
		printf("Error occurred during command buffer submission\n");
		return false;
	}
	return true;
}

bool VulkanQueue::submitCommandBuffersAndWait(std::vector<VulkanCommandBuffer*> commandBuffers, std::vector<WaitSemaphoreInfo> waitSemaphoreInfos, std::vector<VkSemaphore> signalSemaphores, VulkanFence* fence, uint64_t timeout)
{
	if (!submitCommandBuffers(commandBuffers, waitSemaphoreInfos, signalSemaphores, fence))
	{
		return false;
	}

	return fence->wait(timeout);
}

bool VulkanQueue::presentImage(std::vector<VkSemaphore> renderingSemaphores, std::vector<PresentInfo> imagesToPresent)
{
	VkResult result;

	std::vector<VkSwapchainKHR> swapchains;
	std::vector<uint32_t> imageIndices;
	for (auto& imageToPresent : imagesToPresent) 
	{
		swapchains.emplace_back(imageToPresent.Swapchain);
		imageIndices.emplace_back(imageToPresent.ImageIndex);
	}

	VkPresentInfoKHR presentInfo = {
		VK_STRUCTURE_TYPE_PRESENT_INFO_KHR,                   // VkStructureType          sType
		nullptr,                                              // const void*              pNext
		static_cast<uint32_t>(renderingSemaphores.size()),    // uint32_t                 waitSemaphoreCount
		renderingSemaphores.data(),                           // const VkSemaphore      * pWaitSemaphores
		static_cast<uint32_t>(swapchains.size()),             // uint32_t                 swapchainCount
		swapchains.data(),                                    // const VkSwapchainKHR   * pSwapchains
		imageIndices.data(),                                  // const uint32_t         * pImageIndices
		nullptr                                               // VkResult*                pResults
	};

	result = vkQueuePresentKHR(mQueue, &presentInfo);
	if (result != VK_SUCCESS)
	{
		// TODO : Log
		printf("Could not present image\n");
		return false;
	}
	return true;
}

VulkanU32 VulkanQueue::getFamilyIndex() const
{
	return mFamilyIndex;
}

VulkanU32 VulkanQueue::getIndex() const
{
	return mIndex;
}

const VkQueueFamilyProperties& VulkanQueue::getFamilyProperties() const
{
	return getDevice().getQueueFamiliesProperties()[mFamilyIndex];
}

const VkQueueFlags& VulkanQueue::getFlags() const
{
	return getFamilyProperties().queueFlags;
}

bool VulkanQueue::hasGraphicsFlag() const
{
	return (getFlags() & VK_QUEUE_GRAPHICS_BIT) != 0;
}

bool VulkanQueue::hasComputeFlag() const
{
	return (getFlags() & VK_QUEUE_COMPUTE_BIT) != 0;
}

bool VulkanQueue::hasTransferFlag() const
{
	return (getFlags() & VK_QUEUE_TRANSFER_BIT) != 0;
}

bool VulkanQueue::hasSparseBindingFlag() const
{
	return (getFlags() & VK_QUEUE_SPARSE_BINDING_BIT) != 0;
}

bool VulkanQueue::isInitialized() const
{
	return mQueue != VK_NULL_HANDLE;
}

const VkQueue& VulkanQueue::getHandle() const
{
	return mQueue;
}

VulkanQueue::VulkanQueue(VulkanDevice& device, VulkanU32 familyIndex, VulkanU32 index)
	: mQueue(VK_NULL_HANDLE)
	, mFamilyIndex(familyIndex)
	, mIndex(index)
{
	VULKAN_OBJECTTRACKER_REGISTER();
	
	// TODO : Move this elsewhere ?
	#if (!defined VULKAN_MONO_DEVICE)
	setDeviceId(device.getDeviceId());
	#endif
}

bool VulkanQueue::initialize()
{
	vkGetDeviceQueue(getDeviceHandle(), mFamilyIndex, mIndex, &mQueue);
	return true;
}

VULKAN_NAMESPACE_END