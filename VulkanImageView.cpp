#include "VulkanImage.hpp"

#include "VulkanDevice.hpp"
#include "VulkanImageView.hpp"

namespace nu
{
namespace Vulkan
{

ImageView::Ptr ImageView::createImageView(Device& device, Image& image, VkImageViewType viewType, VkFormat format, VkImageAspectFlags aspect)
{
	ImageView::Ptr imageView(new ImageView(device, &image, VK_NULL_HANDLE, viewType, format, aspect));
	if (imageView != nullptr)
	{
		if (!imageView->init())
		{
			imageView.reset();
		}
	}
	return imageView;
}

ImageView::~ImageView()
{
	ObjectTracker::unregisterObject(ObjectType_ImageView);

	release();
}

bool ImageView::isInitialized() const
{
	return mImageView != VK_NULL_HANDLE;
}

const VkImageView& ImageView::getHandle() const
{
	return mImageView;
}

const VkImage& ImageView::getImageHandle() const
{
	if (mImage != nullptr)
	{
		return mImage->getHandle();
	}
	else
	{
		return mImageHandleFromSwapchain;
	}
}

const VkDevice& ImageView::getDeviceHandle() const
{
	return mDevice.getHandle();
}

ImageView::ImageView(Device& device, Image* image, VkImage imageHandleFromSwapchain, VkImageViewType viewType, VkFormat format, VkImageAspectFlags aspect)
	: mDevice(device)
	, mImage(image)
	, mImageView(VK_NULL_HANDLE)
	, mViewType(viewType)
	, mFormat(format)
	, mAspect(aspect)
	, mImageHandleFromSwapchain(imageHandleFromSwapchain)
{
	ObjectTracker::registerObject(ObjectType_ImageView);
}

bool ImageView::init()
{
	VkImageViewCreateInfo imageViewCreateInfo = {
		VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO,   // VkStructureType            sType
		nullptr,                                    // const void               * pNext
		0,                                          // VkImageViewCreateFlags     flags
		getImageHandle(),                           // VkImage                    image
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

	VkResult result = vkCreateImageView(mDevice.getHandle(), &imageViewCreateInfo, nullptr, &mImageView);
	if (result != VK_SUCCESS || mImageView == VK_NULL_HANDLE) 
	{
		// TODO : Use Numea Log System
		printf("Could not create image view\n");
		return false;
	}
	return true;
}

bool ImageView::release()
{
	if (mImageView != VK_NULL_HANDLE)
	{
		vkDestroyImageView(mDevice.getHandle(), mImageView, nullptr);
		mImageView = VK_NULL_HANDLE;
		return true;
	}
	return false;
}

ImageView::Ptr ImageView::createImageViewFromSwapchain(Device& device, VkImage imageHandleFromSwapchain, VkImageViewType viewType, VkFormat format, VkImageAspectFlags aspect)
{
	ImageView::Ptr imageView(new ImageView(device, nullptr, imageHandleFromSwapchain, viewType, format, aspect));
	if (imageView != nullptr)
	{
		if (!imageView->init())
		{
			imageView.reset();
		}
	}
	return imageView;
}

} // namespace Vulkan
} // namespace nu