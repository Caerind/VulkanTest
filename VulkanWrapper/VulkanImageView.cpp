#include "VulkanImage.hpp"

#include "VulkanDevice.hpp"
#include "VulkanImageView.hpp"

VULKAN_NAMESPACE_BEGIN

VulkanImageView::~VulkanImageView()
{
	release();

	VULKAN_OBJECTTRACKER_UNREGISTER();
}

VkImageViewType VulkanImageView::getViewType() const
{
	return mViewType;
}

VkFormat VulkanImageView::getFormat() const
{
	return mFormat;
}

VkImageAspectFlags VulkanImageView::getAspect() const
{
	return mAspect;
}

VulkanImage& VulkanImageView::getImage()
{
	return mImage;
}

const VulkanImage& VulkanImageView::getImage() const
{
	return mImage;
}

const VkImageView& VulkanImageView::getHandle() const
{
	return mImageView;
}

const VkImage& VulkanImageView::getImageHandle() const
{
	return mImage.getHandle();
}

VulkanImageViewPtr VulkanImageView::createImageView(VulkanImage& image, VkImageViewType viewType, VkFormat format, VkImageAspectFlags aspect)
{
	VulkanImageViewPtr imageView(new VulkanImageView(image, VK_NULL_HANDLE, viewType, format, aspect));
	if (imageView != nullptr)
	{
		if (!imageView->init())
		{
			imageView.reset();
		}
	}
	return imageView;
}

VulkanImageView::VulkanImageView(VulkanImage& image, VkImage imageHandleFromSwapchain, VkImageViewType viewType, VkFormat format, VkImageAspectFlags aspect)
	: mImage(image)
	, mImageView(VK_NULL_HANDLE)
	, mViewType(viewType)
	, mFormat(format)
	, mAspect(aspect)
{
	VULKAN_OBJECTTRACKER_REGISTER();
}

bool VulkanImageView::init()
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

	VkResult result = vkCreateImageView(mImage.getDeviceHandle(), &imageViewCreateInfo, nullptr, &mImageView);
	if (result != VK_SUCCESS || mImageView == VK_NULL_HANDLE) 
	{
		// TODO : Use Numea Log System
		printf("Could not create image view\n");
		return false;
	}
	return true;
}

void VulkanImageView::release()
{
	if (mImageView != VK_NULL_HANDLE)
	{
		vkDestroyImageView(mImage.getDeviceHandle(), mImageView, nullptr);
		mImageView = VK_NULL_HANDLE;
	}
}

VULKAN_NAMESPACE_END