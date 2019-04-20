#include "VulkanSwapchain.hpp"

#include "../System/CallOnExit.hpp"

#include "VulkanDevice.hpp"
#include "VulkanImage.hpp"
#include "VulkanImageView.hpp"
#include "VulkanSurface.hpp"
#include "VulkanSampler.hpp"

VULKAN_NAMESPACE_BEGIN

VulkanSwapchain::~VulkanSwapchain()
{
	release();

	VULKAN_OBJECTTRACKER_UNREGISTER();
}

bool VulkanSwapchain::recreate(std::vector<FrameResources>& framesResources)
{
	// TODO : Check if re-init is find
	// TODO : No need to re-select every parameter if already choosen and valid
	return init(framesResources);
}

const VkFormat& VulkanSwapchain::getFormat() const
{
	return mFormat;
}

const VkColorSpaceKHR& VulkanSwapchain::getColorSpace() const
{
	return mColorSpace;
}

const VkExtent2D& VulkanSwapchain::getSize() const
{
	return mSize;
}

const VkFormat& VulkanSwapchain::getDepthFormat() const
{
	return mDepthFormat;
}

bool VulkanSwapchain::acquireImageIndex(uint64_t timeout, VkSemaphore semaphore, VkFence fence, uint32_t& imageIndex)
{
	VkResult result = vkAcquireNextImageKHR(mDevice.getHandle(), mSwapchain, timeout, semaphore, fence, &imageIndex);
	switch (result)
	{
		case VK_SUCCESS:
		case VK_SUBOPTIMAL_KHR:
			return true;
		default:
			return false;
	}
}

VkImage VulkanSwapchain::getImageHandle(uint32_t index) const
{
	uint32_t size = (uint32_t)mSwapchainImages.size();
	if (index < size && mSwapchainImages[index] != nullptr)
	{
		return mSwapchainImages[index]->getHandle();
	}
	return VK_NULL_HANDLE;
}

VkImageView VulkanSwapchain::getImageViewHandle(uint32_t index) const
{
	uint32_t size = (uint32_t)mSwapchainImages.size();
	if (index < size && mSwapchainImages[index] != nullptr && mSwapchainImages[index]->getImageViewCount() == 1)
	{
		return mSwapchainImages[index]->getImageView(0)->getHandle();
	}
	return VK_NULL_HANDLE;
}

VulkanSurface& VulkanSwapchain::getSurface()
{
	return mSurface;
}

const VulkanSurface& VulkanSwapchain::getSurface() const
{
	return mSurface;
}

VulkanDevice& VulkanSwapchain::getDevice()
{
	return mDevice;
}

const VulkanDevice& VulkanSwapchain::getDevice() const
{
	return mDevice;
}

const VkSwapchainKHR& VulkanSwapchain::getHandle() const
{
	return mSwapchain;
}

const VkSurfaceKHR& VulkanSwapchain::getSurfaceHandle() const
{
	return mSurface.getHandle();
}

const VkDevice& VulkanSwapchain::getDeviceHandle() const
{
	return mDevice.getHandle();
}

VulkanSwapchainPtr VulkanSwapchain::createSwapchain(VulkanDevice& device, VulkanSurface& surface, std::vector<FrameResources>& framesResources)
{
	VulkanSwapchainPtr swapchain(new VulkanSwapchain(device, surface));
	if (swapchain != nullptr)
	{
		if (!swapchain->init(framesResources))
		{
			swapchain.reset();
		}
	}
	return swapchain;
}

VulkanSwapchain::VulkanSwapchain(VulkanDevice& device, VulkanSurface& surface)
	: mDevice(device)
	, mSurface(surface)
	, mSwapchain(VK_NULL_HANDLE)
	, mReady(false)
{
	// TODO : Default values for each variables

	mDepthFormat = VK_FORMAT_D16_UNORM; // TODO : Unhardcode

	VULKAN_OBJECTTRACKER_REGISTER();
}

bool VulkanSwapchain::init(std::vector<FrameResources>& framesResources)
{
	// TODO : Do we really need to wait ?
	// TODO : Maybe only for recreate ?
	// TODO : Even not sure for the first todo question...
	mDevice.waitForAllSubmittedCommands();

	mReady = false;

	mSwapchainImages.clear();

	VkSwapchainKHR oldSwapchain = mSwapchain;
	mSwapchain = VK_NULL_HANDLE;

	// We will need to destroy the old swapchain
	// This is used to avoid forgetting about it and duplication of code
	// TODO : VulkanCallOnExit
	nu::CallOnExit oldSwapchainDestroyer([&]()
	{
		if (oldSwapchain != VK_NULL_HANDLE)
		{
			vkDestroySwapchainKHR(mDevice.getHandle(), oldSwapchain, nullptr);
			oldSwapchain = VK_NULL_HANDLE;
		}
	});

	VkPresentModeKHR desiredPresentMode = VK_PRESENT_MODE_MAILBOX_KHR; // TODO : Unhardcode
	if (!selectPresentMode(desiredPresentMode))
	{
		return false; // oldSwapchainDestroyer.call() will happend here
	}

	if (!queryCapabilities())
	{
		return false; // oldSwapchainDestroyer.call() will happend here
	}

	uint32_t desiredImageCount = 3; // TODO : Unhardcode
	if (!selectImageCount(desiredImageCount))
	{
		return false; // oldSwapchainDestroyer.call() will happend here
	}

	if (!querySize())
	{
		return false; // oldSwapchainDestroyer.call() will happend here
	}

	VkImageUsageFlags desiredSwapchainImageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT; // TODO : Unhardcode
	if (!selectImageUsage(desiredSwapchainImageUsage))
	{
		return false; // oldSwapchainDestroyer.call() will happend here
	}

	VkSurfaceTransformFlagBitsKHR desiredSurfaceTransform = VK_SURFACE_TRANSFORM_IDENTITY_BIT_KHR; // TODO : Unhardcode
	if (!selectSurfaceTransform(desiredSurfaceTransform))
	{
		return false; // oldSwapchainDestroyer.call() will happend here
	}

	// TODO : Find what's wrong with this desiredSurfaceFormat
	VkSurfaceFormatKHR desiredSurfaceFormat = { VK_FORMAT_R8G8B8A8_UNORM, VK_COLOR_SPACE_SRGB_NONLINEAR_KHR }; // TODO : Unhardcode
	if (!selectSurfaceFormat(desiredSurfaceFormat))
	{
		return false; // oldSwapchainDestroyer.call() will happend here
	}

	VkSwapchainCreateInfoKHR swapchainCreateInfo = {
		VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR,  // VkStructureType                  sType
		nullptr,                                      // const void                     * pNext
		0,                                            // VkSwapchainCreateFlagsKHR        flags
		mSurface.getHandle(),                         // VkSurfaceKHR                     surface
		mImageCount,                                  // uint32_t                         minImageCount
		mFormat,                                      // VkFormat                         imageFormat
		mColorSpace,                                  // VkColorSpaceKHR                  imageColorSpace
		mSize,                                        // VkExtent2D                       imageExtent
		1,                                            // uint32_t                         imageArrayLayers
		mImageUsage,                                  // VkImageUsageFlags                imageUsage
		VK_SHARING_MODE_EXCLUSIVE,                    // VkSharingMode                    imageSharingMode
		0,                                            // uint32_t                         queueFamilyIndexCount
		nullptr,                                      // const uint32_t                 * pQueueFamilyIndices
		mSurfaceTransform,                            // VkSurfaceTransformFlagBitsKHR    preTransform
		VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR,            // VkCompositeAlphaFlagBitsKHR      compositeAlpha
		mPresentMode,                                 // VkPresentModeKHR                 presentMode
		VK_TRUE,                                      // VkBool32                         clipped
		oldSwapchain                                  // VkSwapchainKHR                   oldSwapchain
	};

	VkResult result = vkCreateSwapchainKHR(mDevice.getHandle(), &swapchainCreateInfo, nullptr, &mSwapchain);
	if (result != VK_SUCCESS || mSwapchain == VK_NULL_HANDLE)
	{
		// TODO : Use Numea System Log
		printf("Could not create a swapchain\n");
		return false; // oldSwapchainDestroyer.call() will happend here
	}

	// Don't forget to destroy the old swapchain if we arrive here
	oldSwapchainDestroyer.callAndReset();

	std::vector<VkImage> swapchainImageHandles;
	if (!queryImageHandles(swapchainImageHandles))
	{
		return false;
	}

	for (size_t i = 0; i < swapchainImageHandles.size(); i++)
	{
		// TODO : Move this parameters
		VkImageType imageType = VK_IMAGE_TYPE_2D;
		VkFormat imageFormat = mFormat;
		VkExtent3D imageSize = { mSize.width, mSize.height, 1 };
		uint32_t imageNumMipmaps = 1; // TODO : Is this correct ?
		uint32_t imageNumLayers = 1; // 1 for non-stereoscopic-3D applications // TODO : Pass this as parameters ?
		VkSampleCountFlagBits imageSamples = VK_SAMPLE_COUNT_1_BIT;
		VkImageUsageFlags imageUsage = mImageUsage;
		bool imageCubemap = false;
		// TODO : And maybe the initial layout, which might not be (isn't?) undefined

		// Create an image from swapchain image
		mSwapchainImages.emplace_back(VulkanImage::createImageFromSwapchain(mDevice, swapchainImageHandles[i], imageType, imageFormat, imageSize, imageNumMipmaps, imageNumLayers, imageSamples, imageUsage, imageCubemap));
		if (mSwapchainImages.back() == nullptr)
		{
			return false;
		}

		// Create an image view which will be used later by getImageViewHandle(...)
		VulkanImageView* imageView = mSwapchainImages.back()->createImageView(VK_IMAGE_VIEW_TYPE_2D, mFormat, VK_IMAGE_ASPECT_COLOR_BIT);
		if (imageView == nullptr)
		{
			return false;
		}
	}

	mSwapchainDepthImages.clear();

	// TODO : Only if wanted
	if (true) // Use depth
	{
		// TODO : Use Image Count ?
		// TODO : Where is the frame count set for frameresources ?
		const uint32_t framesCount = 3; // TODO : Unhardcode
		for (uint32_t i = 0; i < framesCount; i++)
		{
			VulkanImageHelperPtr depthImage = VulkanImageHelper::createImage2DAndView(mDevice, mDepthFormat, mSize, 1, 1, VK_SAMPLE_COUNT_1_BIT, VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT, VK_IMAGE_ASPECT_DEPTH_BIT);
			if (depthImage == nullptr || depthImage->getImage() == nullptr || depthImage->getMemoryBlock() == nullptr || depthImage->getImageView() == nullptr)
			{
				return false;
			}

			framesResources[i].mDepthAttachment = depthImage->getImageView();

			mSwapchainDepthImages.emplace_back(std::move(depthImage));
		}
	}

	mReady = true;

	return true;
}

void VulkanSwapchain::release()
{
	if (mSwapchain != VK_NULL_HANDLE)
	{
		vkDestroySwapchainKHR(mDevice.getHandle(), mSwapchain, nullptr);
		mSwapchain = VK_NULL_HANDLE;
	}
}

bool VulkanSwapchain::selectPresentMode(VkPresentModeKHR desiredPresentMode)
{
	uint32_t presentModesCount = 0;
	VkResult result;

	result = vkGetPhysicalDeviceSurfacePresentModesKHR(mDevice.getPhysicalDeviceHandle(), mSurface.getHandle(), &presentModesCount, nullptr);
	if (result != VK_SUCCESS || presentModesCount == 0)
	{
		// TODO : Use Numea System Log
		printf("Could not get the number of supported present modes\n");
		return false;
	}

	std::vector<VkPresentModeKHR> presentModes(presentModesCount);
	result = vkGetPhysicalDeviceSurfacePresentModesKHR(mDevice.getPhysicalDeviceHandle(), mSurface.getHandle(), &presentModesCount, presentModes.data());
	if (result != VK_SUCCESS || presentModesCount == 0)
	{
		// TODO : Use Numea System Log
		printf("Could not enumerate present modes\n");
		return false;
	}

	// Select present mode
	for (auto& currentPresentMode : presentModes)
	{
		if (currentPresentMode == desiredPresentMode)
		{
			mPresentMode = desiredPresentMode;
			return true;
		}
	}

	// TODO : Use Numea System Log
	printf("Desired present mode is not supported. Selecting default FIFO mode\n");
	for (auto& currentPresentMode : presentModes)
	{
		if (currentPresentMode == VK_PRESENT_MODE_FIFO_KHR)
		{
			mPresentMode = VK_PRESENT_MODE_FIFO_KHR;
			return true;
		}
	}

	mPresentMode = VK_PRESENT_MODE_IMMEDIATE_KHR; // TODO : Is this correct ?

	// TODO : Use Numea System Log
	printf("VK_PRESENT_MODE_FIFO_KHR is not supported though it's mandatory for all drivers!\n");
	return false;
}

bool VulkanSwapchain::queryCapabilities()
{
	VkResult result = vkGetPhysicalDeviceSurfaceCapabilitiesKHR(mDevice.getPhysicalDeviceHandle(), mSurface.getHandle(), &mSurfaceCapabilities);
	if (result != VK_SUCCESS)
	{
		// TODO : Use Numea System Log
		printf("Could not get the capabilities of a presentation surface\n");
		return false;
	}
	return true;
}

bool VulkanSwapchain::selectImageCount(uint32_t desiredImageCount)
{
	// TODO : Use desired image count
	// TODO : Improve selection of number of images
	mImageCount = mSurfaceCapabilities.minImageCount + 1;
	if (mSurfaceCapabilities.maxImageCount > 0 && mImageCount > mSurfaceCapabilities.maxImageCount)
	{
		mImageCount = mSurfaceCapabilities.maxImageCount;
	}
	return true;
}

bool VulkanSwapchain::querySize()
{
	// On some platform extent works weird // TODO : Improve that comment
	if (mSurfaceCapabilities.currentExtent.width == 0xFFFFFFFF)
	{
		mSize = { 640, 480 };

		if (mSize.width < mSurfaceCapabilities.minImageExtent.width)
		{
			mSize.width = mSurfaceCapabilities.minImageExtent.width;
		}
		else if (mSize.width > mSurfaceCapabilities.maxImageExtent.width)
		{
			mSize.width = mSurfaceCapabilities.maxImageExtent.width;
		}

		if (mSize.height < mSurfaceCapabilities.minImageExtent.height)
		{
			mSize.height = mSurfaceCapabilities.minImageExtent.height;
		}
		else if (mSize.height > mSurfaceCapabilities.maxImageExtent.height)
		{
			mSize.height = mSurfaceCapabilities.maxImageExtent.height;
		}
	}
	else
	{
		mSize = mSurfaceCapabilities.currentExtent;
	}
	if (mSize.width == 0 || mSize.height == 0)
	{
		return false;
	}
	return true;
}

bool VulkanSwapchain::selectImageUsage(VkImageUsageFlags desiredImageUsage)
{
	mImageUsage = desiredImageUsage & mSurfaceCapabilities.supportedUsageFlags;
	if (mImageUsage != desiredImageUsage)
	{
		// TODO : Use Numea System Log
		printf("Could not create swapchain with that image usage\n");
		return false;
	}
	return true;
}

bool VulkanSwapchain::selectSurfaceTransform(VkSurfaceTransformFlagBitsKHR desiredSurfaceTransform)
{
	if (mSurfaceCapabilities.supportedTransforms & desiredSurfaceTransform)
	{
		mSurfaceTransform = desiredSurfaceTransform;
	}
	else
	{
		mSurfaceTransform = mSurfaceCapabilities.currentTransform;
	}
	return true;
}

bool VulkanSwapchain::selectSurfaceFormat(VkSurfaceFormatKHR desiredSurfaceFormat)
{
	uint32_t formatsCount = 0;

	VkResult result = vkGetPhysicalDeviceSurfaceFormatsKHR(mDevice.getPhysicalDeviceHandle(), mSurface.getHandle(), &formatsCount, nullptr);
	if (result != VK_SUCCESS || formatsCount == 0)
	{
		// TODO : Use Numea System Log
		printf("Could not get the number of supported surface formats\n");
		return false;
	}

	std::vector<VkSurfaceFormatKHR> surfaceFormats(formatsCount);
	result = vkGetPhysicalDeviceSurfaceFormatsKHR(mDevice.getPhysicalDeviceHandle(), mSurface.getHandle(), &formatsCount, surfaceFormats.data());
	if (result != VK_SUCCESS || formatsCount == 0 || surfaceFormats.size() == 0)
	{
		// TODO : Use Numea System Log
		printf("Could not enumerate supported surface formats\n");
		return false;
	}

	// TODO : Instead of choosing the nearest to what the user want, make the user directly choose the one he prefers
	// Moving the querying of surface formats at upper level

	// TODO : Is this useful ?
	if (surfaceFormats.size() == 1 && surfaceFormats[0].format == VK_FORMAT_UNDEFINED)
	{
		mFormat = desiredSurfaceFormat.format;
		mColorSpace = desiredSurfaceFormat.colorSpace;
		return true;
	}

	// Try to find desired surface format
	for (auto& surfaceFormat : surfaceFormats)
	{
		if (desiredSurfaceFormat.format == surfaceFormat.format && desiredSurfaceFormat.colorSpace == surfaceFormat.colorSpace)
		{
			mFormat = desiredSurfaceFormat.format;
			mColorSpace = desiredSurfaceFormat.colorSpace;
			return true;
		}
	}

	// Try to find desired format and any available color space with it
	for (auto& surfaceFormat : surfaceFormats)
	{
		if (desiredSurfaceFormat.format == surfaceFormat.format)
		{
			mFormat = desiredSurfaceFormat.format;
			mColorSpace = surfaceFormat.colorSpace;

			// TODO : Use Numea System Log
			printf("Desired combination of format and colorspace is not supported. Selecting other colorspace\n");
			return true;
		}
	}

	// Fallback scenario
	mFormat = surfaceFormats[0].format;
	mColorSpace = surfaceFormats[0].colorSpace;

	// TODO : I always get this error... WHY ?
	// TODO : What is the Desired Surface Format I want, and do I really don't support it ?
	// TODO : Or there is something else ?

	// TODO : Use Numea System Log
	printf("Desired format is not supported. Selecting available format - colorspace combination\n");
	return true;
}

bool VulkanSwapchain::queryImageHandles(std::vector<VkImage>& swapchainImageHandles)
{
	uint32_t imagesCount = 0;

	VkResult result = vkGetSwapchainImagesKHR(mDevice.getHandle(), mSwapchain, &imagesCount, nullptr);
	if (result != VK_SUCCESS || imagesCount == 0)
	{
		// TODO : Use Numea System Log
		printf("Could not get the number of swapchain images\n");
		return false;
	}

	swapchainImageHandles.resize(imagesCount);

	result = vkGetSwapchainImagesKHR(mDevice.getHandle(), mSwapchain, &imagesCount, swapchainImageHandles.data());
	if (result != VK_SUCCESS || imagesCount == 0)
	{
		// TODO : Use Numea System Log
		printf("Could not enumerate swapchain images\n");
		return false;
	}

	return true;
}

VULKAN_NAMESPACE_END