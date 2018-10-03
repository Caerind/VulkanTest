// MIT License
//
// Copyright( c ) 2017 Packt
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files( the "Software" ), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and / or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions :
//
// The above copyright notice and this permission notice shall be included in all
// copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.
//
// Vulkan Cookbook
// ISBN: 9781786468154
// © Packt Publishing Limited
//
// Author:   Pawel Lapinski
// LinkedIn: https://www.linkedin.com/in/pawel-lapinski-84522329
//
// Common

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

#include "../VulkanFunctions.hpp"

#include "../VulkanCommandBuffer.hpp"
#include "../VulkanSemaphore.hpp"
#include "../VulkanFence.hpp"
#include "../VulkanImageView.hpp"
#include "../VulkanFramebuffer.hpp"

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
bool getBinaryFileContents(const std::string& filename, std::vector<unsigned char>& contents);

#endif // COMMON
