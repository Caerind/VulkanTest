#ifndef NU_VULKAN_SWAPCHAIN_HPP
#define NU_VULKAN_SWAPCHAIN_HPP

#include "CookBook/Common.hpp"

#include <memory>

#include "VulkanImage.hpp"
#include "VulkanImageView.hpp"
#include "VulkanHelper.hpp"

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

		static Swapchain::Ptr createSwapchain(Device& device, Surface* surface, std::vector<FrameResources>& framesResources);

		~Swapchain();

		bool recreate(std::vector<FrameResources>& framesResources);

		const VkFormat& getFormat() const;
		const VkColorSpaceKHR& getColorSpace() const;
		const VkExtent2D& getSize() const;

		const VkFormat& getDepthFormat() const;

		bool acquireImageIndex(uint64_t timeout, VkSemaphore semaphore, VkFence fence, uint32_t& imageIndex);
		// TODO : Maybe acquireImage directly instead of index ?

		// TODO : Improve accessors (const and ref where needed)
		VkImage getImage(uint32_t index) const;
		VkImageView getImageView(uint32_t index) const;

		bool isInitialized() const;
		const VkSwapchainKHR& getHandle() const;
		const VkDevice& getDeviceHandle() const;

	private:
		Swapchain(Device& device, Surface* surface);

		bool init(std::vector<FrameResources>& framesResources);
		bool release();

		bool createSwapchainInternal(VkImageUsageFlags desiredImageUsage, VkSwapchainKHR& oldSwapchain); // TODO : Move old swapchain inside
		bool selectDesiredPresentMode(VkPresentModeKHR desiredPresentMode, VkPresentModeKHR& presentMode);
		bool queryCapabilities(VkSurfaceCapabilitiesKHR& surfaceCapabilities);
		bool selectSurfaceFormat(VkSurfaceFormatKHR desiredSurfaceFormat);
		bool queryHandlesOfImages();

	private:
		VkSwapchainKHR mSwapchain;
		Device& mDevice;
		Surface* mSurface;
		bool mReady;

		VkFormat mFormat;
		VkColorSpaceKHR mColorSpace;
		VkExtent2D mSize;

		std::vector<VkImage> mSwapchainImages;
		std::vector<ImageView::Ptr> mSwapchainImageViews;
		std::vector<VkImageView> mSwapchainImageViewsRaw;

		VkFormat mDepthFormat;
		std::vector<ImageHelper::Ptr> mSwapchainDepthImages;
};

} // namespace Vulkan
} // namespace nu

#endif // NU_VULKAN_SWAPCHAIN_HPP