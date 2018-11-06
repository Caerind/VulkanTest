#ifndef NU_VULKAN_SEMAPHORE_HPP
#define NU_VULKAN_SEMAPHORE_HPP

#include <memory>

#include "VulkanFunctions.hpp"

// TODO : Add getters

namespace nu
{
namespace Vulkan
{

class Device;
class Semaphore
{
	public:
		typedef std::unique_ptr<Semaphore> Ptr;

		~Semaphore();

		Device& getDevice();
		const Device& getDevice() const;
		const VkSemaphore& getHandle() const;
		const VkDevice& getDeviceHandle() const;

	private:
		friend class Device;
		static Semaphore::Ptr createSemaphore(Device& device);

		Semaphore(Device& device);

		bool init();
		void release();

	private:
		Device& mDevice;
		VkSemaphore mSemaphore;
};

} // namespace Vulkan
} // namespace nu

#endif // NU_VULKAN_SEMAPHORE_HPP