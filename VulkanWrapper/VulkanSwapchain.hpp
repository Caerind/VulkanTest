#ifndef NU_VULKAN_SWAPCHAIN_HPP
#define NU_VULKAN_SWAPCHAIN_HPP

#include <memory>

#include "VulkanImage.hpp"
#include "VulkanImageView.hpp"
#include "VulkanHelper.hpp"

#include "../CookBook/Common.hpp" // TODO : Remove this (used for FrameResources)

namespace nu
{
namespace Vulkan
{

class Device;
class Surface;
class Swapchain
{
	public:
		typedef std::unique_ptr<Swapchain> Ptr;

		~Swapchain();

		bool recreate(std::vector<FrameResources>& framesResources);

		const VkFormat& getFormat() const;
		const VkColorSpaceKHR& getColorSpace() const;
		const VkExtent2D& getSize() const;

		const VkFormat& getDepthFormat() const;

		bool acquireImageIndex(uint64_t timeout, VkSemaphore semaphore, VkFence fence, uint32_t& imageIndex);
		// TODO : Maybe acquireImage directly instead of index ?

		VkImage getImageHandle(uint32_t index) const;
		VkImageView getImageViewHandle(uint32_t index) const;

		Surface& getSurface();
		const Surface& getSurface() const;
		Device& getDevice();
		const Device& getDevice() const;
		const VkSwapchainKHR& getHandle() const;
		const VkSurfaceKHR& getSurfaceHandle() const;
		const VkDevice& getDeviceHandle() const;

	private:
		friend class Device;
		static Swapchain::Ptr createSwapchain(Device& device, Surface& surface, std::vector<FrameResources>& framesResources);

		Swapchain(Device& device, Surface& surface);

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
		Device& mDevice;
		Surface& mSurface;
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

		std::vector<Image::Ptr> mSwapchainImages;

		VkFormat mDepthFormat;
		std::vector<ImageHelper::Ptr> mSwapchainDepthImages;
};

} // namespace Vulkan
} // namespace nu

#endif // NU_VULKAN_SWAPCHAIN_HPP