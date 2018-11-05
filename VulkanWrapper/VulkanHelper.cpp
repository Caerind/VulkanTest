#include "VulkanHelper.hpp"

#include "VulkanDevice.hpp"

namespace nu
{
namespace Vulkan
{

ImageHelper::Ptr ImageHelper::createImage2DAndView(Device& device, VkFormat format, VkExtent2D size, uint32_t numMipmaps, uint32_t numLayers, VkSampleCountFlagBits samples, VkImageUsageFlags usage, VkImageAspectFlags aspect)
{
	ImageHelper::Ptr image(new ImageHelper());
	if (image != nullptr)
	{
		image->mImage = device.createImage(VK_IMAGE_TYPE_2D, format, { size.width, size.height, 1 }, numMipmaps, numLayers, samples, usage, false);
		if (image->mImage == nullptr || !image->mImage->isInitialized())
		{
			image.reset();
			return image;
		}
		
		image->mMemoryBlock = image->mImage->allocateAndBindMemoryBlock(VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);
		if (image->mMemoryBlock == nullptr || !image->mMemoryBlock->isInitialized())
		{
			image.reset();
			return image;
		}
			
		image->mImageView = image->mImage->createImageView(VK_IMAGE_VIEW_TYPE_2D, format, aspect);
		if (image->mImageView == nullptr || !image->mImageView->isInitialized())
		{
			image.reset();
			return image;
		}
	}
	return image;
}

ImageHelper::Ptr ImageHelper::createLayered2DImageWithCubemapView(Device& device, uint32_t size, uint32_t numMipmaps, VkImageUsageFlags usage, VkImageAspectFlags aspect)
{
	ImageHelper::Ptr image(new ImageHelper());
	if (image != nullptr)
	{
		image->mImage = device.createImage(VK_IMAGE_TYPE_2D, VK_FORMAT_R8G8B8A8_UNORM, { size, size, 1 }, numMipmaps, 6, VK_SAMPLE_COUNT_1_BIT, usage, true);
		if (image->mImage == nullptr || !image->mImage->isInitialized())
		{
			image.reset();
			return image;
		}

		image->mMemoryBlock = image->mImage->allocateAndBindMemoryBlock(VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);
		if (image->mMemoryBlock == nullptr || !image->mMemoryBlock->isInitialized())
		{
			image.reset();
			return image;
		}

		image->mImageView = image->mImage->createImageView(VK_IMAGE_VIEW_TYPE_CUBE, VK_FORMAT_R8G8B8A8_UNORM, aspect);
		if (image->mImageView == nullptr || !image->mImageView->isInitialized())
		{
			image.reset();
			return image;
		}
	}
	return image;
}

ImageHelper::Ptr ImageHelper::createSampledImage(Device& device, VkImageType type, VkFormat format, VkExtent3D size, uint32_t numMipmaps, uint32_t numLayers, VkImageUsageFlags usage, bool cubemap, VkImageViewType viewType, VkImageAspectFlags aspect, bool linearFiltering)
{
	ImageHelper::Ptr image(new ImageHelper());
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
		if (image->mImage == nullptr || !image->mImage->isInitialized())
		{
			image.reset();
			return image;
		}

		image->mMemoryBlock = image->mImage->allocateAndBindMemoryBlock(VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);
		if (image->mMemoryBlock == nullptr || !image->mMemoryBlock->isInitialized())
		{
			image.reset();
			return image;
		}

		image->mImageView = image->mImage->createImageView(viewType, format, aspect);
		if (image->mImageView == nullptr || !image->mImageView->isInitialized())
		{
			image.reset();
			return image;
		}
	}
	return image;
}

ImageHelper::Ptr ImageHelper::createCombinedImageSampler(Device& device, VkImageType type, VkFormat format, VkExtent3D size, uint32_t numMipmaps, uint32_t numLayers, VkImageUsageFlags usage, bool cubemap, VkImageViewType viewType, VkImageAspectFlags aspect, VkFilter magFilter, VkFilter minFilter, VkSamplerMipmapMode mipmapMode, VkSamplerAddressMode uAddressMode, VkSamplerAddressMode vAddressMode, VkSamplerAddressMode wAddressMode, float lodBias, float minLod, float maxLod, bool anisotropyEnable, float maxAnisotropy, bool compareEnable, VkCompareOp compareOperator, VkBorderColor borderColor, bool unnormalizedCoords)
{
	ImageHelper::Ptr image(new ImageHelper());
	if (image != nullptr)
	{
		image->mSampler = device.createSampler(magFilter, minFilter, mipmapMode, uAddressMode, vAddressMode, wAddressMode, lodBias, minLod, maxLod, anisotropyEnable, maxAnisotropy, compareEnable, compareOperator, borderColor, unnormalizedCoords);
		if (image->mSampler == nullptr || !image->mSampler->isInitialized())
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
		if (image->mImage == nullptr || !image->mImage->isInitialized())
		{
			image.reset();
			return image;
		}

		image->mMemoryBlock = image->mImage->allocateAndBindMemoryBlock(VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);
		if (image->mMemoryBlock == nullptr || !image->mMemoryBlock->isInitialized())
		{
			image.reset();
			return image;
		}

		image->mImageView = image->mImage->createImageView(viewType, format, aspect);
		if (image->mImageView == nullptr || !image->mImageView->isInitialized())
		{
			image.reset();
			return image;
		}
	}
	return image;
}

ImageHelper::Ptr ImageHelper::createStorageImage(Device& device, VkImageType type, VkFormat format, VkExtent3D size, uint32_t numMipmaps, uint32_t numLayers, VkImageUsageFlags usage, VkImageViewType viewType, VkImageAspectFlags aspect, bool atomicOperations)
{
	ImageHelper::Ptr image(new ImageHelper());
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
		if (image->mImage == nullptr || !image->mImage->isInitialized())
		{
			image.reset();
			return image;
		}

		image->mMemoryBlock = image->mImage->allocateAndBindMemoryBlock(VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);
		if (image->mMemoryBlock == nullptr || !image->mMemoryBlock->isInitialized())
		{
			image.reset();
			return image;
		}

		image->mImageView = image->mImage->createImageView(viewType, format, aspect);
		if (image->mImageView == nullptr || !image->mImageView->isInitialized())
		{
			image.reset();
			return image;
		}
	}
	return image;
}

ImageHelper::Ptr ImageHelper::createInputAttachment(Device& device, VkImageType type, VkFormat format, VkExtent3D size, VkImageUsageFlags usage, VkImageViewType viewType, VkImageAspectFlags aspect)
{
	ImageHelper::Ptr image(new ImageHelper());
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
		if (image->mImage == nullptr || !image->mImage->isInitialized())
		{
			image.reset();
			return image;
		}

		image->mMemoryBlock = image->mImage->allocateAndBindMemoryBlock(VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);
		if (image->mMemoryBlock == nullptr || !image->mMemoryBlock->isInitialized())
		{
			image.reset();
			return image;
		}

		image->mImageView = image->mImage->createImageView(viewType, format, aspect);
		if (image->mImageView == nullptr || !image->mImageView->isInitialized())
		{
			image.reset();
			return image;
		}
	}
	return image;
}

Image* ImageHelper::getImage()
{
	return mImage.get();
}

MemoryBlock* ImageHelper::getMemoryBlock()
{
	return mMemoryBlock.get();
}

ImageView* ImageHelper::getImageView()
{
	return mImageView.get();
}

Sampler* ImageHelper::getSampler()
{
	return mSampler.get();
}

bool ImageHelper::hasSampler() const
{
	return mSampler != nullptr;
}

ImageHelper::ImageHelper()
	: mImage(nullptr)
	, mMemoryBlock(nullptr)
	, mImageView(nullptr)
	, mSampler(nullptr)
{
}

BufferHelper::Ptr BufferHelper::createUniformTexelBuffer(Device& device, VkFormat format, VkDeviceSize size, VkImageUsageFlags usage)
{
	BufferHelper::Ptr buffer(new BufferHelper());
	if (buffer != nullptr)
	{
		const VkFormatProperties& formatProperties = device.getPhysicalDevice().getFormatProperties(format);
		if (!(formatProperties.bufferFeatures & VK_FORMAT_FEATURE_UNIFORM_TEXEL_BUFFER_BIT))
		{
			// TODO : Use Numea System Log
			printf("Provided format is not supported for a uniform texel buffer\n");
			buffer.reset();
			return buffer;
		}

		buffer->mBuffer = device.createBuffer(size, usage | VK_BUFFER_USAGE_UNIFORM_TEXEL_BUFFER_BIT);
		if (buffer->mBuffer == nullptr || !buffer->mBuffer->isInitialized())
		{
			buffer.reset();
			return buffer;
		}

		buffer->mMemoryBlock = buffer->mBuffer->allocateAndBindMemoryBlock(VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);
		if (buffer->mMemoryBlock == nullptr || !buffer->mMemoryBlock->isInitialized())
		{
			buffer.reset();
			return buffer;
		}

		buffer->mBufferView = buffer->mBuffer->createBufferView(format, 0, VK_WHOLE_SIZE);
		if (buffer->mBufferView == nullptr || !buffer->mBufferView->isInitialized())
		{
			buffer.reset();
			return buffer;
		}
	}
	return buffer;
}

BufferHelper::Ptr BufferHelper::createStorageTexelBuffer(Device& device, VkFormat format, VkDeviceSize size, VkBufferUsageFlags usage, bool atomicOperations)
{
	BufferHelper::Ptr buffer(new BufferHelper());
	if (buffer != nullptr)
	{
		const VkFormatProperties& formatProperties = device.getPhysicalDevice().getFormatProperties(format);
		if (!(formatProperties.bufferFeatures & VK_FORMAT_FEATURE_STORAGE_TEXEL_BUFFER_BIT))
		{
			// TODO : Use Numea System Log
			printf("Provided format is not supported for a storage texel buffer\n");
			buffer.reset();
			return buffer;
		}

		if (atomicOperations && !(formatProperties.bufferFeatures & VK_FORMAT_FEATURE_STORAGE_TEXEL_BUFFER_ATOMIC_BIT))
		{
			// TODO : Use Numea System Log
			printf("Provided format is not supported for atomic operations on storage texel buffers\n");
			buffer.reset();
			return buffer;
		}

		buffer->mBuffer = device.createBuffer(size, usage | VK_BUFFER_USAGE_STORAGE_TEXEL_BUFFER_BIT);
		if (buffer->mBuffer == nullptr || !buffer->mBuffer->isInitialized())
		{
			buffer.reset();
			return buffer;
		}

		buffer->mMemoryBlock = buffer->mBuffer->allocateAndBindMemoryBlock(VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);
		if (buffer->mMemoryBlock == nullptr || !buffer->mMemoryBlock->isInitialized())
		{
			buffer.reset();
			return buffer;
		}

		buffer->mBufferView = buffer->mBuffer->createBufferView(format, 0, VK_WHOLE_SIZE);
		if (buffer->mBufferView == nullptr || !buffer->mBufferView->isInitialized())
		{
			buffer.reset();
			return buffer;
		}
	}
	return buffer;
}

BufferHelper::Ptr BufferHelper::createUniformBuffer(Device& device, VkDeviceSize size, VkBufferUsageFlags usage)
{
	BufferHelper::Ptr buffer(new BufferHelper());
	if (buffer != nullptr)
	{
		buffer->mBuffer = device.createBuffer(size, usage | VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT);
		if (buffer->mBuffer == nullptr || !buffer->mBuffer->isInitialized())
		{
			buffer.reset();
			return buffer;
		}

		buffer->mMemoryBlock = buffer->mBuffer->allocateAndBindMemoryBlock(VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);
		if (buffer->mMemoryBlock == nullptr || !buffer->mMemoryBlock->isInitialized())
		{
			buffer.reset();
			return buffer;
		}
	}
	return buffer;
}

BufferHelper::Ptr BufferHelper::createStorageBuffer(Device& device, VkDeviceSize size, VkBufferUsageFlags usage)
{
	BufferHelper::Ptr buffer(new BufferHelper());
	if (buffer != nullptr)
	{
		buffer->mBuffer = device.createBuffer(size, usage | VK_BUFFER_USAGE_STORAGE_BUFFER_BIT);
		if (buffer->mBuffer == nullptr || !buffer->mBuffer->isInitialized())
		{
			buffer.reset();
			return buffer;
		}

		buffer->mMemoryBlock = buffer->mBuffer->allocateAndBindMemoryBlock(VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);
		if (buffer->mMemoryBlock == nullptr || !buffer->mMemoryBlock->isInitialized())
		{
			buffer.reset();
			return buffer;
		}
	}
	return buffer;
}

Buffer* BufferHelper::getBuffer()
{
	return mBuffer.get();
}

MemoryBlock* BufferHelper::getMemoryBlock()
{
	return mMemoryBlock.get();
}

BufferView* BufferHelper::getBufferView()
{
	return mBufferView.get();
}

bool BufferHelper::hasBufferView() const
{
	return mBufferView != nullptr;
}

BufferHelper::BufferHelper()
	: mBuffer(nullptr)
	, mMemoryBlock(nullptr)
	, mBufferView(nullptr)
{
}

} // namespace Vulkan
} // namespace nu