#include "VulkanImage.hpp"

#include "VulkanDevice.hpp"
#include "VulkanImageView.hpp"

namespace nu
{
namespace Vulkan
{

ImageView::~ImageView()
{
	release();

	ObjectTracker::unregisterObject(ObjectType_ImageView);
}

VkImageViewType ImageView::getViewType() const
{
	return mViewType;
}

VkFormat ImageView::getFormat() const
{
	return mFormat;
}

VkImageAspectFlags ImageView::getAspect() const
{
	return mAspect;
}

Image& ImageView::getImage()
{
	return mImage;
}

const Image& ImageView::getImage() const
{
	return mImage;
}

Device& ImageView::getDevice()
{
	return mDevice;
}

const Device& ImageView::getDevice() const
{
	return mDevice;
}

const VkImageView& ImageView::getHandle() const
{
	return mImageView;
}

const VkImage& ImageView::getImageHandle() const
{
	return mImage.getHandle();
}

const VkDevice& ImageView::getDeviceHandle() const
{
	return mDevice.getHandle();
}

ImageView::Ptr ImageView::createImageView(Image& image, VkImageViewType viewType, VkFormat format, VkImageAspectFlags aspect)
{
	ImageView::Ptr imageView(new ImageView(image, VK_NULL_HANDLE, viewType, format, aspect));
	if (imageView != nullptr)
	{
		if (!imageView->init())
		{
			imageView.reset();
		}
	}
	return imageView;
}

ImageView::ImageView(Image& image, VkImage imageHandleFromSwapchain, VkImageViewType viewType, VkFormat format, VkImageAspectFlags aspect)
	: mDevice(image.getDevice())
	, mImage(image)
	, mImageView(VK_NULL_HANDLE)
	, mViewType(viewType)
	, mFormat(format)
	, mAspect(aspect)
{
	ObjectTracker::registerObject(ObjectType_ImageView);
}

bool ImageView::init()
{
	VkImageViewCreateInfo imageViewCreateInfo = {
		VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO,   // VkStructureType            sType
		nullptr,                                    // const void               * pNext
		0,                                          // VkImageViewCreateFlags     flags
		mImage.getHandle(),                         // VkImage                    image
		mViewType,                                  // VkImageViewType            viewType
		mFormat,                                    // VkFormat                   format
		{                                           // VkComponentMapping         components
			VK_COMPONENT_SWIZZLE_IDENTITY,              // VkComponentSwizzle         r
			VK_COMPONENT_SWIZZLE_IDENTITY,              // VkComponentSwizzle         g
			VK_COMPONENT_SWIZZLE_IDENTITY,              // VkComponentSwizzle         b
			VK_COMPONENT_SWIZZLE_IDENTITY               // VkComponentSwizzle         a
		},
		{                                           // VkImageSubresourceRange    subresourceRange
			mAspect,                                    // VkImageAspectFlags         aspectMask
			0,                                          // uint32_t                   baseMipLevel
			VK_REMAINING_MIP_LEVELS,                    // uint32_t                   levelCount
			0,                                          // uint32_t                   baseArrayLayer
			VK_REMAINING_ARRAY_LAYERS                   // uint32_t                   layerCount
		}
	};
	// TODO : Swizzle components ?
	// TODO : baseMipLevel ?
	// TODO : levelCount ?
	// TODO : baseArrayLayer ?
	// TODO : layerCount ?

	VkResult result = vkCreateImageView(mDevice.getHandle(), &imageViewCreateInfo, nullptr, &mImageView);
	if (result != VK_SUCCESS || mImageView == VK_NULL_HANDLE) 
	{
		// TODO : Use Numea Log System
		printf("Could not create image view\n");
		return false;
	}
	return true;
}

void ImageView::release()
{
	if (mImageView != VK_NULL_HANDLE)
	{
		vkDestroyImageView(mDevice.getHandle(), mImageView, nullptr);
		mImageView = VK_NULL_HANDLE;
	}
}

} // namespace Vulkan
} // namespace nu