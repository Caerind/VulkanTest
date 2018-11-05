#ifndef NU_VULKAN_SURFACE_HPP
#define NU_VULKAN_SURFACE_HPP

#include "VulkanFunctions.hpp"

#include <memory>

namespace nu
{
namespace Vulkan
{

struct WindowParameters
{
	#ifdef VK_USE_PLATFORM_WIN32_KHR
		HINSTANCE          hinstance;
		HWND               hwnd;
	#elif defined VK_USE_PLATFORM_XLIB_KHR
		Display          * dpy;
		Window             window;
	#elif defined VK_USE_PLATFORM_XCB_KHR
		xcb_connection_t * connection;
		xcb_window_t       window;
	#endif
};

class Instance;
class Surface
{
	public:
		typedef std::unique_ptr<Surface> Ptr;

		static Surface::Ptr createSurface(Instance& instance, const WindowParameters& windowParameters);

		~Surface();

		bool isInitialized() const;
		const VkSurfaceKHR& getHandle() const;
		const VkInstance& getInstanceHandle() const;

	private:
		Surface(Instance& instance);

		bool init(const WindowParameters& windowParameters);
		bool release();

		VkSurfaceKHR mSurface;
		Instance& mInstance;
};

} // namespace Vulkan
} // namespace nu

#endif // NU_VULKAN_SURFACE_HPP