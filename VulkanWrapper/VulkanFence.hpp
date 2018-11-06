#ifndef NU_VULKAN_FENCE_HPP
#define NU_VULKAN_FENCE_HPP

#include <memory>

#include "VulkanFunctions.hpp"

namespace nu
{
namespace Vulkan
{

class Device;
class Fence
{
	public:
		typedef std::unique_ptr<Fence> Ptr;

		~Fence();

		bool wait(uint64_t timeout);

		bool reset();

		bool isSignaled() const;

		Device& getDevice();
		const Device& getDevice() const;
		const VkFence& getHandle() const;
		const VkDevice& getDeviceHandle() const;

	private:
		friend class Device;
		static Fence::Ptr createFence(Device& device, bool signaled);

		Fence(Device& device, bool signaled);

		bool init();
		void release();

	private:
		Device& mDevice;
		VkFence mFence;

		bool mSignaled;
};

} // namespace Vulkan
} // namespace nu

#endif // NU_VULKAN_FENCE_HPP