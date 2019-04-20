#include "VulkanSurface.hpp"

#include "VulkanInstance.hpp"

VULKAN_NAMESPACE_BEGIN

VulkanSurface::~VulkanSurface()
{
	release();

	VULKAN_OBJECTTRACKER_UNREGISTER();
}

const VulkanWindowParameters& VulkanSurface::getWindowParameters() const
{
	return mWindowParameters;
}

VulkanPlatformConnectionType VulkanSurface::getPlatformConnection() const
{
	return mWindowParameters.platformConnection;
}

VulkanPlatformWindowType VulkanSurface::getPlatformWindow() const
{
	return mWindowParameters.platformWindow;
}

const VkSurfaceKHR& VulkanSurface::getHandle() const
{
	return mSurface;
}

VulkanSurfacePtr VulkanSurface::createSurface(const VulkanWindowParameters& windowParameters)
{
	VulkanSurfacePtr surface(new VulkanSurface(windowParameters));
	if (surface != nullptr)
	{
		if (!surface->init())
		{
			surface.reset();
		}
	}
	return surface;
}

VulkanSurface::VulkanSurface(const VulkanWindowParameters& windowParameters)
	: mSurface(VK_NULL_HANDLE)
	, mWindowParameters(windowParameters)
{
	VULKAN_OBJECTTRACKER_REGISTER();
}

bool VulkanSurface::init()
{
	VkResult result;

	#if defined(VK_USE_PLATFORM_WIN32_KHR)
		VkWin32SurfaceCreateInfoKHR surfaceCreateInfo = {
			VK_STRUCTURE_TYPE_WIN32_SURFACE_CREATE_INFO_KHR,  // VkStructureType                 sType
			nullptr,                                          // const void                    * pNext
			0,                                                // VkWin32SurfaceCreateFlagsKHR    flags
			mWindowParameters.platformConnection,             // HINSTANCE                       hinstance
			mWindowParameters.platformWindow                  // HWND                            hwnd
		};

		result = vkCreateWin32SurfaceKHR(getInstanceHandle(), &surfaceCreateInfo, nullptr, &mSurface);

	#elif defined(VK_USE_PLATFORM_XLIB_KHR)
		VkXlibSurfaceCreateInfoKHR surfaceCreateInfo = {
			VK_STRUCTURE_TYPE_XLIB_SURFACE_CREATE_INFO_KHR,   // VkStructureType                 sType
			nullptr,                                          // const void                    * pNext
			0,                                                // VkXlibSurfaceCreateFlagsKHR     flags
			mWindowParameters.platformConnection,             // Display                       * dpy
			mWindowParameters.platformWindow                  // Window                          window
		};

		result = vkCreateXlibSurfaceKHR(getInstanceHandle(), &surfaceCreateInfo, nullptr, &mSurface);

	#elif defined(VK_USE_PLATFORM_XCB_KHR)
		VkXcbSurfaceCreateInfoKHR surfaceCreateInfo = {
			VK_STRUCTURE_TYPE_XCB_SURFACE_CREATE_INFO_KHR,    // VkStructureType                 sType
			nullptr,                                          // const void                    * pNext
			0,                                                // VkXcbSurfaceCreateFlagsKHR      flags
			windowParameters.platformConnection,              // xcb_connection_t              * connection
			mWindowParameters.platformWindow                  // xcb_window_t                    window
		};

		result = vkCreateXcbSurfaceKHR(getInstanceHandle(), &surfaceCreateInfo, nullptr, &mSurface);
	#else
		VULKAN_LOG_ERROR("Unsupported platform for presentation surface");
		return false;
	#endif

	if (result != VK_SUCCESS || mSurface == VK_NULL_HANDLE)
	{
		VULKAN_LOG_ERROR("Could not create presentation surface");
		return false;
	}

	return true;
}

void VulkanSurface::release()
{
	if (mSurface != VK_NULL_HANDLE)
	{
		vkDestroySurfaceKHR(getInstanceHandle(), mSurface, nullptr);
		mSurface = VK_NULL_HANDLE;
	}
}

VULKAN_NAMESPACE_END