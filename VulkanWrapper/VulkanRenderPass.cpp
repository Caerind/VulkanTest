#include "VulkanRenderPass.hpp"

#include "VulkanDevice.hpp"
#include "VulkanFramebuffer.hpp"

VULKAN_NAMESPACE_BEGIN

VulkanRenderPass::~VulkanRenderPass()
{
	destroy();

	VULKAN_OBJECTTRACKER_UNREGISTER();
}

void VulkanRenderPass::addAttachment(VkFormat format)
{
	VkAttachmentDescription attachmentDescription = {
		0,                                                // VkAttachmentDescriptionFlags     flags
		format,                                           // VkFormat                         format
		VK_SAMPLE_COUNT_1_BIT,                            // VkSampleCountFlagBits            samples
		VK_ATTACHMENT_LOAD_OP_DONT_CARE,                  // VkAttachmentLoadOp               loadOp
		VK_ATTACHMENT_STORE_OP_DONT_CARE,                 // VkAttachmentStoreOp              storeOp
		VK_ATTACHMENT_LOAD_OP_DONT_CARE,                  // VkAttachmentLoadOp               stencilLoadOp
		VK_ATTACHMENT_STORE_OP_DONT_CARE,                 // VkAttachmentStoreOp              stencilStoreOp
		VK_IMAGE_LAYOUT_UNDEFINED,                        // VkImageLayout                    initialLayout
		VK_IMAGE_LAYOUT_UNDEFINED                         // VkImageLayout                    finalLayout
	};

	mAttachmentDescriptions.push_back(attachmentDescription);
}

void VulkanRenderPass::addAttachment(VkAttachmentDescriptionFlags flags, VkFormat format, VkSampleCountFlagBits samples, VkAttachmentLoadOp loadOp, VkAttachmentStoreOp storeOp, VkAttachmentLoadOp stencilLoadOp, VkAttachmentStoreOp stencilStoreOp, VkImageLayout initialLayout, VkImageLayout finalLayout)
{
	VkAttachmentDescription attachmentDescription = {
		flags,                                            // VkAttachmentDescriptionFlags     flags
		format,                                           // VkFormat                         format
		samples,                                          // VkSampleCountFlagBits            samples
		loadOp,                                           // VkAttachmentLoadOp               loadOp
		storeOp,                                          // VkAttachmentStoreOp              storeOp
		stencilLoadOp,                                    // VkAttachmentLoadOp               stencilLoadOp
		stencilStoreOp,                                   // VkAttachmentStoreOp              stencilStoreOp
		initialLayout,                                    // VkImageLayout                    initialLayout
		finalLayout                                       // VkImageLayout                    finalLayout
	};

	mAttachmentDescriptions.push_back(attachmentDescription);
}

void VulkanRenderPass::addAttachment(const VkAttachmentDescription& attachmentDescription)
{
	mAttachmentDescriptions.push_back(attachmentDescription);
}

void VulkanRenderPass::setAttachmentFlags(VkAttachmentDescriptionFlags flags)
{
	mAttachmentDescriptions.back().flags = flags;
}

void VulkanRenderPass::setAttachmentFormat(VkFormat format)
{
	mAttachmentDescriptions.back().format = format;
}

void VulkanRenderPass::setAttachmentSamples(VkSampleCountFlagBits samples)
{
	mAttachmentDescriptions.back().samples = samples;
}

void VulkanRenderPass::setAttachmentLoadOp(VkAttachmentLoadOp loadOp)
{
	mAttachmentDescriptions.back().loadOp = loadOp;
}

void VulkanRenderPass::setAttachmentStoreOp(VkAttachmentStoreOp storeOp)
{
	mAttachmentDescriptions.back().storeOp = storeOp;
}

void VulkanRenderPass::setAttachmentStencilLoadOp(VkAttachmentLoadOp stencilLoadOp)
{
	mAttachmentDescriptions.back().stencilLoadOp = stencilLoadOp;
}

void VulkanRenderPass::setAttachmentStencilStoreOp(VkAttachmentStoreOp stencilStoreOp)
{
	mAttachmentDescriptions.back().stencilStoreOp = stencilStoreOp;
}

void VulkanRenderPass::setAttachmentInitialLayout(VkImageLayout initialLayout)
{
	mAttachmentDescriptions.back().initialLayout = initialLayout;
}

void VulkanRenderPass::setAttachmentFinalLayout(VkImageLayout finalLayout)
{
	mAttachmentDescriptions.back().finalLayout = finalLayout;
}

void VulkanRenderPass::addSubpass(VkPipelineBindPoint bindPoint)
{
	VkSubpassDescription subpassDescription = {
		0,                         // VkSubpassDescriptionFlags        flags
		bindPoint,                 // VkPipelineBindPoint              pipelineBindPoint
		0,                         // uint32_t                         inputAttachmentCount
		nullptr,                   // const VkAttachmentReference    * pInputAttachments
		0,                         // uint32_t                         colorAttachmentCount
		nullptr,                   // const VkAttachmentReference    * pColorAttachments
		nullptr,                   // const VkAttachmentReference    * pResolveAttachments
		nullptr,                   // const VkAttachmentReference    * pDepthStencilAttachment
		0,                         // uint32_t                         preserveAttachmentCount
		nullptr                    // const uint32_t                 * pPreserveAttachments
	};

	mSubpassDescriptions.push_back(subpassDescription);
}

void VulkanRenderPass::addColorAttachmentToSubpass(uint32_t attachment, VkImageLayout imageLayout)
{
	VULKAN_ASSERT(mNumColorAttachmentReferences < MaxColorAttachmentReferences);

	mColorAttachmentReferences[mNumColorAttachmentReferences] = VkAttachmentReference {
		attachment,      // uint32_t                             attachment
		imageLayout      // VkImageLayout                        layout
	};

	VkSubpassDescription& subpassDescription = mSubpassDescriptions.back();
	if (subpassDescription.colorAttachmentCount == 0)
	{
		subpassDescription.pColorAttachments = &mColorAttachmentReferences[mNumColorAttachmentReferences];
	}

	subpassDescription.colorAttachmentCount++;
	mNumColorAttachmentReferences++;
}

void VulkanRenderPass::addInputAttachmentToSubpass(uint32_t attachment, VkImageLayout imageLayout)
{
	VULKAN_ASSERT(mNumInputAttachmentReferences < MaxInputAttachmentReferences);

	mInputAttachmentReferences[mNumInputAttachmentReferences] = VkAttachmentReference{
		attachment,      // uint32_t                             attachment
		imageLayout      // VkImageLayout                        layout
	};

	VkSubpassDescription& subpassDescription = mSubpassDescriptions.back();
	if (subpassDescription.inputAttachmentCount == 0)
	{
		subpassDescription.pInputAttachments = &mInputAttachmentReferences[mNumInputAttachmentReferences];
	}

	subpassDescription.inputAttachmentCount++;
	mNumInputAttachmentReferences++;
}

void VulkanRenderPass::addDepthStencilAttachmentToSubpass(uint32_t attachment, VkImageLayout imageLayout)
{
	VkAttachmentReference attachmentReference = {
		attachment,      // uint32_t                             attachment
		imageLayout      // VkImageLayout                        layout
	};
	mDepthStencilAttachmentReference = std::unique_ptr<VkAttachmentReference>(new VkAttachmentReference(attachmentReference));

	mSubpassDescriptions.back().pDepthStencilAttachment = mDepthStencilAttachmentReference.get();
}

void VulkanRenderPass::addDependency(uint32_t srcSubpass, uint32_t dstSubpass)
{
	VkSubpassDependency subpassDependency = {
		srcSubpass,                                     // uint32_t                   srcSubpass
		dstSubpass,                                     // uint32_t                   dstSubpass
		0,                                              // VkPipelineStageFlags       srcStageMask
		0,                                              // VkPipelineStageFlags       dstStageMask
		0,                                              // VkAccessFlags              srcAccessMask
		0,                                              // VkAccessFlags              dstAccessMask
		VK_DEPENDENCY_BY_REGION_BIT                     // VkDependencyFlags          dependencyFlags
	};

	mSubpassDependencies.push_back(subpassDependency);
}

void VulkanRenderPass::addDependency(uint32_t srcSubpass, uint32_t dstSubpass, VkPipelineStageFlags srcStageMask, VkPipelineStageFlags dstStageMask, VkAccessFlags srcAccessMask, VkAccessFlags dstAccessMask, VkDependencyFlags flags)
{
	VkSubpassDependency subpassDependency = {
		srcSubpass,                                     // uint32_t                   srcSubpass
		dstSubpass,                                     // uint32_t                   dstSubpass
		srcStageMask,                                   // VkPipelineStageFlags       srcStageMask
		dstStageMask,                                   // VkPipelineStageFlags       dstStageMask
		srcAccessMask,                                  // VkAccessFlags              srcAccessMask
		dstAccessMask,                                  // VkAccessFlags              dstAccessMask
		VK_DEPENDENCY_BY_REGION_BIT                     // VkDependencyFlags          dependencyFlags
	};

	mSubpassDependencies.push_back(subpassDependency);
}

void VulkanRenderPass::addDependency(const VkSubpassDependency& subpassDependency)
{
	mSubpassDependencies.push_back(subpassDependency);
}

void VulkanRenderPass::setDependencySrcSubpass(uint32_t srcSubpass)
{
	mSubpassDependencies.back().srcSubpass = srcSubpass;
}

void VulkanRenderPass::setDependencyDstSubpass(uint32_t dstSubpass)
{
	mSubpassDependencies.back().dstSubpass = dstSubpass;
}

void VulkanRenderPass::setDependencySrcStageMask(VkPipelineStageFlags srcStageMask)
{
	mSubpassDependencies.back().srcStageMask = srcStageMask;
}

void VulkanRenderPass::setDependencyDstStageMask(VkPipelineStageFlags dstStageMask)
{
	mSubpassDependencies.back().dstStageMask = dstStageMask;
}

void VulkanRenderPass::setDependencySrcAccessMask(VkAccessFlags srcAccessMask)
{
	mSubpassDependencies.back().srcAccessMask = srcAccessMask;
}

void VulkanRenderPass::setDependencyDstAccessMask(VkAccessFlags dstAccessMask)
{
	mSubpassDependencies.back().dstAccessMask = dstAccessMask;
}

void VulkanRenderPass::setDependencyFlags(VkDependencyFlags dependencyFlags)
{
	mSubpassDependencies.back().dependencyFlags = dependencyFlags;
}

bool VulkanRenderPass::create()
{
	VkRenderPassCreateInfo renderPassCreateInfo = {
		VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO,                // VkStructureType                    sType
		nullptr,                                                  // const void                       * pNext
		0,                                                        // VkRenderPassCreateFlags            flags
		static_cast<uint32_t>(mAttachmentDescriptions.size()),    // uint32_t                           attachmentCount
		mAttachmentDescriptions.data(),                           // const VkAttachmentDescription    * pAttachments
		static_cast<uint32_t>(mSubpassDescriptions.size()),       // uint32_t                           subpassCount
		mSubpassDescriptions.data(),                              // const VkSubpassDescription       * pSubpasses
		static_cast<uint32_t>(mSubpassDependencies.size()),       // uint32_t                           dependencyCount
		mSubpassDependencies.data()                               // const VkSubpassDependency        * pDependencies
	};

	VkResult result = vkCreateRenderPass(getDeviceHandle(), &renderPassCreateInfo, nullptr, &mRenderPass);
	if (result != VK_SUCCESS || mRenderPass == VK_NULL_HANDLE)
	{
		// TODO : Use Numea System Log
		printf("Could not create a render pass\n");
		return false;
	}
	return true;
}

bool VulkanRenderPass::destroy()
{
	if (mRenderPass != VK_NULL_HANDLE)
	{
		vkDestroyRenderPass(getDeviceHandle(), mRenderPass, nullptr);
		mRenderPass = VK_NULL_HANDLE;
		return true;
	}
	return false;
}

bool VulkanRenderPass::isCreated() const
{
	return mRenderPass != VK_NULL_HANDLE;
}

const VkRenderPass& VulkanRenderPass::getHandle() const
{
	return mRenderPass;
}

VulkanFramebufferPtr VulkanRenderPass::createFramebuffer(const std::vector<VkImageView>& attachments, uint32_t width, uint32_t height, uint32_t layers)
{
	return VulkanFramebuffer::createFramebuffer(getDevice(), *this, attachments, width, height, layers);
}

VulkanRenderPass::VulkanRenderPass()
	: mRenderPass(VK_NULL_HANDLE)
	, mAttachmentDescriptions()
	, mSubpassDescriptions()
	, mSubpassDependencies()
	, mColorAttachmentReferences()
	, mNumColorAttachmentReferences(0)
	, mInputAttachmentReferences()
	, mNumInputAttachmentReferences(0)
	, mDepthStencilAttachmentReference(nullptr)
{
	VULKAN_OBJECTTRACKER_REGISTER();
}

VULKAN_NAMESPACE_END