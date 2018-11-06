#ifndef NU_VULKAN_BUFFER_HPP
#define NU_VULKAN_BUFFER_HPP

#include <memory>
#include <vector>

#include "VulkanFunctions.hpp"

#include "VulkanMemoryBlock.hpp"
#include "VulkanBufferView.hpp"

// TODO : Binding on MemoryBlock side or Image/Buffer side ?

namespace nu
{
namespace Vulkan
{

class Device;
class Buffer
{
	public:
		typedef std::unique_ptr<Buffer> Ptr;

		~Buffer();

		const VkMemoryRequirements& getMemoryRequirements() const;

		MemoryBlock* allocateMemoryBlock(VkMemoryPropertyFlagBits memoryProperties);
		bool ownMemoryBlock() const;
		bool isBoundToMemoryBlock() const;
		MemoryBlock* getMemoryBlock();
		VkDeviceSize getOffsetInMemoryBlock() const;

		BufferView* createBufferView(VkFormat format, VkDeviceSize memoryOffset, VkDeviceSize memoryRange);
		BufferView* getBufferView(uint32_t index);
		const BufferView* getBufferView(uint32_t index) const;
		uint32_t getBufferViewCount() const;
		void clearBufferViews();

		VkDeviceSize getSize() const;
		VkBufferUsageFlags getUsage() const;

		Device& getDevice();
		const Device& getDevice() const;
		const VkBuffer& getHandle() const;
		const VkDevice& getDeviceHandle() const;

	private:
		friend class Device;
		static Buffer::Ptr createBuffer(Device& device, VkDeviceSize size, VkBufferUsageFlags usage);

		Buffer(Device& device, VkDeviceSize size, VkBufferUsageFlags usage);

		bool init();
		void release();

		friend class MemoryBlock;
		void bindToMemoryBlock(MemoryBlock* memoryBlock, VkDeviceSize offsetInMemoryBlock);

		Device& mDevice;
		VkBuffer mBuffer;

		MemoryBlock::Ptr mOwnedMemoryBlock;
		MemoryBlock* mMemoryBlock;
		VkDeviceSize mOffsetInMemoryBlock;

		std::vector<BufferView::Ptr> mBufferViews;

		VkDeviceSize mSize;
		VkBufferUsageFlags mUsage;

		mutable bool mMemoryRequirementsQueried;
		mutable VkMemoryRequirements mMemoryRequirements;
};

} // namespace Vulkan
} // namespace nu

#endif // NU_VULKAN_BUFFER_HPP