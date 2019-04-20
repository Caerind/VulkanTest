#pragma once

#include "VulkanFunctions.hpp"

VULKAN_NAMESPACE_BEGIN

class VulkanFramebuffer : public VulkanObject<VulkanObjectType_Framebuffer>
{
	public:
		static VulkanFramebufferPtr createFramebuffer(VulkanDevice& device, VulkanRenderPass& renderPass, const std::vector<VkImageView>& attachments, VulkanU32 width, VulkanU32 height, VulkanU32 layers);

		~VulkanFramebuffer();

		bool isInitialized() const;
		const VkFramebuffer& getHandle() const;
		const VkDevice& getDeviceHandle() const;

	private:
		VulkanFramebuffer(VulkanDevice& device, VulkanRenderPass& renderPass, const std::vector<VkImageView>& attachments, VulkanU32 width, VulkanU32 height, VulkanU32 layers);

		bool init();
		bool release();

		VulkanDevice& mDevice;
		VulkanRenderPass& mRenderPass;
		VkFramebuffer mFramebuffer;

		std::vector<VkImageView> mAttachments;
		VulkanU32 mWidth;
		VulkanU32 mHeight;
		VulkanU32 mLayers;
};

VULKAN_NAMESPACE_END