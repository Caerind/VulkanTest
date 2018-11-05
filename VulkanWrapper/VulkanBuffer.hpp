#ifndef NU_VULKAN_BUFFER_HPP
#define NU_VULKAN_BUFFER_HPP

#include "VulkanFunctions.hpp"

#include "VulkanMemoryBlock.hpp"
#include "VulkanBufferView.hpp"

#include <memory>

namespace nu
{
namespace Vulkan
{

class Device;
class Buffer
{
	public:
		typedef std::unique_ptr<Buffer> Ptr;

		static Buffer::Ptr createBuffer(Device& device, VkDeviceSize size, VkBufferUsageFlags usage);

		~Buffer();

		const VkMemoryRequirements& getMemoryRequirements() const;

		MemoryBlock::Ptr allocateAndBindMemoryBlock(VkMemoryPropertyFlagBits memoryProperties);

		BufferView::Ptr createBufferView(VkFormat format, VkDeviceSize memoryOffset, VkDeviceSize memoryRange);

		bool isInitialized() const;
		const VkBuffer& getHandle() const;
		const VkDevice& getDeviceHandle() const;
		Device& getDevice();
		const Device& getDevice() const;

	private:
		Buffer(Device& device);

		bool init(VkDeviceSize size, VkBufferUsageFlags usage);
		bool release();

		Device& mDevice;
		VkBuffer mBuffer;

		mutable bool mMemoryRequirementsQueried;
		mutable VkMemoryRequirements mMemoryRequirements;
};

} // namespace Vulkan
} // namespace nu

#endif // NU_VULKAN_BUFFER_HPP