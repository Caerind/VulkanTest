#ifndef NU_VULKAN_MEMORY_BLOCK_HPP
#define NU_VULKAN_MEMORY_BLOCK_HPP

#include <memory>

#include "VulkanFunctions.hpp"

// TODO : Can multiple memory range from the same memory block be mapped at the same time ? -> If yes, instead of the variables, use a vector. It will be easier to flush everything at the same time
// TODO : Control over flushing
// TODO : Read/Write only a subpart of mapped memory
// TODO : Binding on MemoryBlock side or Image/Buffer side ?

namespace nu
{
namespace Vulkan
{

class Device;
class Buffer;
class Image;
class MemoryBlock
{
	public:
		typedef std::unique_ptr<MemoryBlock> Ptr;

		~MemoryBlock();

		bool map(VkDeviceSize offset, VkDeviceSize size);
		bool read(void* data);
		bool write(const void* data);
		bool unmap();

		bool bind(Buffer* buffer, VkDeviceSize memoryOffset);
		bool bind(Image* image, VkDeviceSize memoryOffset);

		bool isHostVisible() const;
		bool isDeviceLocal() const;
		bool isHostCoherent() const;

		VkDeviceSize getSize() const;
		VkDeviceSize getAlignment() const;
		uint32_t getMemoryType() const;
		VkMemoryPropertyFlags getMemoryProperties() const;

		bool isMapped() const;
		VkDeviceSize getMappedOffset() const;
		VkDeviceSize getMappedSize() const;

		Device& getDevice();
		const Device& getDevice() const;
		const VkDeviceMemory& getHandle() const;
		const VkDevice& getDeviceHandle() const;

	private:
		friend class Device;
		static MemoryBlock::Ptr createMemoryBlock(Device& device, VkMemoryRequirements memoryRequirements, VkMemoryPropertyFlags memoryProperties);

		MemoryBlock(Device& device, VkMemoryRequirements memoryRequirements, VkMemoryPropertyFlags memoryProperties);

		bool init();
		void release();

	private:
		Device& mDevice;
		VkDeviceMemory mMemoryBlock;
		VkMemoryRequirements mMemoryRequirements;
		VkMemoryPropertyFlags mMemoryProperties;

		void* mMappedData;
		VkDeviceSize mMappedOffset;
		VkDeviceSize mMappedSize;
};

} // namespace Vulkan
} // namespace nu

#endif // NU_VULKAN_BUFFER_HPP