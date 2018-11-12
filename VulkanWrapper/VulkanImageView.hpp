#ifndef NU_VULKAN_IMAGE_VIEW_HPP
#define NU_VULKAN_IMAGE_VIEW_HPP

#include <memory>

#include "VulkanFunctions.hpp"

namespace nu
{
namespace Vulkan
{

class Device;
class Image;
class ImageView
{
	public:
		typedef std::unique_ptr<ImageView> Ptr;

		~ImageView();

		VkImageViewType getViewType() const;
		VkFormat getFormat() const;
		VkImageAspectFlags getAspect() const;

		Image& getImage();
		const Image& getImage() const;
		Device& getDevice();
		const Device& getDevice() const;
		const VkImageView& getHandle() const;
		const VkImage& getImageHandle() const;
		const VkDevice& getDeviceHandle() const;

	private:
		friend class Image;
		static ImageView::Ptr createImageView(Image& image, VkImageViewType viewType, VkFormat format, VkImageAspectFlags aspect);

		ImageView(Image& image, VkImage imageHandleFromSwapchain, VkImageViewType viewType, VkFormat format, VkImageAspectFlags aspect);

		bool init();
		void release();

		Device& mDevice;
		Image& mImage;
		VkImageView mImageView;

		VkImageViewType mViewType;
		VkFormat mFormat;
		VkImageAspectFlags mAspect;
};

} // namespace Vulkan
} // namespace nu

#endif // NU_VULKAN_IMAGE_VIEW_HPP