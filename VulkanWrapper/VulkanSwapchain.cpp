#include "VulkanSwapchain.hpp"

#include "VulkanDevice.hpp"
#include "VulkanSurface.hpp"

namespace nu
{
namespace Vulkan
{

Swapchain::~Swapchain()
{
	release();

	ObjectTracker::unregisterObject(ObjectType_Swapchain);
}

bool Swapchain::recreate(std::vector<FrameResources>& framesResources)
{
	return init(framesResources);
}

const VkFormat& Swapchain::getFormat() const
{
	return mFormat;
}

const VkColorSpaceKHR& Swapchain::getColorSpace() const
{
	return mColorSpace;
}

const VkExtent2D& Swapchain::getSize() const
{
	return mSize;
}

const VkFormat& Swapchain::getDepthFormat() const
{
	return mDepthFormat;
}

bool Swapchain::acquireImageIndex(uint64_t timeout, VkSemaphore semaphore, VkFence fence, uint32_t& imageIndex)
{
	VkResult result;
	result = vkAcquireNextImageKHR(mDevice.getHandle(), mSwapchain, timeout, semaphore, fence, &imageIndex);
	switch (result)
	{
		case VK_SUCCESS:
		case VK_SUBOPTIMAL_KHR:
			return true;
		default:
			return false;
	}
}

VkImage Swapchain::getImage(uint32_t index) const
{
	uint32_t size = (uint32_t)mSwapchainImages.size();
	if (index < size)
	{
		return mSwapchainImages[index];
	}
	return VK_NULL_HANDLE;
}

VkImageView Swapchain::getImageView(uint32_t index) const
{
	uint32_t size = (uint32_t)mSwapchainImageViewsRaw.size();
	if (index < size)
	{
		return mSwapchainImageViewsRaw[index];
	}
	return VK_NULL_HANDLE;
}

const VkSwapchainKHR& Swapchain::getHandle() const
{
	return mSwapchain;
}

const VkDevice& Swapchain::getDeviceHandle() const
{
	return mDevice.getHandle();
}

Swapchain::Ptr Swapchain::createSwapchain(Device& device, Surface& surface, std::vector<FrameResources>& framesResources)
{
	Swapchain::Ptr swapchain(new Swapchain(device, surface));
	if (swapchain != nullptr)
	{
		if (!swapchain->init(framesResources))
		{
			swapchain.reset();
		}
	}
	return swapchain;
}

Swapchain::Swapchain(Device& device, Surface& surface)
	: mDevice(device)
	, mSurface(surface)
	, mSwapchain(VK_NULL_HANDLE)
	, mReady(false)
{
	// TODO : Default values for each variables
	mDepthFormat = VK_FORMAT_D16_UNORM; // TODO : Unhardcode

	ObjectTracker::registerObject(ObjectType_Swapchain);
}

bool Swapchain::init(std::vector<FrameResources>& framesResources)
{
	mDevice.waitForAllSubmittedCommands();

	mReady = false;

	mSwapchainImageViewsRaw.clear();
	mSwapchainImageViews.clear();
	mSwapchainImages.clear();

	VkImageUsageFlags swapchainImageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT; // TODO : Unhardcode

	VkSwapchainKHR oldSwapchain = mSwapchain;
	mSwapchain = VK_NULL_HANDLE;

	bool result = createSwapchainInternal(swapchainImageUsage, oldSwapchain); // TODO : Move oldSwapchain inside
	if (oldSwapchain != VK_NULL_HANDLE)
	{
		vkDestroySwapchainKHR(mDevice.getHandle(), oldSwapchain, nullptr);
		oldSwapchain = VK_NULL_HANDLE;
	}
	if (!result)
	{
		return false;
	}

	for (size_t i = 0; i < mSwapchainImages.size(); i++)
	{
		mSwapchainImageViews.emplace_back(ImageView::createImageViewFromSwapchain(mDevice, mSwapchainImages[i], VK_IMAGE_VIEW_TYPE_2D, mFormat, VK_IMAGE_ASPECT_COLOR_BIT));
		if (mSwapchainImageViews[i] == nullptr || !mSwapchainImageViews[i]->isInitialized())
		{
			return false;
		}
		mSwapchainImageViewsRaw.push_back(mSwapchainImageViews[i]->getHandle());
	}

	mSwapchainDepthImages.clear();

	// TODO : Only if wanted
	if (true) // Use depth
	{
		const uint32_t framesCount = 3; // TODO : Unhardcode
		for (uint32_t i = 0; i < framesCount; ++i)
		{
			ImageHelper::Ptr depthImage = ImageHelper::createImage2DAndView(mDevice, mDepthFormat, mSize, 1, 1, VK_SAMPLE_COUNT_1_BIT, VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT, VK_IMAGE_ASPECT_DEPTH_BIT);
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

void Swapchain::release()
{
	if (mSwapchain != VK_NULL_HANDLE)
	{
		vkDestroySwapchainKHR(mDevice.getHandle(), mSwapchain, nullptr);
		mSwapchain = VK_NULL_HANDLE;
	}
}


bool Swapchain::createSwapchainInternal(VkImageUsageFlags desiredImageUsage, VkSwapchainKHR& oldSwapchain)
{
	VkPresentModeKHR desiredPresentMode = VK_PRESENT_MODE_MAILBOX_KHR; // TODO : Unhardcode
	VkPresentModeKHR presentMode; // TODO : Do this need to be saved ?
	if (!selectDesiredPresentMode(desiredPresentMode, presentMode))
	{
		return false;
	}

	VkSurfaceCapabilitiesKHR surfaceCapabilities; // TODO : Do this need to be saved ?// TODO : Do this need to be saved ?
	if (!queryCapabilities(surfaceCapabilities))
	{
		return false;
	}

	uint32_t imageCount; // TODO : Do this need to be saved ?
	imageCount = surfaceCapabilities.minImageCount + 1; // TODO : Improve selection of number of images
	if (surfaceCapabilities.maxImageCount > 0 && imageCount > surfaceCapabilities.maxImageCount)
	{
		imageCount = surfaceCapabilities.maxImageCount;
	}

	// On some platform extent works weird // TODO : Improve that comment
	if (surfaceCapabilities.currentExtent.width == 0xFFFFFFFF)
	{
		mSize = { 640, 480 }; // TODO : Unhardcode default size

		if (mSize.width < surfaceCapabilities.minImageExtent.width)
		{
			mSize.width = surfaceCapabilities.minImageExtent.width;
		}
		else if (mSize.width > surfaceCapabilities.maxImageExtent.width)
		{
			mSize.width = surfaceCapabilities.maxImageExtent.width;
		}

		if (mSize.height < surfaceCapabilities.minImageExtent.height)
		{
			mSize.height = surfaceCapabilities.minImageExtent.height;
		}
		else if (mSize.height > surfaceCapabilities.maxImageExtent.height)
		{
			mSize.height = surfaceCapabilities.maxImageExtent.height;
		}
	}
	else
	{
		mSize = surfaceCapabilities.currentExtent;
	}
	if (mSize.width == 0 || mSize.height == 0)
	{
		return false;
	}

	VkImageUsageFlags imageUsage; // TODO : Do this need to be saved ?
	imageUsage = desiredImageUsage & surfaceCapabilities.supportedUsageFlags;
	if (imageUsage != desiredImageUsage)
	{
		// TODO : Use Numea System Log
		printf("Could not create swapchain with that image usage\n");
		return false;
	}

	VkSurfaceTransformFlagBitsKHR desiredSurfaceTransform = VK_SURFACE_TRANSFORM_IDENTITY_BIT_KHR; // TODO : Unhardcode
	VkSurfaceTransformFlagBitsKHR surfaceTransform; // TODO : Do this need to be saved ?
	if (surfaceCapabilities.supportedTransforms & desiredSurfaceTransform)
	{
		surfaceTransform = desiredSurfaceTransform;
	}
	else
	{
		surfaceTransform = surfaceCapabilities.currentTransform;
	}

	VkSurfaceFormatKHR desiredSurfaceFormat = { VK_FORMAT_R8G8B8A8_UNORM, VK_COLOR_SPACE_SRGB_NONLINEAR_KHR }; // TODO : Unhardcode
	if (!selectSurfaceFormat(desiredSurfaceFormat))
	{
		return false;
	}

	VkSwapchainCreateInfoKHR swapchainCreateInfo = {
		VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR,  // VkStructureType                  sType
		nullptr,                                      // const void                     * pNext
		0,                                            // VkSwapchainCreateFlagsKHR        flags
		mSurface.getHandle(),                         // VkSurfaceKHR                     surface
		imageCount,                                   // uint32_t                         minImageCount
		mFormat,                                      // VkFormat                         imageFormat
		mColorSpace,                                  // VkColorSpaceKHR                  imageColorSpace
		mSize,                                        // VkExtent2D                       imageExtent
		1,                                            // uint32_t                         imageArrayLayers
		imageUsage,                                   // VkImageUsageFlags                imageUsage
		VK_SHARING_MODE_EXCLUSIVE,                    // VkSharingMode                    imageSharingMode
		0,                                            // uint32_t                         queueFamilyIndexCount
		nullptr,                                      // const uint32_t                 * pQueueFamilyIndices
		surfaceTransform,                             // VkSurfaceTransformFlagBitsKHR    preTransform
		VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR,            // VkCompositeAlphaFlagBitsKHR      compositeAlpha
		presentMode,                                  // VkPresentModeKHR                 presentMode
		VK_TRUE,                                      // VkBool32                         clipped
		oldSwapchain                                  // VkSwapchainKHR                   oldSwapchain
	};

	VkResult result = vkCreateSwapchainKHR(mDevice.getHandle(), &swapchainCreateInfo, nullptr, &mSwapchain);
	if (result != VK_SUCCESS || mSwapchain == VK_NULL_HANDLE)
	{
		// TODO : Use Numea System Log
		printf("Could not create a swapchain\n");
		return false;
	}

	if (oldSwapchain != VK_NULL_HANDLE)
	{
		vkDestroySwapchainKHR(mDevice.getHandle(), oldSwapchain, nullptr);
		oldSwapchain = VK_NULL_HANDLE;
	}

	if (!queryHandlesOfImages())
	{
		return false;
	}

	return true;
}

bool Swapchain::selectDesiredPresentMode(VkPresentModeKHR desiredPresentMode, VkPresentModeKHR& presentMode)
{
	uint32_t presentModesCount = 0;
	VkResult result;

	result = vkGetPhysicalDeviceSurfacePresentModesKHR(mDevice.getPhysicalHandle(), mSurface.getHandle(), &presentModesCount, nullptr);
	if (result != VK_SUCCESS || presentModesCount == 0)
	{
		// TODO : Use Numea System Log
		printf("Could not get the number of supported present modes\n");
		return false;
	}

	std::vector<VkPresentModeKHR> presentModes(presentModesCount);
	result = vkGetPhysicalDeviceSurfacePresentModesKHR(mDevice.getPhysicalHandle(), mSurface.getHandle(), &presentModesCount, presentModes.data());
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
			presentMode = desiredPresentMode;
			return true;
		}
	}

	// TODO : Use Numea System Log
	printf("Desired present mode is not supported. Selecting default FIFO mode\n");
	for (auto& currentPresentMode : presentModes)
	{
		if (currentPresentMode == VK_PRESENT_MODE_FIFO_KHR)
		{
			presentMode = VK_PRESENT_MODE_FIFO_KHR;
			return true;
		}
	}

	// TODO : Use Numea System Log
	printf("VK_PRESENT_MODE_FIFO_KHR is not supported though it's mandatory for all drivers!\n");
	return false;
}

bool Swapchain::queryCapabilities(VkSurfaceCapabilitiesKHR& surfaceCapabilities)
{
	VkResult result = vkGetPhysicalDeviceSurfaceCapabilitiesKHR(mDevice.getPhysicalHandle(), mSurface.getHandle(), &surfaceCapabilities);
	if (result != VK_SUCCESS)
	{
		// TODO : Use Numea System Log
		printf("Could not get the capabilities of a presentation surface\n");
		return false;
	}
	return true;
}

bool Swapchain::selectSurfaceFormat(VkSurfaceFormatKHR desiredSurfaceFormat)
{
	uint32_t formatsCount = 0;
	VkResult result;

	result = vkGetPhysicalDeviceSurfaceFormatsKHR(mDevice.getPhysicalHandle(), mSurface.getHandle(), &formatsCount, nullptr);
	if (result != VK_SUCCESS || formatsCount == 0)
	{
		// TODO : Use Numea System Log
		printf("Could not get the number of supported surface formats\n");
		return false;
	}

	std::vector<VkSurfaceFormatKHR> surfaceFormats(formatsCount);
	result = vkGetPhysicalDeviceSurfaceFormatsKHR(mDevice.getPhysicalHandle(), mSurface.getHandle(), &formatsCount, surfaceFormats.data());
	if (result != VK_SUCCESS || formatsCount == 0)
	{
		// TODO : Use Numea System Log
		printf("Could not enumerate supported surface formats\n");
		return false;
	}

	// TODO : Instead of choosing the nearest to what the user want, make the user directly choose the one he prefers
	// Moving the querying of surface formats at upper level

	if (surfaceFormats.size() == 1 && surfaceFormats[0].format == VK_FORMAT_UNDEFINED)
	{
		mFormat = desiredSurfaceFormat.format;
		mColorSpace = desiredSurfaceFormat.colorSpace;
		return true;
	}

	for (auto& surfaceFormat : surfaceFormats)
	{
		if (desiredSurfaceFormat.format == surfaceFormat.format && desiredSurfaceFormat.colorSpace == surfaceFormat.colorSpace)
		{
			mFormat = desiredSurfaceFormat.format;
			mColorSpace = desiredSurfaceFormat.colorSpace;
			return true;
		}
	}

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

	// TODO : assert(surfaceFormats.size() > 0);

	mFormat = surfaceFormats[0].format;
	mColorSpace = surfaceFormats[0].colorSpace;

	// TODO : Use Numea System Log
	printf("Desired format is not supported. Selecting available format - colorspace combination\n");
	return true;
}

bool Swapchain::queryHandlesOfImages()
{
	uint32_t imagesCount = 0;
	VkResult result;

	result = vkGetSwapchainImagesKHR(mDevice.getHandle(), mSwapchain, &imagesCount, nullptr);
	if (result != VK_SUCCESS || imagesCount == 0)
	{
		// TODO : Use Numea System Log
		printf("Could not get the number of swapchain images\n");
		return false;
	}

	mSwapchainImages.resize(imagesCount);
	result = vkGetSwapchainImagesKHR(mDevice.getHandle(), mSwapchain, &imagesCount, mSwapchainImages.data());
	if (result != VK_SUCCESS || imagesCount == 0)
	{
		// TODO : Use Numea System Log
		printf("Could not enumerate swapchain images\n");
		return false;
	}

	return true;
}

} // namespace Vulkan
} // namespace nu