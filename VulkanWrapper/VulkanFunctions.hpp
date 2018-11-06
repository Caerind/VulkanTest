#ifndef NU_VULKAN_FUNCTIONS_HPP
#define NU_VULKAN_FUNCTIONS_HPP

// TODO : Move this
#define VK_USE_PLATFORM_WIN32_KHR
#define VK_NO_PROTOTYPES
#include "../ThirdParty/vulkan.h"

#ifdef VK_USE_PLATFORM_WIN32_KHR
	#define PlatformConnectionType HINSTANCE
	#define PlatformWindowType HWND
#elif defined VK_USE_PLATFORM_XLIB_KHR
	#define PlatformConnectionType Display*
	#define PlatformWindowType ::Window
#elif defined VK_USE_PLATFORM_XCB_KHR
	#define PlatformConnectionType xcb_connection_t*
	#define PlatformWindowType xcb_window_t
#else
	#error Unsupported platform
#endif

#include "VulkanObjectTracker.hpp"

#include <cstdio>

namespace nu 
{
namespace Vulkan
{

#define NU_EXPORTED_VULKAN_FUNCTION(name) extern PFN_##name name;
#define NU_GLOBAL_LEVEL_VULKAN_FUNCTION(name) extern PFN_##name name;
#define NU_INSTANCE_LEVEL_VULKAN_FUNCTION(name) extern PFN_##name name;
#define NU_INSTANCE_LEVEL_VULKAN_FUNCTION_FROM_EXTENSION(name, extension) extern PFN_##name name;
#define NU_DEVICE_LEVEL_VULKAN_FUNCTION(name) extern PFN_##name name;
#define NU_DEVICE_LEVEL_VULKAN_FUNCTION_FROM_EXTENSION(name, extension) extern PFN_##name name;

#include "ListOfVulkanFunctions.inl"

} // namespace Vulkan
} // namespace nu

#endif // NU_VULKAN_FUNCTIONS_HPP