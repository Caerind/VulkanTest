#include "VulkanFramebuffer.hpp"

#include "VulkanDevice.hpp"
#include "VulkanRenderPass.hpp"

VULKAN_NAMESPACE_BEGIN

VulkanFramebufferPtr VulkanFramebuffer::createFramebuffer(VulkanDevice& device, VulkanRenderPass& renderPass, const std::vector<VkImageView>& attachments, VulkanU32 width, VulkanU32 height, VulkanU32 layers)
{
	VulkanFramebufferPtr framebuffer(new VulkanFramebuffer(device, renderPass, attachments, width, height, layers));
	if (framebuffer != nullptr)
	{
		if (!framebuffer->init())
		{
			framebuffer.reset();
		}
	}
	return framebuffer;
}

VulkanFramebuffer::~VulkanFramebuffer()
{
	release();

	VULKAN_OBJECTTRACKER_UNREGISTER();
}

bool VulkanFramebuffer::isInitialized() const
{
	return mFramebuffer != VK_NULL_HANDLE;
}

const VkFramebuffer& VulkanFramebuffer::getHandle() const
{
	return mFramebuffer;
}

const VkDevice& VulkanFramebuffer::getDeviceHandle() const
{
	return mDevice.getHandle();
}

VulkanFramebuffer::VulkanFramebuffer(VulkanDevice& device, VulkanRenderPass& renderPass, const std::vector<VkImageView>& attachments, VulkanU32 width, VulkanU32 height, VulkanU32 layers)
	: mDevice(device)
	, mRenderPass(renderPass)
	, mFramebuffer(VK_NULL_HANDLE)
	, mAttachments(attachments)
	, mWidth(width)
	, mHeight(height)
	, mLayers(layers)
{
	VULKAN_OBJECTTRACKER_REGISTER();
}

bool VulkanFramebuffer::init()
{
	VkFramebufferCreateInfo framebufferCreateInfo = {
		VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO,    // VkStructureType              sType
		nullptr,                                      // const void                 * pNext
		0,                                            // VkFramebufferCreateFlags     flags
		mRenderPass.getHandle(),                      // VkRenderPass                 renderPass
		static_cast<VulkanU32>(mAttachments.size()),  // uint32_t                     attachmentCount
		mAttachments.data(),                          // const VkImageView          * pAttachments
		mWidth,                                       // uint32_t                     width
		mHeight,                                      // uint32_t                     height
		mLayers                                       // uint32_t                     layers
	};

	VkResult result = vkCreateFramebuffer(mDevice.getHandle(), &framebufferCreateInfo, nullptr, &mFramebuffer);
	if (result != VK_SUCCESS || mFramebuffer == VK_NULL_HANDLE)
	{
		VULKAN_LOG_ERROR("Could not create framebuffer");
		return false;
	}

	return true;
}

bool VulkanFramebuffer::release()
{
	if (mFramebuffer != VK_NULL_HANDLE)
	{
		vkDestroyFramebuffer(mDevice.getHandle(), mFramebuffer, nullptr);
		mFramebuffer = VK_NULL_HANDLE;
		return true;
	}
	return false;
}

VULKAN_NAMESPACE_END