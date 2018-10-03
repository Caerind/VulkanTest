#include "VulkanImage.hpp"

#include "VulkanDevice.hpp"

namespace nu
{
namespace Vulkan
{

Image::Ptr Image::createImage(Device& device, VkImageType type, VkFormat format, VkExtent3D size, uint32_t numMipmaps, uint32_t numLayers, VkSampleCountFlagBits samples, VkImageUsageFlags usageScenarios, bool cubemap)
{
	Image::Ptr image(new Image(device, type, format, size, numMipmaps, numLayers, samples, usageScenarios, cubemap));
	if (image != nullptr)
	{
		if (!image->init())
		{
			image.reset();
		}
	}
	return image;
}

Image::~Image()
{
	ObjectTracker::unregisterObject(ObjectType_Image);

	release();
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

MemoryBlock::Ptr Image::allocateAndBindMemoryBlock(VkMemoryPropertyFlagBits memoryProperties)
{
	return MemoryBlock::createMemoryBlock(this, memoryProperties);
}

ImageView::Ptr Image::createImageView(VkImageViewType viewType, VkFormat format, VkImageAspectFlags aspect)
{
	return ImageView::createImageView(mDevice, *this, viewType, format, aspect);
}

bool Image::isInitialized() const 
{
	return mImage != VK_NULL_HANDLE;
}

const VkImage& Image::getHandle() const
{
	return mImage;
}

const VkDevice& Image::getDeviceHandle() const
{
	return mDevice.getHandle();
}

Device& Image::getDevice()
{
	return mDevice;
}

const Device& Image::getDevice() const
{
	return mDevice;
}

Image::Image(Device& device, VkImageType type, VkFormat format, VkExtent3D size, uint32_t numMipmaps, uint32_t numLayers, VkSampleCountFlagBits samples, VkImageUsageFlags usageScenarios, bool cubemap)
	: mDevice(device)
	, mImage(VK_NULL_HANDLE)
	, mType(type)
	, mFormat(format)
	, mSize(size)
	, mNumMipmaps(numMipmaps)
	, mNumLayers(numLayers)
	, mSamples(samples)
	, mUsageScenarios(usageScenarios)
	, mCubemap(cubemap)
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
		mUsageScenarios,                                    // VkImageUsageFlags        usage
		VK_SHARING_MODE_EXCLUSIVE,                          // VkSharingMode            sharingMode
		0,                                                  // uint32_t                 queueFamilyIndexCount
		nullptr,                                            // const uint32_t         * pQueueFamilyIndices
		VK_IMAGE_LAYOUT_UNDEFINED                           // VkImageLayout            initialLayout
	};

	VkResult result = vkCreateImage(mDevice.getHandle(), &imageCreateInfo, nullptr, &mImage);
	if (result != VK_SUCCESS || mImage == VK_NULL_HANDLE)
	{
		// TODO : Use Numea System Log
		printf("Could not create an image\n");
		return false;
	}
	return true;
}

bool Image::release()
{
	if (mImage != VK_NULL_HANDLE)
	{
		vkDestroyImage(mDevice.getHandle(), mImage, nullptr);
		mImage = VK_NULL_HANDLE;
		return true;
	}
	return false;
}

} // namespace Vulkan
} // namespace nu