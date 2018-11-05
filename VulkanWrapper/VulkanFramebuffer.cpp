#include "VulkanFramebuffer.hpp"

#include "VulkanDevice.hpp"

namespace nu
{
namespace Vulkan
{

Framebuffer::Ptr Framebuffer::createFramebuffer(Device& device, RenderPass& renderPass, const std::vector<VkImageView>& attachments, uint32_t width, uint32_t height, uint32_t layers)
{
	Framebuffer::Ptr framebuffer(new Framebuffer(device, renderPass, attachments, width, height, layers));
	if (framebuffer != nullptr)
	{
		if (!framebuffer->init())
		{
			framebuffer.reset();
		}
	}
	return framebuffer;
}

Framebuffer::~Framebuffer()
{
	ObjectTracker::unregisterObject(ObjectType_Framebuffer);

	release();
}

bool Framebuffer::isInitialized() const
{
	return mFramebuffer != VK_NULL_HANDLE;
}

const VkFramebuffer& Framebuffer::getHandle() const
{
	return mFramebuffer;
}

const Device& Framebuffer::getDeviceHandle() const
{
	return mDevice;
}

Framebuffer::Framebuffer(Device& device, RenderPass& renderPass, const std::vector<VkImageView>& attachments, uint32_t width, uint32_t height, uint32_t layers)
	: mDevice(device)
	, mRenderPass(renderPass)
	, mFramebuffer(VK_NULL_HANDLE)
	, mAttachments(attachments)
	, mWidth(width)
	, mHeight(height)
	, mLayers(layers)
{
	ObjectTracker::registerObject(ObjectType_Framebuffer);
}

bool Framebuffer::init()
{
	VkFramebufferCreateInfo framebufferCreateInfo = {
		VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO,    // VkStructureType              sType
		nullptr,                                      // const void                 * pNext
		0,                                            // VkFramebufferCreateFlags     flags
		mRenderPass.getHandle(),                      // VkRenderPass                 renderPass
		static_cast<uint32_t>(mAttachments.size()),   // uint32_t                     attachmentCount
		mAttachments.data(),                          // const VkImageView          * pAttachments
		mWidth,                                       // uint32_t                     width
		mHeight,                                      // uint32_t                     height
		mLayers                                       // uint32_t                     layers
	};

	VkResult result = vkCreateFramebuffer(mDevice.getHandle(), &framebufferCreateInfo, nullptr, &mFramebuffer);
	if (result != VK_SUCCESS || mFramebuffer == VK_NULL_HANDLE)
	{
		// TODO : Use Numea System Log
		printf("Could not create a framebuffer\n");
		return false;
	}

	return true;
}

bool Framebuffer::release()
{
	if (mFramebuffer != VK_NULL_HANDLE)
	{
		vkDestroyFramebuffer(mDevice.getHandle(), mFramebuffer, nullptr);
		mFramebuffer = VK_NULL_HANDLE;
		return true;
	}
	return false;
}

} // namespace Vulkan
} // namespace nu