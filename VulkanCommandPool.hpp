#ifndef NU_VULKAN_COMMAND_POOL_HPP
#define NU_VULKAN_COMMAND_POOL_HPP

#include "VulkanFunctions.hpp"
#include "VulkanCommandBuffer.hpp"

#include <memory>

namespace nu
{
namespace Vulkan
{

class Device;
class CommandPool
{
	public:
		typedef std::unique_ptr<CommandPool> Ptr;

		// TODO : Store settings as attributes
		static CommandPool::Ptr createCommandPool(Device& device, VkCommandPoolCreateFlags parameters, uint32_t queueFamily);

		~CommandPool();

		CommandBuffer::Ptr allocatePrimaryCommandBuffer();
		CommandBuffer::Ptr allocateSecondaryCommandBuffer();
		// TODO : Allocate n at once
		
		bool reset(); // TODO : Add release resources parameter ?

		// TODO : Free command buffers ?

		bool isInitialized() const;
		const VkCommandPool& getHandle() const;
		const VkDevice& getDeviceHandle() const;

	private:
		CommandPool(Device& device);

		bool init(VkCommandPoolCreateFlags parameters, uint32_t queueFamily);
		bool release();

		VkCommandPool mCommandPool;
		Device& mDevice;
};

} // namespace Vulkan
} // namespace nu

#endif // NU_VULKAN_COMMAND_POOL_HPP