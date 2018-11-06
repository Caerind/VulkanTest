#ifndef NU_VULKAN_BUFFER_VIEW_HPP
#define NU_VULKAN_BUFFER_VIEW_HPP

#include <memory>

#include "VulkanFunctions.hpp"

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

		~BufferView();

		VkFormat getFormat() const;
		VkDeviceSize getOffset() const;
		VkDeviceSize getSize() const;

		Device& getDevice();
		const Device& getDevice() const;
		const VkBufferView& getHandle() const;
		const VkBuffer& getBufferHandle() const;
		const VkDevice& getDeviceHandle() const;

	private:
		friend class Buffer;
		static BufferView::Ptr createBufferView(Buffer& buffer, VkFormat format, VkDeviceSize offset, VkDeviceSize size);

		BufferView(Buffer& buffer, VkFormat format, VkDeviceSize offset, VkDeviceSize size);

		bool init();
		void release();

		Device& mDevice;
		Buffer& mBuffer;
		VkBufferView mBufferView;

		VkFormat mFormat;
		VkDeviceSize mOffset;
		VkDeviceSize mSize;
};

} // namespace Vulkan
} // namespace nu

#endif // NU_VULKAN_BUFFER_VIEW_HPP