#ifndef COMMON
#define COMMON

#ifdef _WIN32
	#include <Windows.h>
#elif defined __linux
	#include <dlfcn.h>
#endif

#include <iostream>
#include <vector>
#include <array>
#include <string>
#include <cstring>
#include <thread>
#include <cmath>
#include <functional>
#include <memory>
#include <vector>

#include "../VulkanWrapper/VulkanFunctions.hpp"

#include "../VulkanWrapper/VulkanCommandBuffer.hpp"
#include "../VulkanWrapper/VulkanSemaphore.hpp"
#include "../VulkanWrapper/VulkanFence.hpp"
#include "../VulkanWrapper/VulkanImageView.hpp"
#include "../VulkanWrapper/VulkanFramebuffer.hpp"

struct FrameResources 
{
	nu::Vulkan::CommandBuffer::Ptr mCommandBuffer;
	nu::Vulkan::Semaphore::Ptr mImageAcquiredSemaphore;
	nu::Vulkan::Semaphore::Ptr mReadyToPresentSemaphore;
	nu::Vulkan::Fence::Ptr mDrawingFinishedFence;
	nu::Vulkan::ImageView* mDepthAttachment;
	nu::Vulkan::Framebuffer::Ptr mFramebuffer;

	FrameResources(nu::Vulkan::CommandBuffer::Ptr command_buffer,
		nu::Vulkan::Semaphore::Ptr image_acquired_semaphore,
		nu::Vulkan::Semaphore::Ptr ready_to_present_semaphore,
		nu::Vulkan::Fence::Ptr drawing_finished_fence,
		nu::Vulkan::ImageView* depth_attachment,
		nu::Vulkan::Framebuffer::Ptr framebuffer) :
		mCommandBuffer(std::move(command_buffer)),
		mImageAcquiredSemaphore(std::move(image_acquired_semaphore)),
		mReadyToPresentSemaphore(std::move(ready_to_present_semaphore)),
		mDrawingFinishedFence(std::move(drawing_finished_fence)),
		mDepthAttachment(depth_attachment),
		mFramebuffer(std::move(framebuffer)) 
	{
	}

	FrameResources(FrameResources && other) 
	{
		*this = std::move(other);
	}

	FrameResources& operator=(FrameResources && other) 
	{
		if (this != &other) {
			mCommandBuffer = std::move(other.mCommandBuffer);
			mImageAcquiredSemaphore = std::move(other.mImageAcquiredSemaphore);
			mReadyToPresentSemaphore = std::move(other.mReadyToPresentSemaphore);
			mDrawingFinishedFence = std::move(other.mDrawingFinishedFence);
			mDepthAttachment = std::move(other.mDepthAttachment);
			mFramebuffer = std::move(other.mFramebuffer);
		}
		return *this;
	}

	FrameResources(FrameResources const &) = delete;
	FrameResources& operator=(FrameResources const &) = delete;
};

struct SubpassParameters 
{
	VkPipelineBindPoint                  PipelineType;
	std::vector<VkAttachmentReference>   InputAttachments;
	std::vector<VkAttachmentReference>   ColorAttachments;
	std::vector<VkAttachmentReference>   ResolveAttachments;
	const VkAttachmentReference*         DepthStencilAttachment;
	std::vector<uint32_t>                PreserveAttachments;
};

struct WaitSemaphoreInfo
{
	VkSemaphore           Semaphore;
	VkPipelineStageFlags  WaitingStage;
}; 

struct PresentInfo 
{
	VkSwapchainKHR  Swapchain;
	uint32_t        ImageIndex;
};

// TODO : Move this somewhere else
bool loadTextureDataFromFile(const std::string& filename, int numRequestedComponents, std::vector<unsigned char>& data, int* width = nullptr, int* height = nullptr, int* numComponents = nullptr, int* dataSize = nullptr);

#endif // COMMON
