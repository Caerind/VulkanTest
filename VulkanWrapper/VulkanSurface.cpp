#include "VulkanSurface.hpp"

#include "VulkanInstance.hpp"

namespace nu
{
namespace Vulkan
{

Surface::~Surface()
{
	release();

	ObjectTracker::unregisterObject(ObjectType_Surface);
}

const WindowParameters& Surface::getWindowParameters() const
{
	return mWindowParameters;
}

PlatformConnectionType Surface::getPlatformConnection() const
{
	return mWindowParameters.platformConnection;
}

PlatformWindowType Surface::getPlatformWindow() const
{
	return mWindowParameters.platformWindow;
}

Instance& Surface::getInstance()
{
	return mInstance;
}

const Instance& Surface::getInstance() const
{
	return mInstance;
}

const VkSurfaceKHR& Surface::getHandle() const
{
	return mSurface;
}

const VkInstance& Surface::getInstanceHandle() const
{
	return mInstance.getHandle();
}

Surface::Ptr Surface::createSurface(Instance& instance, const WindowParameters& windowParameters)
{
	Surface::Ptr surface(new Surface(instance, windowParameters));
	if (surface != nullptr)
	{
		if (!surface->init())
		{
			surface.reset();
		}
	}
	return surface;
}

Surface::Surface(Instance& instance, const WindowParameters& windowParameters)
	: mInstance(instance)
	, mSurface(VK_NULL_HANDLE)
	, mWindowParameters(windowParameters)
{
	ObjectTracker::registerObject(ObjectType_Surface);
}

bool Surface::init()
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

		result = vkCreateWin32SurfaceKHR(mInstance.getHandle(), &surfaceCreateInfo, nullptr, &mSurface);

	#elif defined(VK_USE_PLATFORM_XLIB_KHR)
		VkXlibSurfaceCreateInfoKHR surfaceCreateInfo = {
			VK_STRUCTURE_TYPE_XLIB_SURFACE_CREATE_INFO_KHR,   // VkStructureType                 sType
			nullptr,                                          // const void                    * pNext
			0,                                                // VkXlibSurfaceCreateFlagsKHR     flags
			mWindowParameters.platformConnection,             // Display                       * dpy
			mWindowParameters.platformWindow                  // Window                          window
		};

		result = vkCreateXlibSurfaceKHR(mInstance.getHandle(), &surfaceCreateInfo, nullptr, &mSurface);

	#elif defined(VK_USE_PLATFORM_XCB_KHR)
		VkXcbSurfaceCreateInfoKHR surfaceCreateInfo = {
			VK_STRUCTURE_TYPE_XCB_SURFACE_CREATE_INFO_KHR,    // VkStructureType                 sType
			nullptr,                                          // const void                    * pNext
			0,                                                // VkXcbSurfaceCreateFlagsKHR      flags
			windowParameters.platformConnection,              // xcb_connection_t              * connection
			mWindowParameters.platformWindow                  // xcb_window_t                    window
		};

		result = vkCreateXcbSurfaceKHR(mInstance.getHandle(), &surfaceCreateInfo, nullptr, &mSurface);
	#else
		// TODO : Use Numea System Log
		printf("Unsupported platform for presentation surface\n");
		return false;
	#endif

	if (result != VK_SUCCESS || mSurface == VK_NULL_HANDLE)
	{
		// TODO : Use Numea System Log
		printf("Could not create presentation surface\n");
		return false;
	}

	return true;
}

void Surface::release()
{
	if (mSurface != VK_NULL_HANDLE)
	{
		vkDestroySurfaceKHR(mInstance.getHandle(), mSurface, nullptr);
		mSurface = VK_NULL_HANDLE;
	}
}

} // namespace Vulkan
} // namespace nu