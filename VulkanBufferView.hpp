#ifndef NU_VULKAN_BUFFER_VIEW_HPP
#define NU_VULKAN_BUFFER_VIEW_HPP

#include "VulkanFunctions.hpp"

#include <memory>

namespace nu
{
namespace Vulkan
{

class Device;
class Buffer;
class BufferView
{
	public:
		typedef std::unique_ptr<BufferView> Ptr;

		static BufferView::Ptr createBufferView(Device& device, Buffer& buffer, VkFormat format, VkDeviceSize memoryOffset, VkDeviceSize memoryRange);

		~BufferView();

		bool isInitialized() const;
		const VkBufferView& getHandle() const;
		const VkBuffer& getBufferHandle() const;
		const VkDevice& getDeviceHandle() const;

	private:
		BufferView(Device& device, Buffer& buffer, VkFormat format, VkDeviceSize memoryOffset, VkDeviceSize memoryRange);

		bool init();
		bool release();

		Device& mDevice;
		Buffer& mBuffer;
		VkBufferView mBufferView;

		VkFormat mFormat;
		VkDeviceSize mMemoryOffset;
		VkDeviceSize mMemoryRange;
};

} // namespace Vulkan
} // namespace nu

#endif // NU_VULKAN_BUFFER_VIEW_HPP