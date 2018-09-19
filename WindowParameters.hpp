#ifndef NU_WINDOW_PARAMETERS_HPP
#define NU_WINDOW_PARAMETERS_HPP

#include "System/Prerequisites.hpp"
#include "VulkanFunctions.hpp"

namespace nu
{

struct WindowParameters
{
	#if defined(VK_USE_PLATFORM_WIN32_KHR)
		HINSTANCE hinstance;
		HWND hwnd;
	#elif defined(VK_USE_PLATFORM_XLIB_KHR)
		Display* dpy;
		Window window;
	#elif defined(VK_USE_PLATFORM_XCB_KHR)
		xcb_connection_t* connection;
		xcb_window_t window;
	#endif
};

} // namespace nu

#endif // NU_WINDOW_PARAMETERS_HPP