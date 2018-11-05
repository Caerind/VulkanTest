#ifndef NU_VULKAN_MEMORY_BLOCK_HPP
#define NU_VULKAN_MEMORY_BLOCK_HPP

#include "VulkanFunctions.hpp"

#include <memory>

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

		static MemoryBlock::Ptr createMemoryBlock(Buffer* buffer, VkMemoryPropertyFlagBits memoryProperties);
		static MemoryBlock::Ptr createMemoryBlock(Image* buffer, VkMemoryPropertyFlagBits memoryProperties);

		~MemoryBlock();

		bool mapUpdateAndUnmapHostVisibleMemory(VkDeviceSize offset, VkDeviceSize dataSize, void* data, bool unmap, void** pointer);

		bool isInitialized() const;
		const VkDeviceMemory& getHandle() const;
		const VkDevice& getDeviceHandle() const;

	private:
		MemoryBlock(Device& device, Buffer* buffer, VkMemoryPropertyFlagBits memoryProperties);
		MemoryBlock(Device& device, Image* image, VkMemoryPropertyFlagBits memoryProperties);

		bool init();
		bool release();

		Device& mDevice;
		Buffer* mBuffer;
		Image* mImage;
		VkDeviceMemory mMemoryBlock;
		VkMemoryPropertyFlagBits mMemoryProperties;
	};

} // namespace Vulkan
} // namespace nu

#endif // NU_VULKAN_BUFFER_HPP