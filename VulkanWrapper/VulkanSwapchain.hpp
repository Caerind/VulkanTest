#pragma once

#include "VulkanHelper.hpp" // TODO : Remove this include
#include "../CookBook/Common.hpp" // TODO : Remove this (used for FrameResources)

VULKAN_NAMESPACE_BEGIN

class VulkanSwapchain : public VulkanObject<VulkanObjectType_Swapchain>
{
	public:
		~VulkanSwapchain();

		bool recreate(std::vector<FrameResources>& framesResources);

		const VkFormat& getFormat() const;
		const VkColorSpaceKHR& getColorSpace() const;
		const VkExtent2D& getSize() const;

		const VkFormat& getDepthFormat() const;

		bool acquireImageIndex(uint64_t timeout, VkSemaphore semaphore, VkFence fence, uint32_t& imageIndex);
		// TODO : Maybe acquireImage directly instead of index ?

		VkImage getImageHandle(uint32_t index) const;
		VkImageView getImageViewHandle(uint32_t index) const;

		VulkanSurface& getSurface();
		const VulkanSurface& getSurface() const;
		VulkanDevice& getDevice();
		const VulkanDevice& getDevice() const;
		const VkSwapchainKHR& getHandle() const;
		const VkSurfaceKHR& getSurfaceHandle() const;
		const VkDevice& getDeviceHandle() const;

	private:
		friend class VulkanDevice;
		static VulkanSwapchainPtr createSwapchain(VulkanDevice& device, VulkanSurface& surface, std::vector<FrameResources>& framesResources);

		VulkanSwapchain(VulkanDevice& device, VulkanSurface& surface);

		bool init(std::vector<FrameResources>& framesResources);
		void release();

		bool selectPresentMode(VkPresentModeKHR desiredPresentMode);
		bool queryCapabilities();
		bool selectImageCount(uint32_t desiredImageCount);
		bool querySize();
		bool selectImageUsage(VkImageUsageFlags desiredImageUsage);
		bool selectSurfaceTransform(VkSurfaceTransformFlagBitsKHR desiredSurfaceTransform);
		bool selectSurfaceFormat(VkSurfaceFormatKHR desiredSurfaceFormat);
		bool queryImageHandles(std::vector<VkImage>& swapchainImageHandles);

	private:
		VulkanDevice& mDevice;
		VulkanSurface& mSurface;
		VkSwapchainKHR mSwapchain;
		bool mReady;

		VkPresentModeKHR mPresentMode;
		VkSurfaceCapabilitiesKHR mSurfaceCapabilities;
		uint32_t mImageCount;
		VkExtent2D mSize;
		VkImageUsageFlags mImageUsage;
		VkSurfaceTransformFlagBitsKHR mSurfaceTransform;
		VkFormat mFormat;
		VkColorSpaceKHR mColorSpace;

		std::vector<VulkanImagePtr> mSwapchainImages;

		VkFormat mDepthFormat;
		std::vector<VulkanImageHelperPtr> mSwapchainDepthImages;
};

VULKAN_NAMESPACE_END