#pragma once

#include "VulkanFunctions.hpp"

VULKAN_NAMESPACE_BEGIN

class VulkanImageView : public VulkanDeviceObject<VulkanObjectType_ImageView>
{
	public:
		~VulkanImageView();

		VkImageViewType getViewType() const;
		VkFormat getFormat() const;
		VkImageAspectFlags getAspect() const;

		VulkanImage& getImage();
		const VulkanImage& getImage() const;
		const VkImageView& getHandle() const;
		const VkImage& getImageHandle() const;

	private:
		friend class VulkanImage;
		static VulkanImageViewPtr createImageView(VulkanImage& image, VkImageViewType viewType, VkFormat format, VkImageAspectFlags aspect);

		VulkanImageView(VulkanImage& image, VkImage imageHandleFromSwapchain, VkImageViewType viewType, VkFormat format, VkImageAspectFlags aspect);

		bool init();
		void release();

	private:
		VulkanImage& mImage;
		VkImageView mImageView;

		VkImageViewType mViewType;
		VkFormat mFormat;
		VkImageAspectFlags mAspect;
};

VULKAN_NAMESPACE_END