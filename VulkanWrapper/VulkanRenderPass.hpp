#pragma once

#include "VulkanFunctions.hpp"

VULKAN_NAMESPACE_BEGIN

class VulkanRenderPass : public VulkanDeviceObject<VulkanObjectType_RenderPass>
{
	public:
		~VulkanRenderPass();

		void addAttachment(VkFormat format);
		void addAttachment(VkAttachmentDescriptionFlags flags, VkFormat format, VkSampleCountFlagBits samples, VkAttachmentLoadOp loadOp, VkAttachmentStoreOp storeOp, VkAttachmentLoadOp stencilLoadOp, VkAttachmentStoreOp stencilStoreOp, VkImageLayout initialLayout, VkImageLayout finalLayout);
		void addAttachment(const VkAttachmentDescription& attachmentDescription);
		void setAttachmentFlags(VkAttachmentDescriptionFlags flags);
		void setAttachmentFormat(VkFormat format);
		void setAttachmentSamples(VkSampleCountFlagBits samples);
		void setAttachmentLoadOp(VkAttachmentLoadOp loadOp);
		void setAttachmentStoreOp(VkAttachmentStoreOp storeOp);
		void setAttachmentStencilLoadOp(VkAttachmentLoadOp stencilLoadOp);
		void setAttachmentStencilStoreOp(VkAttachmentStoreOp stencilStoreOp);
		void setAttachmentInitialLayout(VkImageLayout initialLayout);
		void setAttachmentFinalLayout(VkImageLayout finalLayout);

		// TODO : Use parameters of VkSubpassDescription ?
		// TODO : addSubpass(VkSubpassDescription) ?
		// TODO : addSubpass with all parameters ?
		void addSubpass(VkPipelineBindPoint bindPoint);
		void addColorAttachmentToSubpass(uint32_t attachment, VkImageLayout imageLayout);
		void addInputAttachmentToSubpass(uint32_t attachment, VkImageLayout imageLayout);
		void addDepthStencilAttachmentToSubpass(uint32_t attachment, VkImageLayout imageLayout);

		void addDependency(uint32_t srcSubpass, uint32_t dstSubpass);
		void addDependency(uint32_t srcSubpass, uint32_t dstSubpass, VkPipelineStageFlags srcStageMask, VkPipelineStageFlags dstStageMask, VkAccessFlags srcAccessMask, VkAccessFlags dstAccessMask, VkDependencyFlags flags);
		void addDependency(const VkSubpassDependency& subpassDependency);
		void setDependencySrcSubpass(uint32_t srcSubpass);
		void setDependencyDstSubpass(uint32_t dstSubpass);
		void setDependencySrcStageMask(VkPipelineStageFlags srcStageMask);
		void setDependencyDstStageMask(VkPipelineStageFlags dstStageMask);
		void setDependencySrcAccessMask(VkAccessFlags srcAccessMask);
		void setDependencyDstAccessMask(VkAccessFlags dstAccessMask);
		void setDependencyFlags(VkDependencyFlags dependencyFlags);

		bool create();
		bool destroy();
		bool isCreated() const;
		const VkRenderPass& getHandle() const;

		VulkanFramebufferPtr createFramebuffer(const std::vector<VkImageView>& attachments, uint32_t width, uint32_t height, uint32_t layers);

	private:
		friend class VulkanDevice;
		VulkanRenderPass();

		VkRenderPass mRenderPass;

		static const int MaxColorAttachmentReferences = 20;
		static const int MaxInputAttachmentReferences = 20;

		std::vector<VkAttachmentDescription> mAttachmentDescriptions;
		std::vector<VkSubpassDescription> mSubpassDescriptions;
	    std::vector<VkSubpassDependency> mSubpassDependencies;
		VkAttachmentReference mColorAttachmentReferences[MaxColorAttachmentReferences];
		int mNumColorAttachmentReferences;
		VkAttachmentReference mInputAttachmentReferences[MaxInputAttachmentReferences];
		int mNumInputAttachmentReferences;
		std::unique_ptr<VkAttachmentReference> mDepthStencilAttachmentReference;
};

VULKAN_NAMESPACE_END