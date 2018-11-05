#ifndef NU_VULKAN_IMAGE_VIEW_HPP
#define NU_VULKAN_IMAGE_VIEW_HPP

#include "VulkanFunctions.hpp"

#include <memory>

namespace nu
{
namespace Vulkan
{

class Device;
class Image;
class Swapchain;
class ImageView
{
	public:
		typedef std::unique_ptr<ImageView> Ptr;

		static ImageView::Ptr createImageView(Device& device, Image& image, VkImageViewType viewType, VkFormat format, VkImageAspectFlags aspect);

		~ImageView();

		bool isInitialized() const;
		const VkImageView& getHandle() const;
		const VkImage& getImageHandle() const;
		const VkDevice& getDeviceHandle() const;

	private:
		ImageView(Device& device, Image* image, VkImage imageHandleFromSwapchain, VkImageViewType viewType, VkFormat format, VkImageAspectFlags aspect);

		bool init();
		bool release();

		Device& mDevice;
		Image* mImage;
		VkImageView mImageView;

		VkImageViewType mViewType;
		VkFormat mFormat;
		VkImageAspectFlags mAspect;

		friend class Swapchain;
		static ImageView::Ptr createImageViewFromSwapchain(Device& device, VkImage imageHandleFromSwapchain, VkImageViewType viewType, VkFormat format, VkImageAspectFlags aspect);
		VkImage mImageHandleFromSwapchain;
};

} // namespace Vulkan
} // namespace nu

#endif // NU_VULKAN_IMAGE_VIEW_HPP