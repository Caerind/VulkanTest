#include "VulkanSurface.hpp"

#include "VulkanInstance.hpp"

namespace nu
{
namespace Vulkan
{

Surface::Ptr Surface::createSurface(Instance& instance, const WindowParameters& windowParameters)
{
	Surface::Ptr surface(new Surface(instance));
	if (surface != nullptr)
	{
		if (!surface->init(windowParameters))
		{
			surface.reset();
		}
	}
	return surface;
}

Surface::~Surface()
{
	ObjectTracker::unregisterObject(ObjectType_Surface);

	release();
}

bool Surface::isInitialized() const
{
	return mSurface != VK_NULL_HANDLE;
}

const VkSurfaceKHR& Surface::getHandle() const
{
	return mSurface;
}

const VkInstance& Surface::getInstanceHandle() const
{
	return mInstance.getHandle();
}

Surface::Surface(Instance& instance) 
	: mSurface(VK_NULL_HANDLE)
	, mInstance(instance)
{
	ObjectTracker::registerObject(ObjectType_Surface);
}

bool Surface::init(const WindowParameters& windowParameters)
{
	VkResult result = VK_INCOMPLETE;

	#if defined(VK_USE_PLATFORM_WIN32_KHR)
		VkWin32SurfaceCreateInfoKHR surfaceCreateInfo = {
			VK_STRUCTURE_TYPE_WIN32_SURFACE_CREATE_INFO_KHR,  // VkStructureType                 sType
			nullptr,                                          // const void                    * pNext
			0,                                                // VkWin32SurfaceCreateFlagsKHR    flags
			windowParameters.hinstance,                       // HINSTANCE                       hinstance
			windowParameters.hwnd                             // HWND                            hwnd
		};

		result = vkCreateWin32SurfaceKHR(mInstance.getHandle(), &surfaceCreateInfo, nullptr, &mSurface);

	#elif defined(VK_USE_PLATFORM_XLIB_KHR)
		VkXlibSurfaceCreateInfoKHR surfaceCreateInfo = {
			VK_STRUCTURE_TYPE_XLIB_SURFACE_CREATE_INFO_KHR,   // VkStructureType                 sType
			nullptr,                                          // const void                    * pNext
			0,                                                // VkXlibSurfaceCreateFlagsKHR     flags
			windowParameters.dpy,                             // Display                       * dpy
			windowParameters.window                           // Window                          window
		};

		result = vkCreateXlibSurfaceKHR(mInstance.getHandle(), &surfaceCreateInfo, nullptr, &mSurface);

	#elif defined(VK_USE_PLATFORM_XCB_KHR)
		VkXcbSurfaceCreateInfoKHR surfaceCreateInfo = {
			VK_STRUCTURE_TYPE_XCB_SURFACE_CREATE_INFO_KHR,    // VkStructureType                 sType
			nullptr,                                          // const void                    * pNext
			0,                                                // VkXcbSurfaceCreateFlagsKHR      flags
			windowParameters.connection,                      // xcb_connection_t              * connection
			windowParameters.window                           // xcb_window_t                    window
		};

		result = vkCreateXcbSurfaceKHR(mInstance.getHandle(), &surfaceCreateInfo, nullptr, &mSurface);

	#endif

	if (result == VK_INCOMPLETE)
	{
		// TODO : Use Numea System Log
		printf("Could not find platform for presentation surface\n");
		return false;
	}

	if (result != VK_SUCCESS || mSurface == VK_NULL_HANDLE)
	{
		// TODO : Use Numea System Log
		printf("Could not create presentation surface\n");
		return false;
	}

	return true;
}

bool Surface::release()
{
	if (mSurface != VK_NULL_HANDLE)
	{
		vkDestroySurfaceKHR(mInstance.getHandle(), mSurface, nullptr);
		mSurface = VK_NULL_HANDLE;
		return true;
	}
	return false;
}

} // namespace Vulkan
} // namespace nu