#ifndef NU_VULKAN_SEMAPHORE_HPP
#define NU_VULKAN_SEMAPHORE_HPP

#include "VulkanFunctions.hpp"

#include <memory>

namespace nu
{
namespace Vulkan
{

class Device;
class Semaphore
{
	public:
		typedef std::unique_ptr<Semaphore> Ptr;

		static Semaphore::Ptr createSemaphore(Device& device);

		~Semaphore();

		bool isInitialized() const;
		const VkSemaphore& getHandle() const;
		const Device& getDeviceHandle() const;

	private:
		Semaphore(Device& device);

		bool init();
		bool release();

		Device& mDevice;
		VkSemaphore mSemaphore;
};

} // namespace Vulkan
} // namespace nu

#endif // NU_VULKAN_SEMAPHORE_HPP