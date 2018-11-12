#include "VulkanImage.hpp"

#include "VulkanDevice.hpp"

namespace nu
{
namespace Vulkan
{

Image::~Image()
{
	if (!isSwapchainImage())
	{
		release();
	}

	ObjectTracker::unregisterObject(ObjectType_Image);
}

const VkMemoryRequirements& Image::getMemoryRequirements() const
{
	if (!mMemoryRequirementsQueried)
	{
		mMemoryRequirementsQueried = true;
		vkGetImageMemoryRequirements(mDevice.getHandle(), mImage, &mMemoryRequirements);
	}

	return mMemoryRequirements;
}

MemoryBlock* Image::allocateMemoryBlock(VkMemoryPropertyFlagBits memoryProperties)
{
	mOwnedMemoryBlock = mDevice.createMemoryBlock(getMemoryRequirements(), memoryProperties);
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

bool Image::ownMemoryBlock() const
{
	return mOwnedMemoryBlock != nullptr;
}

bool Image::isBoundToMemoryBlock() const
{
	return mMemoryBlock != nullptr;
}

MemoryBlock* Image::getMemoryBlock()
{
	return mMemoryBlock;
}

VkDeviceSize Image::getOffsetInMemoryBlock() const
{
	return mOffsetInMemoryBlock;
}

ImageView* Image::createImageView(VkImageViewType viewType, VkFormat format, VkImageAspectFlags aspect)
{
	ImageView::Ptr imageView = ImageView::createImageView(*this, viewType, format, aspect);
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

ImageView* Image::getImageView(uint32_t index)
{
	assert(index < getImageViewCount());
	return mImageViews[index].get();
}

const ImageView* Image::getImageView(uint32_t index) const
{
	assert(index < getImageViewCount());
	return mImageViews[index].get();
}

uint32_t Image::getImageViewCount() const
{
	return (uint32_t)mImageViews.size();
}

void Image::clearImageViews()
{
	mImageViews.clear();
}

VkImageType Image::getType() const
{
	return mType;
}

VkFormat Image::getFormat() const
{
	return mFormat;
}

VkExtent3D Image::getSize() const
{
	return mSize;
}

uint32_t Image::getNumMipmaps() const
{
	return mNumMipmaps;
}

uint32_t Image::getNumLayers() const
{
	return mNumLayers;
}

VkSampleCountFlagBits Image::getSamples() const
{
	return mSamples;
}

VkImageUsageFlags Image::getUsage() const
{
	return mUsage;
}

bool Image::isCubemap() const
{
	return mCubemap;
}

bool Image::isSwapchainImage() const
{
	return mIsSwapchainImage;
}

Device& Image::getDevice()
{
	return mDevice;
}

const Device& Image::getDevice() const
{
	return mDevice;
}

const VkImage& Image::getHandle() const
{
	return mImage;
}

const VkDevice& Image::getDeviceHandle() const
{
	return mDevice.getHandle();
}

Image::Ptr Image::createImage(Device& device, VkImageType type, VkFormat format, VkExtent3D size, uint32_t numMipmaps, uint32_t numLayers, VkSampleCountFlagBits samples, VkImageUsageFlags usage, bool cubemap)
{
	Image::Ptr image(new Image(device, type, format, size, numMipmaps, numLayers, samples, usage, cubemap));
	if (image != nullptr)
	{
		if (!image->init())
		{
			image.reset();
		}
	}
	return image;
}

Image::Ptr Image::createImageFromSwapchain(Device& device, VkImage swapchainImageHandle, VkImageType type, VkFormat format, VkExtent3D size, uint32_t numMipmaps, uint32_t numLayers, VkSampleCountFlagBits samples, VkImageUsageFlags usage, bool cubemap)
{
	Image::Ptr image(new Image(device, type, format, size, numMipmaps, numLayers, samples, usage, cubemap));
	if (image != nullptr)
	{
		image->mImage = swapchainImageHandle;
		image->mIsSwapchainImage = true;
	}
	return image;
}

Image::Image(Device& device, VkImageType type, VkFormat format, VkExtent3D size, uint32_t numMipmaps, uint32_t numLayers, VkSampleCountFlagBits samples, VkImageUsageFlags usage, bool cubemap)
	: mDevice(device)
	, mImage(VK_NULL_HANDLE)
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
	ObjectTracker::registerObject(ObjectType_Image);
}

bool Image::init()
{
	VkImageCreateInfo imageCreateInfo = {
		VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO,                // VkStructureType          sType
		nullptr,                                            // const void             * pNext
		mCubemap ? VK_IMAGE_CREATE_CUBE_COMPATIBLE_BIT : 0u,// VkImageCreateFlags       flags
		mType,                                              // VkImageType              imageType
		mFormat,                                            // VkFormat                 format
		mSize,                                              // VkExtent3D               extent
		mNumMipmaps,                                        // uint32_t                 mipLevels
		mCubemap ? 6 * mNumLayers : mNumLayers,             // uint32_t                 arrayLayers
		mSamples,                                           // VkSampleCountFlagBits    samples
		VK_IMAGE_TILING_OPTIMAL,                            // VkImageTiling            tiling
		mUsage,                                             // VkImageUsageFlags        usage
		VK_SHARING_MODE_EXCLUSIVE,                          // VkSharingMode            sharingMode
		0,                                                  // uint32_t                 queueFamilyIndexCount
		nullptr,                                            // const uint32_t         * pQueueFamilyIndices
		VK_IMAGE_LAYOUT_UNDEFINED                           // VkImageLayout            initialLayout
	};
	// TODO : initial image layout ?

	VkResult result = vkCreateImage(mDevice.getHandle(), &imageCreateInfo, nullptr, &mImage);
	if (result != VK_SUCCESS || mImage == VK_NULL_HANDLE)
	{
		// TODO : Use Numea System Log
		printf("Could not create an image\n");
		return false;
	}
	return true;
}

void Image::release()
{
	if (mImage != VK_NULL_HANDLE)
	{
		vkDestroyImage(mDevice.getHandle(), mImage, nullptr);
		mImage = VK_NULL_HANDLE;
	}
}

void Image::bindToMemoryBlock(MemoryBlock* memoryBlock, VkDeviceSize offsetInMemoryBlock)
{
	mMemoryBlock = memoryBlock;
	mOffsetInMemoryBlock = offsetInMemoryBlock;
}

} // namespace Vulkan
} // namespace nu