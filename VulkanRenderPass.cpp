#include "VulkanRenderPass.hpp"

#include "VulkanDevice.hpp"

namespace nu
{
namespace Vulkan
{

RenderPass::Ptr RenderPass::createRenderPass(Device& device, const std::vector<VkAttachmentDescription>& attachmentsDescriptions, const std::vector<SubpassParameters>& subpassParameters, const std::vector<VkSubpassDependency>& subpassDependencies)
{
	RenderPass::Ptr renderPass(new RenderPass(device, attachmentsDescriptions, subpassParameters, subpassDependencies));
	if (renderPass != nullptr)
	{
		if (!renderPass->init())
		{
			renderPass.reset();
		}
	}
	return renderPass;
}

RenderPass::~RenderPass()
{
	ObjectTracker::unregisterObject(ObjectType_RenderPass);

	release();
}

Framebuffer::Ptr RenderPass::createFramebuffer(const std::vector<VkImageView>& attachments, uint32_t width, uint32_t height, uint32_t layers)
{
	return Framebuffer::createFramebuffer(mDevice, *this, attachments, width, height, layers);
}

bool RenderPass::isInitialized() const
{
	return mRenderPass != VK_NULL_HANDLE;
}

const VkRenderPass& RenderPass::getHandle() const
{
	return mRenderPass;
}

const Device& RenderPass::getDeviceHandle() const
{
	return mDevice;
}

RenderPass::RenderPass(Device& device, const std::vector<VkAttachmentDescription>& attachmentsDescriptions, const std::vector<SubpassParameters>& subpassParameters, const std::vector<VkSubpassDependency>& subpassDependencies)
	: mDevice(device)
	, mRenderPass(VK_NULL_HANDLE)
	, mAttachmentsDescriptions(attachmentsDescriptions)
	, mSubpassParameters(subpassParameters)
	, mSubpassDependencies(subpassDependencies)
{
	ObjectTracker::registerObject(ObjectType_RenderPass);
}

bool RenderPass::init()
{
	std::vector<VkSubpassDescription> subpassDescriptions;
	for (auto& subpassDescription : mSubpassParameters)
	{
		subpassDescriptions.push_back({
			0,                                                                      // VkSubpassDescriptionFlags        flags
			subpassDescription.pipelineType,                                        // VkPipelineBindPoint              pipelineBindPoint
			static_cast<uint32_t>(subpassDescription.inputAttachments.size()),      // uint32_t                         inputAttachmentCount
			subpassDescription.inputAttachments.data(),                             // const VkAttachmentReference    * pInputAttachments
			static_cast<uint32_t>(subpassDescription.colorAttachments.size()),      // uint32_t                         colorAttachmentCount
			subpassDescription.colorAttachments.data(),                             // const VkAttachmentReference    * pColorAttachments
			subpassDescription.resolveAttachments.data(),                           // const VkAttachmentReference    * pResolveAttachments
			subpassDescription.depthStencilAttachment,                              // const VkAttachmentReference    * pDepthStencilAttachment
			static_cast<uint32_t>(subpassDescription.preserveAttachments.size()),   // uint32_t                         preserveAttachmentCount
			subpassDescription.preserveAttachments.data()                           // const uint32_t                 * pPreserveAttachments
		});
	}

	VkRenderPassCreateInfo renderPassCreateInfo = {
		VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO,                // VkStructureType                    sType
		nullptr,                                                  // const void                       * pNext
		0,                                                        // VkRenderPassCreateFlags            flags
		static_cast<uint32_t>(mAttachmentsDescriptions.size()),   // uint32_t                           attachmentCount
		mAttachmentsDescriptions.data(),                          // const VkAttachmentDescription    * pAttachments
		static_cast<uint32_t>(subpassDescriptions.size()),        // uint32_t                           subpassCount
		subpassDescriptions.data(),                               // const VkSubpassDescription       * pSubpasses
		static_cast<uint32_t>(mSubpassDependencies.size()),       // uint32_t                           dependencyCount
		mSubpassDependencies.data()                               // const VkSubpassDependency        * pDependencies
	};

	VkResult result = vkCreateRenderPass(mDevice.getHandle(), &renderPassCreateInfo, nullptr, &mRenderPass);
	if (result != VK_SUCCESS || mRenderPass == VK_NULL_HANDLE)
	{
		// TODO : Use Numea System Log
		printf("Could not create a render pass\n");
		return false;
	}
	return true;
}

bool RenderPass::release()
{
	if (mRenderPass != VK_NULL_HANDLE)
	{
		vkDestroyRenderPass(mDevice.getHandle(), mRenderPass, nullptr);
		mRenderPass = VK_NULL_HANDLE;
		return true;
	}
	return false;
}

} // namespace Vulkan
} // namespace nu