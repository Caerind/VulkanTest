#ifndef NU_VULKAN_FRAMEBUFFER_HPP
#define NU_VULKAN_FRAMEBUFFER_HPP

#include "VulkanFunctions.hpp"

#include <memory>
#include <vector>

namespace nu
{
namespace Vulkan
{

class Device;
class RenderPass;
class Framebuffer
{
	public:
		typedef std::unique_ptr<Framebuffer> Ptr;

		static Framebuffer::Ptr createFramebuffer(Device& device, RenderPass& renderPass, const std::vector<VkImageView>& attachments, uint32_t width, uint32_t height, uint32_t layers);

		~Framebuffer();

		bool isInitialized() const;
		const VkFramebuffer& getHandle() const;
		const VkDevice& getDeviceHandle() const;

	private:
		Framebuffer(Device& device, RenderPass& renderPass, const std::vector<VkImageView>& attachments, uint32_t width, uint32_t height, uint32_t layers);

		bool init();
		bool release();

		Device& mDevice;
		RenderPass& mRenderPass;
		VkFramebuffer mFramebuffer;

		std::vector<VkImageView> mAttachments;
		uint32_t mWidth;
		uint32_t mHeight;
		uint32_t mLayers;
};

} // namespace Vulkan
} // namespace nu

#endif // NU_VULKAN_FRAMEBUFFER_HPP