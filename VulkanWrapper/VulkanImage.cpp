#include "VulkanImage.hpp"

#include "VulkanDevice.hpp"
#include "VulkanMemoryBlock.hpp"
#include "VulkanImageView.hpp"

VULKAN_NAMESPACE_BEGIN

VulkanImage::~VulkanImage()
{
	if (!isSwapchainImage())
	{
		release();
	}

	VULKAN_OBJECTTRACKER_UNREGISTER();
}

const VkMemoryRequirements& VulkanImage::getMemoryRequirements() const
{
	if (!mMemoryRequirementsQueried)
	{
		mMemoryRequirementsQueried = true;
		vkGetImageMemoryRequirements(getDeviceHandle(), mImage, &mMemoryRequirements);
	}

	return mMemoryRequirements;
}

VulkanMemoryBlock* VulkanImage::allocateMemoryBlock(VkMemoryPropertyFlagBits memoryProperties)
{
	mOwnedMemoryBlock = getDevice().createMemoryBlock(getMemoryRequirements(), memoryProperties);
	if (mOwnedMemoryBlock != nullptr)
	{
		mOwnedMemoryBlock->bind(this, 0);
	}
	else
	{
		mMemoryBlock = nullptr;
	}
	return mMemoryBlock;
}

bool VulkanImage::ownMemoryBlock() const
{
	return mOwnedMemoryBlock != nullptr;
}

bool VulkanImage::isBoundToMemoryBlock() const
{
	return mMemoryBlock != nullptr;
}

VulkanMemoryBlock* VulkanImage::getMemoryBlock()
{
	return mMemoryBlock;
}

VkDeviceSize VulkanImage::getOffsetInMemoryBlock() const
{
	return mOffsetInMemoryBlock;
}

VulkanImageView* VulkanImage::createImageView(VkImageViewType viewType, VkFormat format, VkImageAspectFlags aspect)
{
	VulkanImageViewPtr imageView = VulkanImageView::createImageView(*this, viewType, format, aspect);
	if (imageView != nullptr)
	{
		mImageViews.emplace_back(std::move(imageView));
		return mImageViews.back().get();
	}
	else
	{
		return nullptr;
	}
}

VulkanImageView* VulkanImage::getImageView(VulkanU32 index)
{
	VULKAN_ASSERT(index < getImageViewCount());
	return mImageViews[index].get();
}

const VulkanImageView* VulkanImage::getImageView(VulkanU32 index) const
{
	VULKAN_ASSERT(index < getImageViewCount());
	return mImageViews[index].get();
}

VulkanU32 VulkanImage::getImageViewCount() const
{
	return (VulkanU32)mImageViews.size();
}

void VulkanImage::clearImageViews()
{
	mImageViews.clear();
}

VkImageType VulkanImage::getType() const
{
	return mType;
}

VkFormat VulkanImage::getFormat() const
{
	return mFormat;
}

VkExtent3D VulkanImage::getSize() const
{
	return mSize;
}

VulkanU32 VulkanImage::getNumMipmaps() const
{
	return mNumMipmaps;
}

VulkanU32 VulkanImage::getNumLayers() const
{
	return mNumLayers;
}

VkSampleCountFlagBits VulkanImage::getSamples() const
{
	return mSamples;
}

VkImageUsageFlags VulkanImage::getUsage() const
{
	return mUsage;
}

bool VulkanImage::isCubemap() const
{
	return mCubemap;
}

bool VulkanImage::isSwapchainImage() const
{
	return mIsSwapchainImage;
}

const VkImage& VulkanImage::getHandle() const
{
	return mImage;
}

VulkanImagePtr VulkanImage::createImage(VkImageType type, VkFormat format, VkExtent3D size, VulkanU32 numMipmaps, VulkanU32 numLayers, VkSampleCountFlagBits samples, VkImageUsageFlags usage, bool cubemap)
{
	VulkanImagePtr image(new VulkanImage(type, format, size, numMipmaps, numLayers, samples, usage, cubemap));
	if (image != nullptr)
	{
		if (!image->init())
		{
			image.reset();
		}
	}
	return image;
}

VulkanImagePtr VulkanImage::createImageFromSwapchain(VulkanDevice& device, VkImage swapchainImageHandle, VkImageType type, VkFormat format, VkExtent3D size, VulkanU32 numMipmaps, VulkanU32 numLayers, VkSampleCountFlagBits samples, VkImageUsageFlags usage, bool cubemap)
{
	VulkanImagePtr image(new VulkanImage(type, format, size, numMipmaps, numLayers, samples, usage, cubemap));
	if (image != nullptr)
	{
		image->mImage = swapchainImageHandle;
		image->mIsSwapchainImage = true;
	}
	return image;
}

VulkanImage::VulkanImage(VkImageType type, VkFormat format, VkExtent3D size, VulkanU32 numMipmaps, VulkanU32 numLayers, VkSampleCountFlagBits samples, VkImageUsageFlags usage, bool cubemap)
	: mImage(VK_NULL_HANDLE)
	, mOwnedMemoryBlock(nullptr)
	, mMemoryBlock(nullptr)
	, mOffsetInMemoryBlock(0)
	, mImageViews()
	, mType(type)
	, mFormat(format)
	, mSize(size)
	, mNumMipmaps(numMipmaps)
	, mNumLayers(numLayers)
	, mSamples(samples)
	, mUsage(usage)
	, mCubemap(cubemap)
	, mIsSwapchainImage(false)
	, mMemoryRequirementsQueried(false)
	, mMemoryRequirements()
{
	VULKAN_OBJECTTRACKER_REGISTER();
}

bool VulkanImage::init()
{
	VkImageCreateInfo imageCreateInfo = {
		VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO,                // VkStructureType          sType
		nullptr,                                            // const void             * pNext
		mCubemap ? VK_IMAGE_CREATE_CUBE_COMPATIBLE_BIT : 0u,// VkImageCreateFlags       flags
		mType,                                              // VkImageType              imageType
		mFormat,                                            // VkFormat                 format
		mSize,                                              // VkExtent3D               extent
		mNumMipmaps,                                        // VulkanU32                 mipLevels
		mCubemap ? 6 * mNumLayers : mNumLayers,             // VulkanU32                 arrayLayers
		mSamples,                                           // VkSampleCountFlagBits    samples
		VK_IMAGE_TILING_OPTIMAL,                            // VkImageTiling            tiling
		mUsage,                                             // VkImageUsageFlags        usage
		VK_SHARING_MODE_EXCLUSIVE,                          // VkSharingMode            sharingMode
		0,                                                  // VulkanU32                 queueFamilyIndexCount
		nullptr,                                            // const VulkanU32         * pQueueFamilyIndices
		VK_IMAGE_LAYOUT_UNDEFINED                           // VkImageLayout            initialLayout
	};
	// TODO : initial image layout ?

	VkResult result = vkCreateImage(getDeviceHandle(), &imageCreateInfo, nullptr, &mImage);
	if (result != VK_SUCCESS || mImage == VK_NULL_HANDLE)
	{
		VULKAN_LOG_ERROR("Could not create an image");
		return false;
	}
	return true;
}

void VulkanImage::release()
{
	if (mImage != VK_NULL_HANDLE)
	{
		vkDestroyImage(getDeviceHandle(), mImage, nullptr);
		mImage = VK_NULL_HANDLE;
	}
}

void VulkanImage::bindToMemoryBlock(VulkanMemoryBlock* memoryBlock, VkDeviceSize offsetInMemoryBlock)
{
	mMemoryBlock = memoryBlock;
	mOffsetInMemoryBlock = offsetInMemoryBlock;
}

VULKAN_NAMESPACE_END