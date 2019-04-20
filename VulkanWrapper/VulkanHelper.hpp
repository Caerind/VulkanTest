#pragma once

#include "VulkanFunctions.hpp"

VULKAN_NAMESPACE_BEGIN

class VulkanImageHelper
{
	public:
		static VulkanImageHelperPtr createImage2DAndView(VulkanDevice& device, VkFormat format, VkExtent2D size, uint32_t numMipmaps, uint32_t numLayers, VkSampleCountFlagBits samples, VkImageUsageFlags usage, VkImageAspectFlags aspect);
		static VulkanImageHelperPtr createLayered2DImageWithCubemapView(VulkanDevice& device, uint32_t size, uint32_t numMipmaps, VkImageUsageFlags usage, VkImageAspectFlags aspect);
		static VulkanImageHelperPtr createSampledImage(VulkanDevice& device, VkImageType type, VkFormat format, VkExtent3D size, uint32_t numMipmaps, uint32_t numLayers, VkImageUsageFlags usage, bool cubemap, VkImageViewType viewType, VkImageAspectFlags aspect, bool linearFiltering);
		static VulkanImageHelperPtr createCombinedImageSampler(VulkanDevice& device, VkImageType type, VkFormat format, VkExtent3D size, uint32_t numMipmaps, uint32_t numLayers, VkImageUsageFlags usage, bool cubemap, VkImageViewType viewType, VkImageAspectFlags aspect, VkFilter magFilter, VkFilter minFilter, VkSamplerMipmapMode mipmapMode, VkSamplerAddressMode uAddressMode, VkSamplerAddressMode vAddressMode, VkSamplerAddressMode wAddressMode, float lodBias, float minLod, float maxLod, bool anisotropyEnable, float maxAnisotropy, bool compareEnable, VkCompareOp compareOperator, VkBorderColor borderColor, bool unnormalizedCoords);
		static VulkanImageHelperPtr createStorageImage(VulkanDevice& device, VkImageType type, VkFormat format, VkExtent3D size, uint32_t numMipmaps, uint32_t numLayers, VkImageUsageFlags usage, VkImageViewType viewType, VkImageAspectFlags aspect, bool atomicOperations);
		static VulkanImageHelperPtr createInputAttachment(VulkanDevice& device, VkImageType type, VkFormat format, VkExtent3D size, VkImageUsageFlags usage, VkImageViewType viewType, VkImageAspectFlags aspect);

		VulkanImage* getImage();
		VulkanMemoryBlock* getMemoryBlock();
		VulkanImageView* getImageView();
		VulkanSampler* getSampler();

		bool hasSampler() const;

	protected:
		VulkanImageHelper();

		VulkanImagePtr mImage;
		VulkanMemoryBlock* mMemoryBlock;
		VulkanImageView* mImageView;
		VulkanSamplerPtr mSampler;
};

VULKAN_NAMESPACE_END