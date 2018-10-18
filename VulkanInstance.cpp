#include "VulkanInstance.hpp"

namespace nu
{
namespace Vulkan
{

Instance::Ptr Instance::createInstance(const std::vector<const char*>& desiredExtensions, const std::vector<const char*> desiredLayers)
{
	return Instance::Ptr(new Instance(desiredExtensions, desiredLayers));
}

Instance::~Instance()
{
	ObjectTracker::unregisterObject(ObjectType_Instance);

	if (mDebugReportCallback != VK_NULL_HANDLE)
	{
		vkDestroyDebugReportCallbackEXT(mInstance, mDebugReportCallback, nullptr);
		mDebugReportCallback = VK_NULL_HANDLE;
	}

	if (mInstance != VK_NULL_HANDLE)
	{
		vkDestroyInstance(mInstance, nullptr);
		mInstance = VK_NULL_HANDLE;
	}
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

const std::vector<const char*>& Instance::getExtensions() const
{
	return mExtensions;
}

const std::vector<const char*>& Instance::getLayers() const
{
	return mLayers;
}

bool Instance::isInstanceExtensionSupported(const char* extensionName)
{
	if (sAvailableInstanceExtensions.size() == 0)
	{
		queryAvailableInstanceExtensions();
	}

	for (const VkExtensionProperties& extension : sAvailableInstanceExtensions)
	{
		if (strcmp(extension.extensionName, extensionName) == 0)
		{
			return true;
		}
	}
	return false;
}

bool Instance::isInstanceLayerSupported(const char* layerName)
{
	if (sAvailableInstanceLayers.size() == 0)
	{
		queryAvailableInstanceLayers();
	}

	for (const VkLayerProperties& layer : sAvailableInstanceLayers)
	{
		if (strcmp(layer.layerName, layerName) == 0)
		{
			return true;
		}
	}
	return false;
}

Instance::Instance(const std::vector<const char*>& desiredExtensions, const std::vector<const char*>& desiredLayers)
	: mInstance(VK_NULL_HANDLE)
	, mDebugReportCallback(VK_NULL_HANDLE)
	, mExtensions(desiredExtensions)
	, mLayers(desiredLayers)
	, mPhysicalDevices()
{
	ObjectTracker::registerObject(ObjectType_Instance);

	if (!Loader::ensureDynamicLibraryLoaded())
	{
		return;
	}
	if (!Loader::ensureExportedFunctionLoaded())
	{
		return;
	}
	if (!Loader::ensureGlobalLevelFunctionsLoaded())
	{
		return;
	}

	// TODO : Only if wanted
	if (true)
	{
		mExtensions.emplace_back(VK_KHR_SURFACE_EXTENSION_NAME);
		#ifdef VK_USE_PLATFORM_WIN32_KHR
			mExtensions.emplace_back(VK_KHR_WIN32_SURFACE_EXTENSION_NAME);
		#elif defined VK_USE_PLATFORM_XCB_KHR
			mExtensions.emplace_back(VK_KHR_XCB_SURFACE_EXTENSION_NAME);
		#elif defined VK_USE_PLATFORM_XLIB_KHR
			mExtensions.emplace_back(VK_KHR_XLIB_SURFACE_EXTENSION_NAME);
		#endif
	}
	// TODO : Only on Debug
	if (true)
	{
		mExtensions.emplace_back(VK_EXT_DEBUG_REPORT_EXTENSION_NAME);
	}

	// TODO : Only on Debug
	if (true)
	{
		for (auto& desiredExtension : mExtensions)
		{
			if (!isInstanceExtensionSupported(desiredExtension))
			{
				// TODO : Numea Log System
				printf("Could not find a desired instance extension : %s\n", desiredExtension);
				return;
			}
		}
	}

	// TODO : Only on Debug
	if (true)
	{
		mLayers.emplace_back("VK_LAYER_LUNARG_standard_validation");
	}

	// TODO : Only on Debug
	if (true)
	{
		for (auto& desiredLayer : mLayers)
		{
			if (!isInstanceLayerSupported(desiredLayer))
			{
				// TODO : Numea Log System
				printf("Could not find a desired instance layer : %s\n", desiredLayer);
				return;
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
		static_cast<uint32_t>(mLayers.size()),              // uint32_t                  enabledLayerCount
		mLayers.data(),                                     // const char * const      * ppEnabledLayerNames
		static_cast<uint32_t>(mExtensions.size()),          // uint32_t                  enabledExtensionCount
		mExtensions.data()                                  // const char * const      * ppEnabledExtensionNames
	};

	VkResult result = vkCreateInstance(&instanceCreateInfo, nullptr, &mInstance);
	if (result != VK_SUCCESS || mInstance == VK_NULL_HANDLE)
	{
		// TODO : Use Numea System Log
		printf("Could not create Vulkan instance\n");
		return;
	}

	if (!Loader::ensureInstanceLevelFunctionsLoaded(*this))
	{
		return;
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
		if (result != VK_SUCCESS || mDebugReportCallback == VK_NULL_HANDLE)
		{
			// TODO : Use Numea System Log
			printf("Could not create DebugReportCallback\n");
			return;
		}
	}
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

bool Instance::queryAvailableInstanceExtensions()
{
	sAvailableInstanceExtensions.clear();

	uint32_t extensionCount = 0;
	VkResult result = VK_SUCCESS;

	result = vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, nullptr);
	if (result != VK_SUCCESS || extensionCount == 0)
	{
		// TODO : Use Numea System Log
		printf("Could not get the number of instance extensions\n");
		return false;
	}

	sAvailableInstanceExtensions.resize(extensionCount);
	result = vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, sAvailableInstanceExtensions.data());
	if (result != VK_SUCCESS || extensionCount == 0)
	{
		// TODO : Use Numea System Log
		printf("Could not enumerate instance extensions\n");
		return false;
	}

	return true;
}

bool Instance::queryAvailableInstanceLayers()
{
	sAvailableInstanceLayers.clear();

	uint32_t layerCount = 0;
	VkResult result = VK_SUCCESS;

	result = vkEnumerateInstanceLayerProperties(&layerCount, nullptr);
	if (result != VK_SUCCESS || layerCount == 0)
	{
		// TODO : Use Numea System Log
		printf("Could not get the number of instance layers\n");
		return false;
	}

	sAvailableInstanceLayers.resize(layerCount);
	result = vkEnumerateInstanceLayerProperties(&layerCount, sAvailableInstanceLayers.data());
	if (result != VK_SUCCESS || layerCount == 0)
	{
		// TODO : Use Numea System Log
		printf("Could not enumerate instance layers\n");
		return false;
	}

	return true;
}

std::vector<VkExtensionProperties> Instance::sAvailableInstanceExtensions;
std::vector<VkLayerProperties> Instance::sAvailableInstanceLayers;

} // namespace Vulkan
} // namespace nu