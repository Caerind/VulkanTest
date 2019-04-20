#pragma once

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
	VulkanCommandBufferPtr mCommandBuffer;
	VulkanSemaphorePtr mImageAcquiredSemaphore;
	VulkanSemaphorePtr mReadyToPresentSemaphore;
	VulkanFencePtr mDrawingFinishedFence;
	VulkanImageView* mDepthAttachment;
	VulkanFramebufferPtr mFramebuffer;

	FrameResources(VulkanCommandBufferPtr command_buffer,
		VulkanSemaphorePtr image_acquired_semaphore,
		VulkanSemaphorePtr ready_to_present_semaphore,
		VulkanFencePtr drawing_finished_fence,
		VulkanImageView* depth_attachment,
		VulkanFramebufferPtr framebuffer) :
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

