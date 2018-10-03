#ifndef NU_VULKAN_FENCE_HPP
#define NU_VULKAN_FENCE_HPP

#include "VulkanFunctions.hpp"

#include <memory>

namespace nu
{
namespace Vulkan
{

class Device;
class Fence
{
	public:
		typedef std::unique_ptr<Fence> Ptr;

		static Fence::Ptr createFence(Device& device, bool signaled);

		~Fence();

		bool isSignaled() const;

		bool wait(uint64_t timeout);

		bool reset();

		bool isInitialized() const;
		const VkFence& getHandle() const;
		const Device& getDeviceHandle() const;

	private:
		Fence(Device& device, bool signaled);

		bool init();
		bool release();

		Device& mDevice;
		VkFence mFence;
		bool mSignaled;
};

} // namespace Vulkan
} // namespace nu

#endif // NU_VULKAN_FENCE_HPP