#ifndef NU_VULKAN_RENDER_PASS_HPP
#define NU_VULKAN_RENDER_PASS_HPP

#include "VulkanFunctions.hpp"

#include <memory>
#include <vector>

#include "VulkanFramebuffer.hpp"

namespace nu
{
namespace Vulkan
{

class Device;
class RenderPass
{
	public:
		typedef std::unique_ptr<RenderPass> Ptr;

		~RenderPass();

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

		Framebuffer::Ptr createFramebuffer(const std::vector<VkImageView>& attachments, uint32_t width, uint32_t height, uint32_t layers);

	private:
		friend class Device;
		RenderPass(Device& device);

		Device& mDevice;
		VkRenderPass mRenderPass;

		std::vector<VkAttachmentDescription> mAttachmentDescriptions;
		std::vector<VkSubpassDescription> mSubpassDescriptions;
	    std::vector<VkSubpassDependency> mSubpassDependencies;
		std::vector<VkAttachmentReference> mColorAttachmentReferences;
		std::unique_ptr<VkAttachmentReference> mDepthStencilAttachmentReference;
};

} // namespace Vulkan
} // namespace nu

#endif // NU_VULKAN_RENDER_PASS_HPP