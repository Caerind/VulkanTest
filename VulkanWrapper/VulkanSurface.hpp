#ifndef NU_VULKAN_SURFACE_HPP
#define NU_VULKAN_SURFACE_HPP

#include <memory>

#include "VulkanFunctions.hpp"

namespace nu
{
namespace Vulkan
{

struct WindowParameters
{
	PlatformConnectionType platformConnection;
	PlatformWindowType platformWindow;
};

class Instance;
class Surface
{
	public:
		typedef std::unique_ptr<Surface> Ptr;

		~Surface();

		const WindowParameters& getWindowParameters() const;
		PlatformConnectionType getPlatformConnection() const;
		PlatformWindowType getPlatformWindow() const;

		Instance& getInstance();
		const Instance& getInstance() const;
		const VkSurfaceKHR& getHandle() const;
		const VkInstance& getInstanceHandle() const;

	private:
		friend class Instance;
		static Surface::Ptr createSurface(Instance& instance, const WindowParameters& windowParameters);

		Surface(Instance& instance, const WindowParameters& windowParameters);

		bool init();
		void release();

		Instance& mInstance;
		VkSurfaceKHR mSurface;

		WindowParameters mWindowParameters;
};

} // namespace Vulkan
} // namespace nu

#endif // NU_VULKAN_SURFACE_HPP