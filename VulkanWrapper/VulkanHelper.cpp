#include "VulkanHelper.hpp"

#include "VulkanDevice.hpp"
#include "VulkanSampler.hpp"
#include "VulkanImage.hpp"
#include "VulkanImageView.hpp"
#include "VulkanPhysicalDevice.hpp"

VULKAN_NAMESPACE_BEGIN

VulkanImageHelperPtr VulkanImageHelper::createImage2DAndView(VulkanDevice& device, VkFormat format, VkExtent2D size, uint32_t numMipmaps, uint32_t numLayers, VkSampleCountFlagBits samples, VkImageUsageFlags usage, VkImageAspectFlags aspect)
{
	VulkanImageHelperPtr image(new VulkanImageHelper());
	if (image != nullptr)
	{
		image->mImage = device.createImage(VK_IMAGE_TYPE_2D, format, { size.width, size.height, 1 }, numMipmaps, numLayers, samples, usage, false);
		if (image->mImage == nullptr)
		{
			image.reset();
			return image;
		}
		
		image->mMemoryBlock = image->mImage->allocateMemoryBlock(VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);
		if (image->mMemoryBlock == nullptr)
		{
			image.reset();
			return image;
		}
			
		image->mImageView = image->mImage->createImageView(VK_IMAGE_VIEW_TYPE_2D, format, aspect);
		if (image->mImageView == nullptr)
		{
			image.reset();
			return image;
		}
	}
	return image;
}

VulkanImageHelperPtr VulkanImageHelper::createLayered2DImageWithCubemapView(VulkanDevice& device, uint32_t size, uint32_t numMipmaps, VkImageUsageFlags usage, VkImageAspectFlags aspect)
{
	VulkanImageHelperPtr image(new VulkanImageHelper());
	if (image != nullptr)
	{
		image->mImage = device.createImage(VK_IMAGE_TYPE_2D, VK_FORMAT_R8G8B8A8_UNORM, { size, size, 1 }, numMipmaps, 6, VK_SAMPLE_COUNT_1_BIT, usage, true);
		if (image->mImage == nullptr)
		{
			image.reset();
			return image;
		}

		image->mMemoryBlock = image->mImage->allocateMemoryBlock(VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);
		if (image->mMemoryBlock == nullptr)
		{
			image.reset();
			return image;
		}

		image->mImageView = image->mImage->createImageView(VK_IMAGE_VIEW_TYPE_CUBE, VK_FORMAT_R8G8B8A8_UNORM, aspect);
		if (image->mImageView == nullptr)
		{
			image.reset();
			return image;
		}
	}
	return image;
}

VulkanImageHelperPtr VulkanImageHelper::createSampledImage(VulkanDevice& device, VkImageType type, VkFormat format, VkExtent3D size, uint32_t numMipmaps, uint32_t numLayers, VkImageUsageFlags usage, bool cubemap, VkImageViewType viewType, VkImageAspectFlags aspect, bool linearFiltering)
{
	VulkanImageHelperPtr image(new VulkanImageHelper());
	if (image != nullptr)
	{
		const VkFormatProperties& formatProperties = device.getPhysicalDevice().getFormatProperties(format);
		if (!(formatProperties.optimalTilingFeatures & VK_FORMAT_FEATURE_SAMPLED_IMAGE_BIT))
		{
			// TODO : Use Numea System Log
			printf("Provided format is not supported for a sampled image\n");
			image.reset();
			return image;
		}
		if (linearFiltering && !(formatProperties.optimalTilingFeatures & VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_LINEAR_BIT))
		{
			// TODO : Use Numea System Log
			printf("Provided format is not supported for a linear image filtering\n");
			image.reset();
			return image;
		}

		image->mImage = device.createImage(type, format, size, numMipmaps, numLayers, VK_SAMPLE_COUNT_1_BIT, usage | VK_IMAGE_USAGE_SAMPLED_BIT, cubemap);
		if (image->mImage == nullptr)
		{
			image.reset();
			return image;
		}

		image->mMemoryBlock = image->mImage->allocateMemoryBlock(VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);
		if (image->mMemoryBlock == nullptr)
		{
			image.reset();
			return image;
		}

		image->mImageView = image->mImage->createImageView(viewType, format, aspect);
		if (image->mImageView == nullptr)
		{
			image.reset();
			return image;
		}
	}
	return image;
}

VulkanImageHelperPtr VulkanImageHelper::createCombinedImageSampler(VulkanDevice& device, VkImageType type, VkFormat format, VkExtent3D size, uint32_t numMipmaps, uint32_t numLayers, VkImageUsageFlags usage, bool cubemap, VkImageViewType viewType, VkImageAspectFlags aspect, VkFilter magFilter, VkFilter minFilter, VkSamplerMipmapMode mipmapMode, VkSamplerAddressMode uAddressMode, VkSamplerAddressMode vAddressMode, VkSamplerAddressMode wAddressMode, float lodBias, float minLod, float maxLod, bool anisotropyEnable, float maxAnisotropy, bool compareEnable, VkCompareOp compareOperator, VkBorderColor borderColor, bool unnormalizedCoords)
{
	VulkanImageHelperPtr image(new VulkanImageHelper());
	if (image != nullptr)
	{
		image->mSampler = device.createSampler(magFilter, minFilter, mipmapMode, uAddressMode, vAddressMode, wAddressMode, lodBias, minLod, maxLod, anisotropyEnable, maxAnisotropy, compareEnable, compareOperator, borderColor, unnormalizedCoords);
		if (image->mSampler == nullptr)
		{
			image.reset();
			return image;
		}

		bool linearFiltering = (magFilter == VK_FILTER_LINEAR) || (minFilter == VK_FILTER_LINEAR) || (mipmapMode == VK_SAMPLER_MIPMAP_MODE_LINEAR);

		const VkFormatProperties& formatProperties = device.getPhysicalDevice().getFormatProperties(format);
		if (!(formatProperties.optimalTilingFeatures & VK_FORMAT_FEATURE_SAMPLED_IMAGE_BIT))
		{
			// TODO : Use Numea System Log
			printf("Provided format is not supported for a sampled image\n");
			image.reset();
			return image;
		}
		if (linearFiltering && !(formatProperties.optimalTilingFeatures & VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_LINEAR_BIT))
		{
			// TODO : Use Numea System Log
			printf("Provided format is not supported for a linear image filtering\n");
			image.reset();
			return image;
		}

		image->mImage = device.createImage(type, format, size, numMipmaps, numLayers, VK_SAMPLE_COUNT_1_BIT, usage | VK_IMAGE_USAGE_SAMPLED_BIT, cubemap);
		if (image->mImage == nullptr)
		{
			image.reset();
			return image;
		}

		image->mMemoryBlock = image->mImage->allocateMemoryBlock(VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);
		if (image->mMemoryBlock == nullptr)
		{
			image.reset();
			return image;
		}

		image->mImageView = image->mImage->createImageView(viewType, format, aspect);
		if (image->mImageView == nullptr)
		{
			image.reset();
			return image;
		}
	}
	return image;
}

VulkanImageHelperPtr VulkanImageHelper::createStorageImage(VulkanDevice& device, VkImageType type, VkFormat format, VkExtent3D size, uint32_t numMipmaps, uint32_t numLayers, VkImageUsageFlags usage, VkImageViewType viewType, VkImageAspectFlags aspect, bool atomicOperations)
{
	VulkanImageHelperPtr image(new VulkanImageHelper());
	if (image != nullptr)
	{
		const VkFormatProperties& formatProperties = device.getPhysicalDevice().getFormatProperties(format);
		if (!(formatProperties.optimalTilingFeatures & VK_FORMAT_FEATURE_STORAGE_IMAGE_BIT))
		{
			// TODO : Use Numea System Log
			printf("Provided format is not supported for a storage image\n");
			image.reset();
			return image;
		}
		if (atomicOperations && !(formatProperties.optimalTilingFeatures & VK_FORMAT_FEATURE_STORAGE_IMAGE_ATOMIC_BIT))
		{
			// TODO : Use Numea System Log
			printf("Provided format is not supported for atomic operations on storage images\n");
			return false;
		}

		image->mImage = device.createImage(type, format, size, numMipmaps, numLayers, VK_SAMPLE_COUNT_1_BIT, usage | VK_IMAGE_USAGE_STORAGE_BIT, false);
		if (image->mImage == nullptr)
		{
			image.reset();
			return image;
		}

		image->mMemoryBlock = image->mImage->allocateMemoryBlock(VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);
		if (image->mMemoryBlock == nullptr)
		{
			image.reset();
			return image;
		}

		image->mImageView = image->mImage->createImageView(viewType, format, aspect);
		if (image->mImageView == nullptr)
		{
			image.reset();
			return image;
		}
	}
	return image;
}

VulkanImageHelperPtr VulkanImageHelper::createInputAttachment(VulkanDevice& device, VkImageType type, VkFormat format, VkExtent3D size, VkImageUsageFlags usage, VkImageViewType viewType, VkImageAspectFlags aspect)
{
	VulkanImageHelperPtr image(new VulkanImageHelper());
	if (image != nullptr)
	{
		const VkFormatProperties& formatProperties = device.getPhysicalDevice().getFormatProperties(format);
		if ((aspect & VK_IMAGE_ASPECT_COLOR_BIT) && !(formatProperties.optimalTilingFeatures & VK_FORMAT_FEATURE_COLOR_ATTACHMENT_BIT))
		{
			// TODO : Use Numea System Log
			printf("Provided format is not supported for an input attachment\n");
			image.reset();
			return image;
		}
		if ((aspect & (VK_IMAGE_ASPECT_DEPTH_BIT | VK_IMAGE_ASPECT_STENCIL_BIT)) && !(formatProperties.optimalTilingFeatures & VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT))
		{
			// TODO : Use Numea System Log
			printf("Provided format is not supported for an input attachment\n");
			image.reset();
			return image;
		}

		image->mImage = device.createImage(type, format, size, 1, 1, VK_SAMPLE_COUNT_1_BIT, usage | VK_IMAGE_USAGE_INPUT_ATTACHMENT_BIT, false);
		if (image->mImage == nullptr)
		{
			image.reset();
			return image;
		}

		image->mMemoryBlock = image->mImage->allocateMemoryBlock(VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);
		if (image->mMemoryBlock == nullptr)
		{
			image.reset();
			return image;
		}

		image->mImageView = image->mImage->createImageView(viewType, format, aspect);
		if (image->mImageView == nullptr)
		{
			image.reset();
			return image;
		}
	}
	return image;
}

VulkanImage* VulkanImageHelper::getImage()
{
	return mImage.get();
}

VulkanMemoryBlock* VulkanImageHelper::getMemoryBlock()
{
	return mMemoryBlock;
}

VulkanImageView* VulkanImageHelper::getImageView()
{
	return mImageView;
}

VulkanSampler* VulkanImageHelper::getSampler()
{
	return mSampler.get();
}

bool VulkanImageHelper::hasSampler() const
{
	return mSampler != nullptr;
}

VulkanImageHelper::VulkanImageHelper()
	: mImage(nullptr)
	, mMemoryBlock(nullptr)
	, mImageView(nullptr)
	, mSampler(nullptr)
{
}