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

struct SubpassParameters 
{
	VkPipelineBindPoint                  pipelineType;
	std::vector<VkAttachmentReference>   inputAttachments;
	std::vector<VkAttachmentReference>   colorAttachments;
	std::vector<VkAttachmentReference>   resolveAttachments;
	const VkAttachmentReference*         depthStencilAttachment;
	std::vector<uint32_t>                preserveAttachments;
};

class Device;
class RenderPass
{
	public:
		typedef std::unique_ptr<RenderPass> Ptr;

		static RenderPass::Ptr createRenderPass(Device& device, const std::vector<VkAttachmentDescription>& attachmentsDescriptions, const std::vector<SubpassParameters>& subpassParameters, const std::vector<VkSubpassDependency>& subpassDependencies);

		~RenderPass();

		Framebuffer::Ptr createFramebuffer(const std::vector<VkImageView>& attachments, uint32_t width, uint32_t height, uint32_t layers);

		bool isInitialized() const;
		const VkRenderPass& getHandle() const;
		const Device& getDeviceHandle() const;

	private:
		RenderPass(Device& device, const std::vector<VkAttachmentDescription>& attachmentsDescriptions, const std::vector<SubpassParameters>& subpassParameters, const std::vector<VkSubpassDependency>& subpassDependencies);

		bool init();
		bool release();

		Device& mDevice;
		VkRenderPass mRenderPass;

		std::vector<VkAttachmentDescription> mAttachmentsDescriptions;
		std::vector<SubpassParameters> mSubpassParameters;
	    std::vector<VkSubpassDependency> mSubpassDependencies;
};

} // namespace Vulkan
} // namespace nu

#endif // NU_VULKAN_RENDER_PASS_HPP