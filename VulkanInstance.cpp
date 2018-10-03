#include "VulkanInstance.hpp"

namespace nu
{
namespace Vulkan
{

Instance::Ptr Instance::createInstance()
{
	Instance::Ptr instance(new Instance());
	if (instance != nullptr)
	{
		if (!instance->init())
		{
			instance.reset();
		}
	}
	return instance;
}

Instance::~Instance()
{
	ObjectTracker::unregisterObject(ObjectType_Instance);

	release();
}

PhysicalDevice& Instance::getPhysicalDevice(uint32_t index)
{
	return mPhysicalDevices[index];
}

const PhysicalDevice& Instance::getPhysicalDevice(uint32_t index) const
{
	return mPhysicalDevices[index];
}

const std::vector<PhysicalDevice>& Instance::getPhysicalDevices() const
{
	if (mPhysicalDevices.size() == 0)
	{
		queryAvailablePhysicalDevices();
	}

	return mPhysicalDevices;
}

uint32_t Instance::getPhysicalDeviceCount() const
{
	if (mPhysicalDevices.size() == 0)
	{
		queryAvailablePhysicalDevices();
	}

	return (uint32_t)mPhysicalDevices.size();
}

Surface::Ptr Instance::createSurface(const WindowParameters& windowParameters)
{
	return Surface::createSurface(*this, windowParameters);
}

bool Instance::isInitialized() const
{
	return mInstance != VK_NULL_HANDLE;
}

const VkInstance& Instance::getHandle() const
{
	return mInstance;
}

Instance::Instance()
	: mInstance(VK_NULL_HANDLE)
	, mDebugReportCallback(VK_NULL_HANDLE)
	, mPhysicalDevices()
{
	ObjectTracker::registerObject(ObjectType_Instance);
}

bool Instance::init()
{
	if (!Loader::areDynamicExportedGlobalLoaded())
	{
		if (!Loader::loadDynamicExportedGlobal())
		{
			return false;
		}
	}

	std::vector<const char*> desiredExtensions;
	// TODO : Only if wanted
	if (true)
	{
		desiredExtensions.emplace_back(VK_KHR_SURFACE_EXTENSION_NAME);
		#ifdef VK_USE_PLATFORM_WIN32_KHR
			desiredExtensions.emplace_back(VK_KHR_WIN32_SURFACE_EXTENSION_NAME);
		#elif defined VK_USE_PLATFORM_XCB_KHR
			desiredExtensions.emplace_back(VK_KHR_XCB_SURFACE_EXTENSION_NAME);
		#elif defined VK_USE_PLATFORM_XLIB_KHR
			desiredExtensions.emplace_back(VK_KHR_XLIB_SURFACE_EXTENSION_NAME);
		#endif
	}
	// TODO : Only on Debug
	if (true)
	{
		desiredExtensions.emplace_back(VK_EXT_DEBUG_REPORT_EXTENSION_NAME);
	}

	// TODO : Only on Debug
	if (true)
	{
		for (auto& desiredExtension : desiredExtensions)
		{
			if (!Loader::isInstanceExtensionSupported(desiredExtension))
			{
				// TODO : Numea Log System
				printf("Could not find a desired instance extension : %s\n", desiredExtension);
				return false;
			}
		}
	}

	std::vector<const char*> desiredLayers;
	// TODO : Only on Debug
	if (true)
	{
		desiredLayers.emplace_back("VK_LAYER_LUNARG_standard_validation");
	}

	// TODO : Only on Debug
	if (true)
	{
		for (auto& desiredLayer : desiredLayers)
		{
			if (!Loader::isInstanceLayerSupported(desiredLayer))
			{
				// TODO : Numea Log System
				printf("Could not find a desired instance layer : %s\n", desiredLayer);
				return false;
			}
		}
	}

	// TODO : Get application name and version
	// TODO : Get engine name and version
	VkApplicationInfo applicationInfo = {
		VK_STRUCTURE_TYPE_APPLICATION_INFO,                // VkStructureType           sType
		nullptr,                                           // const void              * pNext
		"Vulkan Application",                              // const char              * pApplicationName
		VK_MAKE_VERSION(1, 0, 0),                          // uint32_t                  applicationVersion
		"Numea Engine",                                    // const char              * pEngineName
		VK_MAKE_VERSION(1, 0, 0),                          // uint32_t                  engineVersion
		VK_MAKE_VERSION(1, 0, 0)                           // uint32_t                  apiVersion
	};

	VkInstanceCreateInfo instanceCreateInfo = {
		VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO,             // VkStructureType           sType
		nullptr,                                            // const void              * pNext
		0,                                                  // VkInstanceCreateFlags     flags
		&applicationInfo,                                   // const VkApplicationInfo * pApplicationInfo
		static_cast<uint32_t>(desiredLayers.size()),        // uint32_t                  enabledLayerCount
		desiredLayers.data(),                               // const char * const      * ppEnabledLayerNames
		static_cast<uint32_t>(desiredExtensions.size()),    // uint32_t                  enabledExtensionCount
		desiredExtensions.data()                            // const char * const      * ppEnabledExtensionNames
	};

	VkResult result = vkCreateInstance(&instanceCreateInfo, nullptr, &mInstance);
	if ((result != VK_SUCCESS) || (mInstance == VK_NULL_HANDLE))
	{
		// TODO : Use Numea System Log
		printf("Could not create Vulkan instance\n");
		return false;
	}

	if (!Loader::areInstanceLevelFunctionsLoaded())
	{
		if (!Loader::loadInstanceLevelFunctions(mInstance, desiredExtensions))
		{
			return false;
		}
	}

	// TODO : Only on Debug
	if (true)
	{
		// TODO : Change flags (and maybe use differents callbacks for differents flags)
		VkDebugReportCallbackCreateInfoEXT callback = {
			VK_STRUCTURE_TYPE_DEBUG_REPORT_CALLBACK_CREATE_INFO_EXT,    // sType
			NULL,                                                       // pNext
			10,                                                         // flags
			Instance::debugReportCallback,                              // pfnCallback
			NULL                                                        // pUserData
		};

		result = vkCreateDebugReportCallbackEXT(mInstance, &callback, nullptr, &mDebugReportCallback);
		if (result != VK_SUCCESS)
		{
			// TODO : Use Numea System Log
			printf("Could not create DebugReportCallback\n");
			return false;
		}
	}

	return true;
}

bool Instance::release()
{
	if (mDebugReportCallback != VK_NULL_HANDLE)
	{
		vkDestroyDebugReportCallbackEXT(mInstance, mDebugReportCallback, nullptr);
		mDebugReportCallback = VK_NULL_HANDLE;
	}

	if (mInstance != VK_NULL_HANDLE)
	{
		vkDestroyInstance(mInstance, nullptr);
		mInstance = VK_NULL_HANDLE;
		return true;
	}

	return false;
}

bool Instance::queryAvailablePhysicalDevices() const
{
	mPhysicalDevices.clear();

	uint32_t devicesCount;
	std::vector<VkPhysicalDevice> physicalDevices;
	VkResult result = VK_SUCCESS;

	result = vkEnumeratePhysicalDevices(mInstance, &devicesCount, nullptr);
	if (result != VK_SUCCESS || devicesCount == 0)
	{
		// TODO : Use Numea System Log
		printf("Could not get the number of available physical devices\n");
		return false;
	}

	physicalDevices.resize(devicesCount);
	result = vkEnumeratePhysicalDevices(mInstance, &devicesCount, physicalDevices.data());
	if (result != VK_SUCCESS || devicesCount == 0)
	{
		// TODO : Use Numea System Log
		printf("Could not enumerate physical devices\n");
		return false;
	}

	for (size_t i = 0; i < devicesCount; i++)
	{
		mPhysicalDevices.emplace_back(physicalDevices[i]);
	}

	return true;
}

VKAPI_ATTR VkBool32 VKAPI_CALL Instance::debugReportCallback(VkDebugReportFlagsEXT flags, VkDebugReportObjectTypeEXT objectType, uint64_t object, size_t location, int32_t messageCode, const char* pLayerPrefix, const char* pMessage, void* pUserData)
{
	// TODO : Improve callback
	// TODO : Many differents callbacks
	// TODO : Use Numea System Log
	printf("%s\n", pMessage);
	return VK_TRUE;
}

} // namespace Vulkan
} // namespace nu