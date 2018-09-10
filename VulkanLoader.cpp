#include "VulkanLoader.hpp"

namespace nu
{
	
bool VulkanLoader::load()
{
	// Defining inputs


	std::vector<const char*> desiredInstanceExtensions;
	desiredInstanceExtensions.emplace_back(VK_KHR_SURFACE_EXTENSION_NAME);
	#if defined(VK_USE_PLATFORM_WIN32_KHR)
		desiredInstanceExtensions.emplace_back(VK_KHR_WIN32_SURFACE_EXTENSION_NAME);
	#elif defined(VK_USE_PLATFORM_XCB_KHR)
		desiredInstanceExtensions.emplace_back(VK_KHR_XCB_SURFACE_EXTENSION_NAME);
	#elif defined(VK_USE_PLATFORM_XLIB_KHR)  
		desiredInstanceExtensions.emplace_back(VK_KHR_XLIB_SURFACE_EXTENSION_NAME);
	#endif

	std::vector<const char*> desiredDeviceExtensions;
	desiredDeviceExtensions.emplace_back(VK_KHR_SWAPCHAIN_EXTENSION_NAME);

	VkPhysicalDeviceFeatures desiredDeviceFeatures;
	desiredDeviceFeatures.geometryShader = VK_TRUE;

	std::string applicationName = "VulkanCookbook";


	// Start loading


	DynamicLibrary dyn;
	loadDynamicLibrary(dyn);
	loadFunctionExportedFromDynamicLibrary(dyn);
	loadGlobalLevelFunctions();

	printf("Available Instance Extensions : \n");
	std::vector<VkExtensionProperties> availableInstanceExtensions;
	queryAvailableInstanceExtensions(availableInstanceExtensions);
	for (auto& extension : availableInstanceExtensions)
	{
		printf("\t%s\n", extension.extensionName);
	}

	VkInstance instance;
	createInstance(desiredInstanceExtensions, applicationName, instance);
	loadInstanceLevelFunctions(instance, desiredInstanceExtensions);

	printf("Available Physical Devices : \n");
	std::vector<int> physicalDeviceScores;
	std::vector<VkPhysicalDevice> availablePhysicalDevices;
	queryAvailablePhysicalDevices(instance, availablePhysicalDevices);
	for (auto& physicalDevice : availablePhysicalDevices)
	{
		std::vector<VkExtensionProperties> availableDeviceExtensions;
		queryAvailableDeviceExtensions(physicalDevice, availableDeviceExtensions);

		VkPhysicalDeviceFeatures physicalDeviceFeatures;
		VkPhysicalDeviceProperties physicalDeviceProperties;
		queryFeaturesAndPropertiesOfPhysicalDevice(physicalDevice, physicalDeviceFeatures, physicalDeviceProperties);

		std::vector<VkQueueFamilyProperties> availableQueueFamilies;
		queryAvailableQueueFamiliesAndTheirProperties(physicalDevice, availableQueueFamilies);

		int physicalDeviceScore = 0;

		// TODO : Compute score
		if (desiredDeviceFeatures.geometryShader == VK_TRUE && physicalDeviceFeatures.geometryShader == VK_TRUE)
		{
			physicalDeviceScore += 1000;
		}
		else if (desiredDeviceFeatures.geometryShader == VK_TRUE && physicalDeviceFeatures.geometryShader == VK_FALSE)
		{
			physicalDeviceScore = -1;
		}

		physicalDeviceScores.push_back(physicalDeviceScore);

		printf("\t%s : \n", physicalDeviceProperties.deviceName);

		printf("\t\tAvailable Device Extensions : \n");
		for (auto& extension : availableDeviceExtensions)
		{
			printf("\t\t\t%s\n", extension.extensionName);
		}

		printf("\t\tAvailable Queue Families : \n");
		for (auto& queueFamily : availableQueueFamilies)
		{
			printf("\t\t\tType : %d, Count : %d\n", queueFamily.queueFlags, queueFamily.queueCount);
		}
	}

	// TODO : Create physical device with the best score












	/*


	// Create Surface
	VkSmartSurfaceKHR presentationSurface(instance);
	LoadAndCheck(createPresentationSurface(windowParameters));

	// Create Physical/Logical Devices
	std::vector<const char*> desiredDeviceExtensions;
	desiredDeviceExtensions.emplace_back(VK_KHR_SWAPCHAIN_EXTENSION_NAME);
	VkPhysicalDeviceFeatures* desiredDeviceFeatures = nullptr;
	// TODO : desiredDeviceExtensions
	// TODO : desiredDeviceFeatures
	// TODO : desiredDeviceProperties ???
	LoadAndCheck(createDevices(desiredDeviceExtensions, desiredDeviceFeatures));

	*/
	 
	return true;
}

bool VulkanLoader::isLoaded()
{
	return true; // TODO : Vulkan is Loaded ?
}

void VulkanLoader::unload()
{
}

bool VulkanLoader::loadDynamicLibrary(DynamicLibrary& dynamicLibrary)
{
	#if defined(WIN32)
		return dynamicLibrary.loadLibrary("vulkan-1.dll");
	#elif defined(__linux)
		return dynamicLibrary.loadLibrary("libvulkan.so.1");
	#endif
}

bool VulkanLoader::loadFunctionExportedFromDynamicLibrary(const DynamicLibrary& dynamicLibrary)
{
	// TODO : Use Numea System Log
	#define NU_EXPORTED_VULKAN_FUNCTION(name)							           \
		name = (PFN_##name)dynamicLibrary.loadFunction(#name);                     \
		if (name == nullptr)                                                       \
		{                                                                          \
			printf("Could not load exported Vulkan function named: %s\n", #name);  \
		    return false;                                                          \
		}

	#include "ListOfVulkanFunctions.inl"

	return true;
}

bool VulkanLoader::loadGlobalLevelFunctions()
{
	// TODO : Use Numea System Log
	#define NU_GLOBAL_LEVEL_VULKAN_FUNCTION(name)							           \
		name = (PFN_##name)vkGetInstanceProcAddr(nullptr, #name);                      \
		if (name == nullptr)                                                           \
		{                                                                              \
			printf("Could not load global level Vulkan function named: %s\n", #name);  \
		    return false;                                                              \
		}

	#include "ListOfVulkanFunctions.inl"

	return true;
}

bool VulkanLoader::queryAvailableInstanceExtensions(std::vector<VkExtensionProperties>& availableExtensions)
{
	unsigned int extensionsCount;
	VkResult result = VK_SUCCESS;

	result = vkEnumerateInstanceExtensionProperties(nullptr, &extensionsCount, nullptr);
	if ((result != VK_SUCCESS) || (extensionsCount == 0)) 
	{
		// TODO : Use Numea System Log
		printf("Could not get the number of instance extensions\n");
		return false;
	}

	availableExtensions.resize(extensionsCount);
	result = vkEnumerateInstanceExtensionProperties(nullptr, &extensionsCount, availableExtensions.data());
	if ((result != VK_SUCCESS) || (extensionsCount == 0)) 
	{
		// TODO : Use Numea System Log
		printf("Could not enumerate instance extensions\n");
		return false;
	}

	return true;
}

bool VulkanLoader::createInstance(const std::vector<const char*>& desiredExtensions, const std::string& applicationName, VkInstance& instance)
{
	// TODO : Cache this ?
	std::vector<VkExtensionProperties> availableInstanceExtensions;
	queryAvailableInstanceExtensions(availableInstanceExtensions);

	for (auto& extension : desiredExtensions) 
	{
		if (!isExtensionSupported(availableInstanceExtensions, extension)) 
		{
			// TODO : Use Numea System Log
			printf("Extension named '%s' is not supported by an instance object\n", extension);
			return false;
		}
	}

	// TODO : Get application version
	// TODO : Get engine name and version from Numea
	VkApplicationInfo applicationInfo = {
		VK_STRUCTURE_TYPE_APPLICATION_INFO,                // VkStructureType           sType
		nullptr,                                           // const void              * pNext
		applicationName.c_str(),                           // const char              * pApplicationName
		VK_MAKE_VERSION(1, 0, 0),                          // uint32_t                  applicationVersion
		"Numea Engine",                                    // const char              * pEngineName
		VK_MAKE_VERSION(1, 0, 0),                          // uint32_t                  engineVersion
		VK_MAKE_VERSION(1, 0, 0)                           // uint32_t                  apiVersion
	};
	
	// TODO : Validation Layer (see here : https://vulkan-tutorial.com/Drawing_a_triangle/Setup/Validation_layers)
	VkInstanceCreateInfo instanceCreateInfo = {
		VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO,             // VkStructureType           sType
		nullptr,                                            // const void              * pNext
		0,                                                  // VkInstanceCreateFlags     flags
		&applicationInfo,                                   // const VkApplicationInfo * pApplicationInfo
		0,                                                  // uint32_t                  enabledLayerCount
		nullptr,                                            // const char * const      * ppEnabledLayerNames
		static_cast<uint32_t>(desiredExtensions.size()),    // uint32_t                  enabledExtensionCount
		desiredExtensions.data()                            // const char * const      * ppEnabledExtensionNames
	};

	VkResult result = vkCreateInstance(&instanceCreateInfo, nullptr, &instance);
	if ((result != VK_SUCCESS) || (instance == VK_NULL_HANDLE)) 
	{
		// TODO : Use Numea System Log
		printf("Could not create Vulkan instance\n");
		return false;
	}

	return true;
}

bool VulkanLoader::loadInstanceLevelFunctions(VkInstance instance, const std::vector<const char*>& enabledExtensions)
{
	// TODO : Use Numea System Log
	// Load core Vulkan API instance-level functions
	#define NU_INSTANCE_LEVEL_VULKAN_FUNCTION(name)                                    \
		name = (PFN_##name)vkGetInstanceProcAddr(instance, #name);                     \
		if(name == nullptr)														       \
		{																			   \
			printf("Could not load instance-level Vulkan function named: %s", #name);  \
			return false;                                                              \
		}

	// TODO : Use Numea System Log
	// Load instance-level functions from enabled extensions
	#define NU_INSTANCE_LEVEL_VULKAN_FUNCTION_FROM_EXTENSION(name, extension)                  \
		for (auto& enabledExtension : enabledExtensions)                                       \
		{                                                                                      \
			if (std::string(enabledExtension) == std::string(extension))                       \
			{                                                                                  \
				name = (PFN_##name)vkGetInstanceProcAddr(instance, #name);                     \
				if (name == nullptr)                                                           \
				{                                                                              \
					printf("Could not load instance-level Vulkan function named: %s", #name);  \
					return false;                                                              \
				}                                                                              \
			}                                                                                  \
		}

	#include "ListOfVulkanFunctions.inl"

	return true;
}

bool VulkanLoader::queryAvailablePhysicalDevices(VkInstance instance, std::vector<VkPhysicalDevice>& availablePhysicalDevices)
{
	unsigned int devicesCount;
	VkResult result = VK_SUCCESS;

	result = vkEnumeratePhysicalDevices(instance, &devicesCount, nullptr);
	if ((result != VK_SUCCESS) || (devicesCount == 0))
	{
		// TODO : Use Numea System Log
		printf("Could not get the number of available physical devices\n");
		return false;
	}

	availablePhysicalDevices.resize(devicesCount);
	result = vkEnumeratePhysicalDevices(instance, &devicesCount, availablePhysicalDevices.data());
	if ((result != VK_SUCCESS) || (devicesCount == 0))
	{
		// TODO : Use Numea System Log
		printf("Could not enumerate physical devices\n");
		return false;
	}

	return true;
}

bool VulkanLoader::queryAvailableDeviceExtensions(VkPhysicalDevice physicalDevice, std::vector<VkExtensionProperties>& availableExtensions)
{
	uint32_t extensionsCount = 0;
	VkResult result = VK_SUCCESS;

	result = vkEnumerateDeviceExtensionProperties(physicalDevice, nullptr, &extensionsCount, nullptr);
	if ((result != VK_SUCCESS) || (extensionsCount == 0))
	{
		// TODO : Use Numea System Log
		printf("Could not get the number of device extensions\n");
		return false;
	}

	availableExtensions.resize(extensionsCount);
	result = vkEnumerateDeviceExtensionProperties(physicalDevice, nullptr, &extensionsCount, availableExtensions.data());
	if ((result != VK_SUCCESS) || (extensionsCount == 0)) 
	{
		// TODO : Use Numea System Log
		printf("Could not enumerate device extensions\n");
		return false;
	}

	return true;
}

void VulkanLoader::queryFeaturesAndPropertiesOfPhysicalDevice(VkPhysicalDevice physicalDevice, VkPhysicalDeviceFeatures& deviceFeatures, VkPhysicalDeviceProperties& deviceProperties)
{
	vkGetPhysicalDeviceFeatures(physicalDevice, &deviceFeatures);
	vkGetPhysicalDeviceProperties(physicalDevice, &deviceProperties);
}

bool VulkanLoader::queryAvailableQueueFamiliesAndTheirProperties(VkPhysicalDevice physicalDevice, std::vector<VkQueueFamilyProperties>& queueFamilies)
{
	uint32_t queueFamiliesCount = 0;

	vkGetPhysicalDeviceQueueFamilyProperties(physicalDevice, &queueFamiliesCount, nullptr);
	if (queueFamiliesCount == 0)
	{
		// TODO : Use Numea System Log
		printf("Could not get the number of queue families\n");
		return false;
	}

	queueFamilies.resize(queueFamiliesCount);
	vkGetPhysicalDeviceQueueFamilyProperties(physicalDevice, &queueFamiliesCount, queueFamilies.data());
	if (queueFamiliesCount == 0)
	{
		// TODO : Use Numea System Log
		printf("Could not acquire properties of queue families\n");
		return false;
	}

	return true;
}

bool VulkanLoader::selectQueueFamilyWithDesiredCapabilities(VkPhysicalDevice physicalDevice, const std::vector<VkQueueFamilyProperties>& queueFamilies, VkQueueFlags desiredCapabilities, uint32_t& queueFamilyIndex)
{
	uint32_t size = static_cast<uint32_t>(queueFamilies.size());
	for (uint32_t index = 0; index < size; index++)
	{
		if (queueFamilies[index].queueCount > 0 && (queueFamilies[index].queueFlags & desiredCapabilities) == desiredCapabilities)
		{
			queueFamilyIndex = index;
			return true;
		}
	}

	return false;
}

bool VulkanLoader::selectQueueFamilyThatSupportsPresentationToSurface(VkPhysicalDevice physicalDevice, const std::vector<VkQueueFamilyProperties>& queueFamilies, VkSurfaceKHR presentationSurface, uint32_t& queueFamilyIndex)
{
	uint32_t size = static_cast<uint32_t>(queueFamilies.size());
	for (uint32_t index = 0; index < size; index++)
	{
		VkBool32 presentationSupported = VK_FALSE;
		VkResult result = vkGetPhysicalDeviceSurfaceSupportKHR(physicalDevice, index, presentationSurface, &presentationSupported);
		if ((result == VK_SUCCESS) && (presentationSupported == VK_TRUE)) 
		{
			queueFamilyIndex = index;
			return true;
		}
	}

	return false;
}

bool VulkanLoader::createLogicalDevice(VkPhysicalDevice physicalDevice, std::vector<QueueInfo> queueInfos, const std::vector<const char*>& desiredExtensions, VkPhysicalDeviceFeatures* desiredFeatures, VkDevice& logicalDevice)
{
	std::vector<VkExtensionProperties> availableExtensions;
	if (!queryAvailableDeviceExtensions(physicalDevice, availableExtensions)) 
	{
		return false;
	}

	for (auto& extension : desiredExtensions) 
	{
		if (!isExtensionSupported(availableExtensions, extension)) 
		{
			// TODO : Use Numea System Log
			printf("Extension named '%s' is not supported by a physical device\n", extension);
			return false;
		}
	}

	std::vector<VkDeviceQueueCreateInfo> queueCreateInfos;

	for (auto& info : queueInfos) 
	{
		queueCreateInfos.push_back({
			VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO,       // VkStructureType                  sType
			nullptr,                                          // const void                     * pNext
			0,                                                // VkDeviceQueueCreateFlags         flags
			info.familyIndex,                                 // uint32_t                         queueFamilyIndex
			static_cast<uint32_t>(info.priorities.size()),    // uint32_t                         queueCount
			info.priorities.data()                            // const float                    * pQueuePriorities
		});
	};

	VkDeviceCreateInfo deviceCreateInfo = {
		VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO,               // VkStructureType                  sType
		nullptr,                                            // const void                     * pNext
		0,                                                  // VkDeviceCreateFlags              flags
		static_cast<uint32_t>(queueCreateInfos.size()),     // uint32_t                         queueCreateInfoCount
		queueCreateInfos.data(),                            // const VkDeviceQueueCreateInfo  * pQueueCreateInfos
		0,                                                  // uint32_t                         enabledLayerCount
		nullptr,                                            // const char * const             * ppEnabledLayerNames
		static_cast<uint32_t>(desiredExtensions.size()),    // uint32_t                         enabledExtensionCount
		desiredExtensions.data(),                           // const char * const             * ppEnabledExtensionNames
		desiredFeatures                                     // const VkPhysicalDeviceFeatures * pEnabledFeatures
	};

	VkResult result = vkCreateDevice(physicalDevice, &deviceCreateInfo, nullptr, &logicalDevice);
	if ((result != VK_SUCCESS) || (logicalDevice == VK_NULL_HANDLE)) 
	{
		// TODO : Use Numea System Log
		printf("Could not create logical device\n");
		return false;
	}

	return true;
}

bool VulkanLoader::loadDeviceLevelFunctions(VkDevice logicalDevice, const std::vector<const char*>& enabledExtensions)
{
	// TODO : Use Numea System Log
	// Load core Vulkan API deivce-level functions
	#define NU_DEVICE_LEVEL_VULKAN_FUNCTION(name)                                      \
		name = (PFN_##name)vkGetDeviceProcAddr(logicalDevice, #name);                  \
		if(name == nullptr)														       \
		{																			   \
			printf("Could not load device-level Vulkan function named: %s", #name);    \
			return false;                                                              \
		}

	// TODO : Use Numea System Log
	// Load device-level functions from enabled extensions
	#define NU_DEVICE_LEVEL_VULKAN_FUNCTION_FROM_EXTENSION(name, extension)                    \
		for (auto& enabledExtension : enabledExtensions)                                       \
		{                                                                                      \
			if (std::string(enabledExtension) == std::string(extension))                       \
			{                                                                                  \
				name = (PFN_##name)vkGetDeviceProcAddr(logicalDevice, #name);                  \
				if (name == nullptr)                                                           \
				{                                                                              \
					printf("Could not load device-level Vulkan function named: %s", #name);    \
					return false;                                                              \
				}                                                                              \
			}                                                                                  \
		}

	#include "ListOfVulkanFunctions.inl"

	return true;
}

void VulkanLoader::getDeviceQueue(VkDevice logicalDevice, uint32_t queueFamilyIndex, uint32_t queueIndex, VkQueue& queue)
{
	vkGetDeviceQueue(logicalDevice, queueFamilyIndex, queueIndex, &queue);
}

bool VulkanLoader::createLogicalDeviceWithGeometryShadersAndGraphicsAndComputeQueues(VkInstance instance, VkDevice& logicalDevice, VkQueue& graphicsQueue, VkQueue& computeQueue)
{
	return false;
}

void VulkanLoader::destroyLogicalDevice(VkDevice& logicalDevice)
{
	if (logicalDevice != VK_NULL_HANDLE)
	{
		vkDestroyDevice(logicalDevice, nullptr);
		logicalDevice = VK_NULL_HANDLE;
	}
}

void VulkanLoader::destroyInstance(VkInstance& instance)
{
	if (instance != VK_NULL_HANDLE)
	{
		vkDestroyInstance(instance, nullptr);
		instance = VK_NULL_HANDLE;
	}
}

void VulkanLoader::releaseDynamicLibrary(DynamicLibrary& dynamicLibrary)
{
	dynamicLibrary.unloadLibrary();
}

bool VulkanLoader::createInstanceWithWsiExtensionsEnabled(std::vector<const char*>& desiredExtensions, const std::string& applicationName, VkInstance& instance)
{
	desiredExtensions.emplace_back(VK_KHR_SURFACE_EXTENSION_NAME);

	#if defined(VK_USE_PLATFORM_WIN32_KHR)
		desiredExtensions.emplace_back(VK_KHR_WIN32_SURFACE_EXTENSION_NAME);
	#elif defined(VK_USE_PLATFORM_XCB_KHR)
		desiredExtensions.emplace_back(VK_KHR_XCB_SURFACE_EXTENSION_NAME);
	#elif defined(VK_USE_PLATFORM_XLIB_KHR)
		desiredExtensions.emplace_back(VK_KHR_XLIB_SURFACE_EXTENSION_NAME);
	#endif

	return createInstance(desiredExtensions, applicationName, instance);
}

bool VulkanLoader::createPresentationSurface(VkInstance instance, const WindowParameters& windowParameters, VkSurfaceKHR& presentationSurface)
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

		result = vkCreateWin32SurfaceKHR(instance, &surfaceCreateInfo, nullptr, &presentationSurface);

	#elif defined(VK_USE_PLATFORM_XLIB_KHR)

		VkXlibSurfaceCreateInfoKHR surfaceCreateInfo = {
			VK_STRUCTURE_TYPE_XLIB_SURFACE_CREATE_INFO_KHR,   // VkStructureType                 sType
			nullptr,                                          // const void                    * pNext
			0,                                                // VkXlibSurfaceCreateFlagsKHR     flags
			windowParameters.dpy,                             // Display                       * dpy
			windowParameters.window                           // Window                          window
		};

		result = vkCreateXlibSurfaceKHR(instance, &surfaceCreateInfo, nullptr, &presentationSurface);

	#elif defined(VK_USE_PLATFORM_XCB_KHR)

		VkXcbSurfaceCreateInfoKHR surfaceCreateInfo = {
			VK_STRUCTURE_TYPE_XCB_SURFACE_CREATE_INFO_KHR,    // VkStructureType                 sType
			nullptr,                                          // const void                    * pNext
			0,                                                // VkXcbSurfaceCreateFlagsKHR      flags
			windowParameters.connection,                      // xcb_connection_t              * connection
			windowParameters.window                           // xcb_window_t                    window
		};

		result = vkCreateXcbSurfaceKHR(instance, &surfaceCreateInfo, nullptr, &presentationSurface);

	#endif

	if (result == VK_INCOMPLETE)
	{
		// TODO : Use Numea System Log
		printf("Could not find platform for presentation surface\n");
		return false;
	}

	if ((result != VK_SUCCESS) || (presentationSurface == VK_NULL_HANDLE))
	{
		// TODO : Use Numea System Log
		printf("Could not create presentation surface\n");
		return false;
	}

	return true;
}

bool VulkanLoader::selectQueueFamilyThatSupportsPresentationToGivenSurface(VkPhysicalDevice physicalDevice, VkSurfaceKHR presentationSurface, uint32_t& queueFamilyIndex)
{
	std::vector<VkQueueFamilyProperties> queueFamilies;
	if (!queryAvailableQueueFamiliesAndTheirProperties(physicalDevice, queueFamilies)) 
	{
		return false;
	}

	for (uint32_t index = 0; index < static_cast<uint32_t>(queueFamilies.size()); index++) 
	{
		VkBool32 presentationSupported = VK_FALSE;
		VkResult result = vkGetPhysicalDeviceSurfaceSupportKHR(physicalDevice, index, presentationSurface, &presentationSupported);
		if ((result == VK_SUCCESS) && (presentationSupported == VK_TRUE)) 
		{
			queueFamilyIndex = index;
			return true;
		}
	}
	return false;
}

bool VulkanLoader::createLogicalDeviceWithWsiExtensionsEnabled(VkPhysicalDevice physicalDevice, std::vector<QueueInfo> queueInfos, std::vector<const char*>& desiredExtensions, VkPhysicalDeviceFeatures* desiredFeatures, VkDevice& logicalDevice)
{
	desiredExtensions.emplace_back(VK_KHR_SWAPCHAIN_EXTENSION_NAME);

	return createLogicalDevice(physicalDevice, queueInfos, desiredExtensions, desiredFeatures, logicalDevice);
}

bool VulkanLoader::selectDesiredPresentationMode(VkPhysicalDevice physicalDevice, VkSurfaceKHR presentationSurface, VkPresentModeKHR desiredPresentMode, VkPresentModeKHR& presentMode)
{
	uint32_t presentModesCount = 0;
	VkResult result;

	result = vkGetPhysicalDeviceSurfacePresentModesKHR(physicalDevice, presentationSurface, &presentModesCount, nullptr);
	if ((result != VK_SUCCESS) || (presentModesCount == 0)) 
	{
		// TODO : Use Numea System Log
		printf("Could not get the number of supported present modes\n");
		return false;
	}

	std::vector<VkPresentModeKHR> presentModes(presentModesCount);
	result = vkGetPhysicalDeviceSurfacePresentModesKHR(physicalDevice, presentationSurface, &presentModesCount, presentModes.data());
	if ((result != VK_SUCCESS) || (presentModesCount == 0))
	{
		// TODO : Use Numea System Log
		printf("Could not enumerate present modes\n");
		return false;
	}

	// Select present mode
	for (auto& currentPresentMode : presentModes) 
	{
		if (currentPresentMode == desiredPresentMode) 
		{
			presentMode = desiredPresentMode;
			return true;
		}
	}

	// TODO : Use Numea System Log
	printf("Desired present mode is not supported. Selecting default FIFO mode\n");
	for (auto & currentPresentMode : presentModes) 
	{
		if (currentPresentMode == VK_PRESENT_MODE_FIFO_KHR) 
		{
			presentMode = VK_PRESENT_MODE_FIFO_KHR;
			return true;
		}
	}

	// TODO : Use Numea System Log
	printf("VK_PRESENT_MODE_FIFO_KHR is not supported though it's mandatory for all drivers!\n");
	return false;
}

bool VulkanLoader::queryCapabilitiesOfPresentationSurface(VkPhysicalDevice physicalDevice, VkSurfaceKHR presentationSurface, VkSurfaceCapabilitiesKHR& surfaceCapabilities)
{
	VkResult result = vkGetPhysicalDeviceSurfaceCapabilitiesKHR(physicalDevice, presentationSurface, &surfaceCapabilities);
	if (result != VK_SUCCESS) 
	{
		// TODO : Use Numea System Log
		printf("Could not get the capabilities of a presentation surface\n");
		return false;
	}
	return true;
}

bool VulkanLoader::selectNumberOfSwapchainImages(const VkSurfaceCapabilitiesKHR& surfaceCapabilities, uint32_t& numberOfImages)
{
	numberOfImages = surfaceCapabilities.minImageCount + 1;
	if ((surfaceCapabilities.maxImageCount > 0) && (numberOfImages > surfaceCapabilities.maxImageCount)) 
	{
		numberOfImages = surfaceCapabilities.maxImageCount;
	}
	return true;
}

bool VulkanLoader::chooseSizeOfSwapchainImages(const VkSurfaceCapabilitiesKHR& surfaceCapabilities, VkExtent2D& sizeOfImages)
{
	if (surfaceCapabilities.currentExtent.width == 0xFFFFFFFF)
	{
		sizeOfImages = { 640, 480 };

		if (sizeOfImages.width < surfaceCapabilities.minImageExtent.width)
		{
			sizeOfImages.width = surfaceCapabilities.minImageExtent.width;
		}
		else if (sizeOfImages.width > surfaceCapabilities.maxImageExtent.width) 
		{
			sizeOfImages.width = surfaceCapabilities.maxImageExtent.width;
		}

		if (sizeOfImages.height < surfaceCapabilities.minImageExtent.height) 
		{
			sizeOfImages.height = surfaceCapabilities.minImageExtent.height;
		}
		else if (sizeOfImages.height > surfaceCapabilities.maxImageExtent.height) 
		{
			sizeOfImages.height = surfaceCapabilities.maxImageExtent.height;
		}
	}
	else 
	{
		sizeOfImages = surfaceCapabilities.currentExtent;
	}
	return true;
}

bool VulkanLoader::selectDesiredUsageScenariosOfSwapchainImages(const VkSurfaceCapabilitiesKHR& surfaceCapabilities, VkImageUsageFlags desiredUsages, VkImageUsageFlags& imageUsage)
{
	imageUsage = desiredUsages & surfaceCapabilities.supportedUsageFlags;
	return desiredUsages == imageUsage;
}

bool VulkanLoader::selectTransformationOfSwapchainImages(const VkSurfaceCapabilitiesKHR& surfaceCapabilities, VkSurfaceTransformFlagBitsKHR desiredTransform, VkSurfaceTransformFlagBitsKHR& surfaceTransform)
{
	if (surfaceCapabilities.supportedTransforms & desiredTransform) 
	{
		surfaceTransform = desiredTransform;
	}
	else 
	{
		surfaceTransform = surfaceCapabilities.currentTransform;
	}
	return true;
}

bool VulkanLoader::selectFormatOfSwapchainImages(VkPhysicalDevice physicalDevice, VkSurfaceKHR presentationSurface, VkSurfaceFormatKHR desiredSurfaceFormat, VkFormat& imageFormat, VkColorSpaceKHR& imageColorSpace)
{
	uint32_t formatsCount = 0;
	VkResult result;

	result = vkGetPhysicalDeviceSurfaceFormatsKHR(physicalDevice, presentationSurface, &formatsCount, nullptr);
	if ((result != VK_SUCCESS) || (formatsCount == 0)) 
	{
		// TODO : Use Numea System Log
		printf("Could not get the number of supported surface formats\n");
		return false;
	}

	std::vector<VkSurfaceFormatKHR> surfaceFormats(formatsCount);
	result = vkGetPhysicalDeviceSurfaceFormatsKHR(physicalDevice, presentationSurface, &formatsCount, surfaceFormats.data());
	if ((result != VK_SUCCESS) || (formatsCount == 0)) 
	{
		// TODO : Use Numea System Log
		printf("Could not enumerate supported surface formats\n");
		return false;
	}

	if ((surfaceFormats.size() == 1) && (surfaceFormats[0].format == VK_FORMAT_UNDEFINED))
	{
		imageFormat = desiredSurfaceFormat.format;
		imageColorSpace = desiredSurfaceFormat.colorSpace;
		return true;
	}

	for (auto& surfaceFormat : surfaceFormats) 
	{
		if ((desiredSurfaceFormat.format == surfaceFormat.format) && (desiredSurfaceFormat.colorSpace == surfaceFormat.colorSpace)) 
		{
			imageFormat = desiredSurfaceFormat.format;
			imageColorSpace = desiredSurfaceFormat.colorSpace;
			return true;
		}
	}

	for (auto& surfaceFormat : surfaceFormats) 
	{
		if ((desiredSurfaceFormat.format == surfaceFormat.format)) 
		{
			imageFormat = desiredSurfaceFormat.format;
			imageColorSpace = surfaceFormat.colorSpace;

			// TODO : Use Numea System Log
			printf("Desired combination of format and colorspace is not supported. Selecting other colorspace\n");
			return true;
		}
	}

	// TODO : assert(surfaceFormats.size() > 0);

	imageFormat = surfaceFormats[0].format;
	imageColorSpace = surfaceFormats[0].colorSpace;

	// TODO : Use Numea System Log
	printf("Desired format is not supported. Selecting available format - colorspace combination\n");
	return true;
}

bool VulkanLoader::createSwapchain(VkDevice logicalDevice, VkSurfaceKHR presentationSurface, uint32_t imageCount, VkSurfaceFormatKHR surfaceFormat, VkExtent2D imageSize, VkImageUsageFlags imageUsage, VkSurfaceTransformFlagBitsKHR surfaceTransform, VkPresentModeKHR presentMode, VkSwapchainKHR & oldSwapchain, VkSwapchainKHR & swapchain)
{
	VkSwapchainCreateInfoKHR swapchainCreateInfo = {
		VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR,  // VkStructureType                  sType
		nullptr,                                      // const void                     * pNext
		0,                                            // VkSwapchainCreateFlagsKHR        flags
		presentationSurface,                          // VkSurfaceKHR                     surface
		imageCount,                                   // uint32_t                         minImageCount
		surfaceFormat.format,                         // VkFormat                         imageFormat
		surfaceFormat.colorSpace,                     // VkColorSpaceKHR                  imageColorSpace
		imageSize,                                    // VkExtent2D                       imageExtent
		1,                                            // uint32_t                         imageArrayLayers
		imageUsage,                                   // VkImageUsageFlags                imageUsage
		VK_SHARING_MODE_EXCLUSIVE,                    // VkSharingMode                    imageSharingMode
		0,                                            // uint32_t                         queueFamilyIndexCount
		nullptr,                                      // const uint32_t                 * pQueueFamilyIndices
		surfaceTransform,                             // VkSurfaceTransformFlagBitsKHR    preTransform
		VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR,            // VkCompositeAlphaFlagBitsKHR      compositeAlpha
		presentMode,                                  // VkPresentModeKHR                 presentMode
		VK_TRUE,                                      // VkBool32                         clipped
		oldSwapchain                                  // VkSwapchainKHR                   oldSwapchain
	};

	VkResult result = vkCreateSwapchainKHR(logicalDevice, &swapchainCreateInfo, nullptr, &swapchain);
	if ((result != VK_SUCCESS) || (swapchain == VK_NULL_HANDLE)) 
	{
		// TODO : Use Numea System Log
		printf("Could not create a swapchain\n");
		return false;
	}

	if (oldSwapchain != VK_NULL_HANDLE) 
	{
		vkDestroySwapchainKHR(logicalDevice, oldSwapchain, nullptr);
		oldSwapchain = VK_NULL_HANDLE;
	}

	return true;
}

bool VulkanLoader::queryHandlesOfSwapchainImages(VkDevice logicalDevice, VkSwapchainKHR swapchain, std::vector<VkImage>& swapchainImages)
{
	uint32_t imagesCount = 0;
	VkResult result;

	result = vkGetSwapchainImagesKHR(logicalDevice, swapchain, &imagesCount, nullptr);
	if ((result != VK_SUCCESS) || (imagesCount == 0)) 
	{
		// TODO : Use Numea System Log
		printf("Could not get the number of swapchain images\n");
		return false;
	}

	swapchainImages.resize(imagesCount);
	result = vkGetSwapchainImagesKHR(logicalDevice, swapchain, &imagesCount, swapchainImages.data());
	if ((result != VK_SUCCESS) || (imagesCount == 0))
	{
		// TODO : Use Numea System Log
		printf("Could not enumerate swapchain images\n");
		return false;
	}

	return true;
}

bool VulkanLoader::createSwapchainWithR8G8B8A8FormatAndMailboxPresentMode(VkPhysicalDevice physicalDevice, VkSurfaceKHR presentationSurface, VkDevice logicalDevice, VkImageUsageFlags swapchainImageUsage, VkExtent2D& imageSize, VkFormat& imageFormat, VkSwapchainKHR& oldSwapchain, VkSwapchainKHR& swapchain, std::vector<VkImage>& swapchainImages)
{
	VkPresentModeKHR desiredPresentMode;
	if (!selectDesiredPresentationMode(physicalDevice, presentationSurface, VK_PRESENT_MODE_MAILBOX_KHR, desiredPresentMode)) 
	{
		return false;
	}

	VkSurfaceCapabilitiesKHR surfaceCapabilities;
	if (!queryCapabilitiesOfPresentationSurface(physicalDevice, presentationSurface, surfaceCapabilities)) 
	{
		return false;
	}

	uint32_t numberOfImages;
	if (!selectNumberOfSwapchainImages(surfaceCapabilities, numberOfImages)) 
	{
		return false;
	}

	if (!chooseSizeOfSwapchainImages(surfaceCapabilities, imageSize)) 
	{
		return false;
	}
	if ((imageSize.width == 0) || (imageSize.height == 0)) 
	{
		return true;
	}

	VkImageUsageFlags imageUsage;
	if (!selectDesiredUsageScenariosOfSwapchainImages(surfaceCapabilities, swapchainImageUsage, imageUsage)) 
	{
		return false;
	}

	VkSurfaceTransformFlagBitsKHR surfaceTransform;
	selectTransformationOfSwapchainImages(surfaceCapabilities, VK_SURFACE_TRANSFORM_IDENTITY_BIT_KHR, surfaceTransform);

	VkColorSpaceKHR imageColorSpace;
	if (!selectFormatOfSwapchainImages(physicalDevice, presentationSurface, { VK_FORMAT_R8G8B8A8_UNORM, VK_COLOR_SPACE_SRGB_NONLINEAR_KHR }, imageFormat, imageColorSpace)) 
	{
		return false;
	}

	if (!createSwapchain(logicalDevice, presentationSurface, numberOfImages, { imageFormat, imageColorSpace }, imageSize, imageUsage, surfaceTransform, desiredPresentMode, oldSwapchain, swapchain)) 
	{
		return false;
	}

	if (!queryHandlesOfSwapchainImages(logicalDevice, swapchain, swapchainImages)) 
	{
		return false;
	}

	return true;
}

bool VulkanLoader::acquireSwapchainImage(VkDevice logicalDevice, VkSwapchainKHR swapchain, VkSemaphore semaphore, VkFence fence, uint32_t& imageIndex)
{
	VkResult result;
	result = vkAcquireNextImageKHR(logicalDevice, swapchain, 2000000000, semaphore, fence, &imageIndex);
	switch (result) 
	{
		case VK_SUCCESS:
		case VK_SUBOPTIMAL_KHR:
			return true;
		default:
			return false;
	}
}

bool VulkanLoader::presentImage(VkQueue queue, std::vector<VkSemaphore> renderingSemaphores, std::vector<PresentInfo> imagesToPresent)
{
	VkResult result;
	std::vector<VkSwapchainKHR> swapchains;
	std::vector<uint32_t> imageIndices;

	for (auto& imageToPresent : imagesToPresent)
	{
		swapchains.emplace_back(imageToPresent.swapchain);
		imageIndices.emplace_back(imageToPresent.imageIndex);
	}

	VkPresentInfoKHR presentInfo = {
		VK_STRUCTURE_TYPE_PRESENT_INFO_KHR,                   // VkStructureType          sType
		nullptr,                                              // const void*              pNext
		static_cast<uint32_t>(renderingSemaphores.size()),    // uint32_t                 waitSemaphoreCount
		renderingSemaphores.data(),                           // const VkSemaphore      * pWaitSemaphores
		static_cast<uint32_t>(swapchains.size()),             // uint32_t                 swapchainCount
		swapchains.data(),                                    // const VkSwapchainKHR   * pSwapchains
		imageIndices.data(),                                  // const uint32_t         * pImageIndices
		nullptr                                               // VkResult*                pResults
	};

	result = vkQueuePresentKHR(queue, &presentInfo);
	switch (result) {
		case VK_SUCCESS:
			return true;
		default:
			return false;
	}
}

void VulkanLoader::destroySwapchain(VkDevice logicalDevice, VkSwapchainKHR& swapchain)
{
	if (swapchain != VK_NULL_HANDLE)
	{
		vkDestroySwapchainKHR(logicalDevice, swapchain, nullptr);
		swapchain = VK_NULL_HANDLE;
	}
}

void VulkanLoader::destroyPresentationSurface(VkInstance instance, VkSurfaceKHR& presentationSurface)
{
	if (presentationSurface != VK_NULL_HANDLE)
	{
		vkDestroySurfaceKHR(instance, presentationSurface, nullptr);
		presentationSurface = VK_NULL_HANDLE;
	}
}

bool VulkanLoader::createCommandPool(VkDevice logicalDevice, VkCommandPoolCreateFlags parameters, uint32_t queueFamily, VkCommandPool& commandPool)
{
	VkCommandPoolCreateInfo commandPoolCreateInfo = {
		VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO,   // VkStructureType              sType
		nullptr,                                      // const void                 * pNext
		parameters,                                   // VkCommandPoolCreateFlags     flags
		queueFamily                                   // uint32_t                     queueFamilyIndex
	};

	VkResult result = vkCreateCommandPool(logicalDevice, &commandPoolCreateInfo, nullptr, &commandPool);
	if (VK_SUCCESS != result) 
	{
		// TODO : Use Numea System Log
		printf("Could not create command pool\n");
		return false;
	}
	return true;
}

bool VulkanLoader::allocateCommandBuffers(VkDevice logicalDevice, VkCommandPool commandPool, VkCommandBufferLevel level, uint32_t count, std::vector<VkCommandBuffer>& commandBuffers)
{
	VkCommandBufferAllocateInfo commandBufferAllocateInfo = {
		VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO,   // VkStructureType          sType
		nullptr,                                          // const void             * pNext
		commandPool,                                      // VkCommandPool            commandPool
		level,                                            // VkCommandBufferLevel     level
		count                                             // uint32_t                 commandBufferCount
	};

	commandBuffers.resize(count);

	VkResult result = vkAllocateCommandBuffers(logicalDevice, &commandBufferAllocateInfo, commandBuffers.data());
	if (VK_SUCCESS != result) 
	{
		// TODO : Use Numea System Log
		printf("Could not allocate command buffers\n");
		return false;
	}
	return true;
}

bool VulkanLoader::beginCommandBufferRecordingOperation(VkCommandBuffer commandBuffer, VkCommandBufferUsageFlags usage, VkCommandBufferInheritanceInfo* secondaryCommandBufferInfo)
{
	VkCommandBufferBeginInfo commandBufferBeginInfo = {
		VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO,    // VkStructureType                        sType
		nullptr,                                        // const void                           * pNext
		usage,                                          // VkCommandBufferUsageFlags              flags
		secondaryCommandBufferInfo                      // const VkCommandBufferInheritanceInfo * pInheritanceInfo
	};

	VkResult result = vkBeginCommandBuffer(commandBuffer, &commandBufferBeginInfo);
	if (VK_SUCCESS != result) 
	{
		// TODO : Use Numea System Log
		printf("Could not begin command buffer recording operation\n");
		return false;
	}
	return true;
}

bool VulkanLoader::endCommandBufferRecordingOperation(VkCommandBuffer commandBuffer)
{
	VkResult result = vkEndCommandBuffer(commandBuffer);
	if (VK_SUCCESS != result) 
	{
		// TODO : Use Numea System Log
		printf("Error occurred during command buffer recording\n");
		return false;
	}
	return true;
}

bool VulkanLoader::resetCommandBuffer(VkCommandBuffer commandBuffer, bool releaseResources)
{
	VkResult result = vkResetCommandBuffer(commandBuffer, releaseResources ? VK_COMMAND_BUFFER_RESET_RELEASE_RESOURCES_BIT : 0);
	if (VK_SUCCESS != result) 
	{
		// TODO : Use Numea System Log
		printf("Error occurred during command buffer reset\n");
		return false;
	}
	return true;
}

bool VulkanLoader::resetCommandPool(VkDevice logicalDevice, VkCommandPool commandPool, bool releaseResources)
{
	VkResult result = vkResetCommandPool(logicalDevice, commandPool, releaseResources ? VK_COMMAND_POOL_RESET_RELEASE_RESOURCES_BIT : 0);
	if (VK_SUCCESS != result) 
	{
		// TODO : Use Numea System Log
		printf("Error occurred during command pool reset\n");
		return false;
	}
	return true;
}

bool VulkanLoader::createSemaphore(VkDevice logicalDevice, VkSemaphore& semaphore)
{
	VkSemaphoreCreateInfo semaphoreCreateInfo = {
		VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO,    // VkStructureType            sType
		nullptr,                                    // const void               * pNext
		0                                           // VkSemaphoreCreateFlags     flags
	};

	VkResult result = vkCreateSemaphore(logicalDevice, &semaphoreCreateInfo, nullptr, &semaphore);
	if (result != VK_SUCCESS) 
	{
		// TODO : Use Numea System Log
		printf("Could not create a semaphore\n");
		return false;
	}
	return true;
}

bool VulkanLoader::createFence(VkDevice logicalDevice, bool signaled, VkFence& fence)
{
	VkFenceCreateInfo fenceCreateInfo = {
		VK_STRUCTURE_TYPE_FENCE_CREATE_INFO,          // VkStructureType        sType
		nullptr,                                      // const void           * pNext
		signaled ? VK_FENCE_CREATE_SIGNALED_BIT : 0u, // VkFenceCreateFlags     flags
	};

	VkResult result = vkCreateFence(logicalDevice, &fenceCreateInfo, nullptr, &fence);
	if (result != VK_SUCCESS)
	{
		// TODO : Use Numea System Log
		printf("Could not create a fence\n");
		return false;
	}
	return true;
}

bool VulkanLoader::waitForFences(VkDevice logicalDevice, const std::vector<VkFence>& fences, VkBool32 waitForAll, uint64_t timeout)
{
	if (fences.size() > 0) 
	{
		VkResult result = vkWaitForFences(logicalDevice, static_cast<uint32_t>(fences.size()), fences.data(), waitForAll, timeout);
		if (result != VK_SUCCESS)
		{
			// TODO : Use Numea System Log
			printf("Waiting on fence failed\n");
			return false;
		}
		return true;
	}
	return false;
}

bool VulkanLoader::resetFences(VkDevice logicalDevice, const std::vector<VkFence>& fences)
{
	if (fences.size() > 0) 
	{
		VkResult result = vkResetFences(logicalDevice, static_cast<uint32_t>(fences.size()), fences.data());
		if (result != VK_SUCCESS)
		{
			// TODO : Use Numea System Log
			printf("Error occurred when tried to reset fences\n");
			return false;
		}
		return true;
	}
	return false;
}

bool VulkanLoader::submitCommandBuffersToQueue(VkQueue queue, std::vector<WaitSemaphoreInfo> waitSemaphoreInfos, std::vector<VkCommandBuffer> commandBuffers, std::vector<VkSemaphore> signalSemaphores, VkFence fence)
{
	std::vector<VkSemaphore> waitSemaphoreHandles;
	std::vector<VkPipelineStageFlags> waitSemaphoreStages;

	for (auto& waitSemaphoreInfo : waitSemaphoreInfos) 
	{
		waitSemaphoreHandles.emplace_back(waitSemaphoreInfo.semaphore);
		waitSemaphoreStages.emplace_back(waitSemaphoreInfo.waitingStage);
	}

	VkSubmitInfo submitInfo = {
		VK_STRUCTURE_TYPE_SUBMIT_INFO,                        // VkStructureType                sType
		nullptr,                                              // const void                   * pNext
		static_cast<uint32_t>(waitSemaphoreInfos.size()),     // uint32_t                       waitSemaphoreCount
		waitSemaphoreHandles.data(),                          // const VkSemaphore            * pWaitSemaphores
		waitSemaphoreStages.data(),                           // const VkPipelineStageFlags   * pWaitDstStageMask
		static_cast<uint32_t>(commandBuffers.size()),         // uint32_t                       commandBufferCount
		commandBuffers.data(),                                // const VkCommandBuffer        * pCommandBuffers
		static_cast<uint32_t>(signalSemaphores.size()),       // uint32_t                       signalSemaphoreCount
		signalSemaphores.data()                               // const VkSemaphore            * pSignalSemaphores
	};

	VkResult result = vkQueueSubmit(queue, 1, &submitInfo, fence);
	if (result != VK_SUCCESS) 
	{
		// TODO : Use Numea System Log
		printf("Error occurred during command buffer submission\n");
		return false;
	}
	return true;
}

bool VulkanLoader::synchronizeTwoCommandBuffers(VkQueue firstQueue, std::vector<WaitSemaphoreInfo> firstWaitSemaphoreInfos, std::vector<VkCommandBuffer> firstCommandBuffers, std::vector<WaitSemaphoreInfo> synchronizingSemaphores, VkQueue secondQueue, std::vector<VkCommandBuffer> secondCommandBuffers, std::vector<VkSemaphore> secondSignalSemaphores, VkFence secondFence)
{
	std::vector<VkSemaphore> firstSignalSemaphores;
	for (auto& semaphoreInfo : synchronizingSemaphores) 
	{
		firstSignalSemaphores.emplace_back(semaphoreInfo.semaphore);
	}

	if (!submitCommandBuffersToQueue(firstQueue, firstWaitSemaphoreInfos, firstCommandBuffers, firstSignalSemaphores, VK_NULL_HANDLE)) 
	{
		return false;
	}

	if (!submitCommandBuffersToQueue(secondQueue, synchronizingSemaphores, secondCommandBuffers, secondSignalSemaphores, secondFence)) 
	{
		return false;
	}

	return true;
}

bool VulkanLoader::checkIfProcessingOfSubmittedCommandBufferHasFinished(VkDevice logicalDevice, VkQueue queue, std::vector<WaitSemaphoreInfo> waitSemaphoreInfos, std::vector<VkCommandBuffer> commandBuffers, std::vector<VkSemaphore> signalSemaphores, VkFence fence, uint64_t timeout, VkResult & waitStatus)
{
	if (!submitCommandBuffersToQueue(queue, waitSemaphoreInfos, commandBuffers, signalSemaphores, fence)) 
	{
		return false;
	}

	return waitForFences(logicalDevice, { fence }, VK_FALSE, timeout);
}

bool VulkanLoader::waitUntilAllCommandsSubmittedToQueueAreFinished(VkQueue queue)
{
	VkResult result = vkQueueWaitIdle(queue);
	if (result != VK_SUCCESS)
	{
		// TODO : Use Numea System Log
		printf("Waiting for all operations submitted to queue failed\n");
		return false;
	}
	return true;
}

bool VulkanLoader::waitForAllSubmittedCommandsToBeFinished(VkDevice logicalDevice)
{
	VkResult result = vkDeviceWaitIdle(logicalDevice);
	if (result != VK_SUCCESS)
	{
		// TODO : Use Numea System Log
		printf("Waiting on a device failed\n");
		return false;
	}
	return true;
}

void VulkanLoader::destroyFence(VkDevice logicalDevice, VkFence& fence)
{
	if (fence != VK_NULL_HANDLE) 
	{
		vkDestroyFence(logicalDevice, fence, nullptr);
		fence = VK_NULL_HANDLE;
	}
}

void VulkanLoader::destroySemaphore(VkDevice logicalDevice, VkSemaphore& semaphore)
{
	if (semaphore != VK_NULL_HANDLE)
	{
		vkDestroySemaphore(logicalDevice, semaphore, nullptr);
		semaphore = VK_NULL_HANDLE;
	}
}

void VulkanLoader::freeCommandBuffers(VkDevice logicalDevice, VkCommandPool commandPool, std::vector<VkCommandBuffer>& commandBuffers)
{
	if (commandBuffers.size() > 0) 
	{
		vkFreeCommandBuffers(logicalDevice, commandPool, static_cast<uint32_t>(commandBuffers.size()), commandBuffers.data());
		commandBuffers.clear();
	}
}

void VulkanLoader::destroyCommandPool(VkDevice logicalDevice, VkCommandPool& commandPool)
{
	if (commandPool != VK_NULL_HANDLE) 
	{
		vkDestroyCommandPool(logicalDevice, commandPool, nullptr);
		commandPool = VK_NULL_HANDLE;
	}
}

bool VulkanLoader::createBuffer(VkDevice logicalDevice, VkDeviceSize size, VkBufferUsageFlags usage, VkBuffer& buffer)
{
	VkBufferCreateInfo bufferCreateInfo = {
		VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO,   // VkStructureType        sType
		nullptr,                                // const void           * pNext
		0,                                      // VkBufferCreateFlags    flags
		size,                                   // VkDeviceSize           size
		usage,                                  // VkBufferUsageFlags     usage
		VK_SHARING_MODE_EXCLUSIVE,              // VkSharingMode          sharingMode
		0,                                      // uint32_t               queueFamilyIndexCount
		nullptr                                 // const uint32_t       * pQueueFamilyIndices
	};

	VkResult result = vkCreateBuffer(logicalDevice, &bufferCreateInfo, nullptr, &buffer);
	if (result != VK_SUCCESS)
	{
		// TODO : Use Numea System Log
		printf("Could not create a buffer\n");
		return false;
	}
	return true;
}

bool VulkanLoader::allocateAndBindMemoryObjectToBuffer(VkPhysicalDevice physicalDevice, VkDevice logicalDevice, VkBuffer buffer, VkMemoryPropertyFlagBits memoryProperties, VkDeviceMemory& memoryObject)
{
	// TODO : Cache this ?
	VkPhysicalDeviceMemoryProperties physicalDeviceMemoryProperties;
	vkGetPhysicalDeviceMemoryProperties(physicalDevice, &physicalDeviceMemoryProperties);

	VkMemoryRequirements memoryRequirements;
	vkGetBufferMemoryRequirements(logicalDevice, buffer, &memoryRequirements);

	memoryObject = VK_NULL_HANDLE;
	for (uint32_t type = 0; type < physicalDeviceMemoryProperties.memoryTypeCount; type++) 
	{
		if ((memoryRequirements.memoryTypeBits & (1 << type)) && ((physicalDeviceMemoryProperties.memoryTypes[type].propertyFlags & memoryProperties) == memoryProperties)) 
		{
			VkMemoryAllocateInfo bufferMemoryAllocateInfo = {
				VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO,   // VkStructureType    sType
				nullptr,                                  // const void       * pNext
				memoryRequirements.size,                  // VkDeviceSize       allocationSize
				type                                      // uint32_t           memoryTypeIndex
			};

			VkResult result = vkAllocateMemory(logicalDevice, &bufferMemoryAllocateInfo, nullptr, &memoryObject);
			if (result == VK_SUCCESS) 
			{
				break;
			}
		}
	}

	if (memoryObject == VK_NULL_HANDLE) 
	{
		// TODO : Use Numea System Log
		printf("Could not allocate memory for a buffer\n");
		return false;
	}

	VkResult result = vkBindBufferMemory(logicalDevice, buffer, memoryObject, 0); //TODO : Use offset
	if (result != VK_SUCCESS)
	{
		// TODO : Use Numea System Log
		printf("Could not bind memory object to a buffer\n");
		return false;
	}
	return true;
}

void VulkanLoader::setBufferMemoryBarrier(VkCommandBuffer commandBuffer, VkPipelineStageFlags generatingStages, VkPipelineStageFlags consumingStages, std::vector<BufferTransition> bufferTransitions)
{
	std::vector<VkBufferMemoryBarrier> bufferMemoryBarriers;

	for (auto& bufferTransition : bufferTransitions) 
	{
		bufferMemoryBarriers.push_back({
			VK_STRUCTURE_TYPE_BUFFER_MEMORY_BARRIER,    // VkStructureType    sType
			nullptr,                                    // const void       * pNext
			bufferTransition.currentAccess,             // VkAccessFlags      srcAccessMask
			bufferTransition.newAccess,                 // VkAccessFlags      dstAccessMask
			bufferTransition.currentQueueFamily,        // uint32_t           srcQueueFamilyIndex
			bufferTransition.newQueueFamily,            // uint32_t           dstQueueFamilyIndex
			bufferTransition.buffer,                    // VkBuffer           buffer
			0,                                          // VkDeviceSize       offset
			VK_WHOLE_SIZE                               // VkDeviceSize       size
			});
		// TODO : Use offset and size !
	}

	if (bufferMemoryBarriers.size() > 0) 
	{
		vkCmdPipelineBarrier(commandBuffer, generatingStages, consumingStages, 0, 0, nullptr, static_cast<uint32_t>(bufferMemoryBarriers.size()), bufferMemoryBarriers.data(), 0, nullptr);
	}
}

bool VulkanLoader::createBufferView(VkDevice logicalDevice, VkBuffer buffer, VkFormat format, VkDeviceSize memoryOffset, VkDeviceSize memoryRange, VkBufferView& bufferView)
{
	VkBufferViewCreateInfo bufferViewCreateInfo = {
		VK_STRUCTURE_TYPE_BUFFER_VIEW_CREATE_INFO,    // VkStructureType            sType
		nullptr,                                      // const void               * pNext
		0,                                            // VkBufferViewCreateFlags    flags
		buffer,                                       // VkBuffer                   buffer
		format,                                       // VkFormat                   format
		memoryOffset,                                 // VkDeviceSize               offset
		memoryRange                                   // VkDeviceSize               range
	};

	VkResult result = vkCreateBufferView(logicalDevice, &bufferViewCreateInfo, nullptr, &bufferView);
	if (result != VK_SUCCESS)
	{
		// TODO : Use Numea System Log
		printf("Could not creat buffer view\n");
		return false;
	}
	return true;
}

bool VulkanLoader::createImage(VkDevice logicalDevice, VkImageType type, VkFormat format, VkExtent3D size, uint32_t numMipmaps, uint32_t numLayers, VkSampleCountFlagBits samples, VkImageUsageFlags usageScenarios, bool cubemap, VkImage& image)
{
	VkImageCreateInfo imageCreateInfo = {
		VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO,                // VkStructureType          sType
		nullptr,                                            // const void             * pNext
		cubemap ? VK_IMAGE_CREATE_CUBE_COMPATIBLE_BIT : 0u, // VkImageCreateFlags       flags
		type,                                               // VkImageType              imageType
		format,                                             // VkFormat                 format
		size,                                               // VkExtent3D               extent
		numMipmaps,                                         // uint32_t                 mipLevels
		cubemap ? 6 * numLayers : numLayers,                // uint32_t                 arrayLayers
		samples,                                            // VkSampleCountFlagBits    samples
		VK_IMAGE_TILING_OPTIMAL,                            // VkImageTiling            tiling
		usageScenarios,                                     // VkImageUsageFlags        usage
		VK_SHARING_MODE_EXCLUSIVE,                          // VkSharingMode            sharingMode
		0,                                                  // uint32_t                 queueFamilyIndexCount
		nullptr,                                            // const uint32_t         * pQueueFamilyIndices
		VK_IMAGE_LAYOUT_UNDEFINED                           // VkImageLayout            initialLayout
	};

	VkResult result = vkCreateImage(logicalDevice, &imageCreateInfo, nullptr, &image);
	if (result != VK_SUCCESS)
	{
		// TODO : Use Numea System Log
		printf("Could not create an image\n");
		return false;
	}
	return true;
}

bool VulkanLoader::allocateAndBindMemoryObjectToImage(VkPhysicalDevice physicalDevice, VkDevice logicalDevice, VkImage image, VkMemoryPropertyFlagBits memoryProperties, VkDeviceMemory& memoryObject)
{
	// TODO : Cache this ?
	VkPhysicalDeviceMemoryProperties physicalDeviceMemoryProperties;
	vkGetPhysicalDeviceMemoryProperties(physicalDevice, &physicalDeviceMemoryProperties);

	VkMemoryRequirements memoryRequirements;
	vkGetImageMemoryRequirements(logicalDevice, image, &memoryRequirements);

	memoryObject = VK_NULL_HANDLE;
	for (uint32_t type = 0; type < physicalDeviceMemoryProperties.memoryTypeCount; type++) 
	{
		if ((memoryRequirements.memoryTypeBits & (1 << type)) && ((physicalDeviceMemoryProperties.memoryTypes[type].propertyFlags & memoryProperties) == memoryProperties)) 
		{
			VkMemoryAllocateInfo imageMemoryAllocateInfo = {
				VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO,   // VkStructureType    sType
				nullptr,                                  // const void       * pNext
				memoryRequirements.size,                  // VkDeviceSize       allocationSize
				type                                      // uint32_t           memoryTypeIndex
			};

			VkResult result = vkAllocateMemory(logicalDevice, &imageMemoryAllocateInfo, nullptr, &memoryObject);
			if (result == VK_SUCCESS) 
			{
				break;
			}
		}
	}

	if (memoryObject == VK_NULL_HANDLE) 
	{
		// TODO : Use Numea System Log
		printf("Could not allocate memory for an image\n");
		return false;
	}

	VkResult result = vkBindImageMemory(logicalDevice, image, memoryObject, 0); // TODO : Use offset
	if (result != VK_SUCCESS)
	{
		// TODO : Use Numea System Log
		printf("Could not bind memory object to an image\n");
		return false;
	}
	return true;
}

void VulkanLoader::setImageMemoryBarrier(VkCommandBuffer commandBuffer, VkPipelineStageFlags generatingStages, VkPipelineStageFlags consumingStages, std::vector<ImageTransition> imageTransitions)
{
	std::vector<VkImageMemoryBarrier> imageMemoryBarriers;

	for (auto& imageTransition : imageTransitions) 
	{
		imageMemoryBarriers.push_back({
			VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER,   // VkStructureType            sType
			nullptr,                                  // const void               * pNext
			imageTransition.currentAccess,            // VkAccessFlags              srcAccessMask
			imageTransition.newAccess,                // VkAccessFlags              dstAccessMask
			imageTransition.currentLayout,            // VkImageLayout              oldLayout
			imageTransition.newLayout,                // VkImageLayout              newLayout
			imageTransition.currentQueueFamily,       // uint32_t                   srcQueueFamilyIndex
			imageTransition.newQueueFamily,           // uint32_t                   dstQueueFamilyIndex
			imageTransition.image,                    // VkImage                    image
			{                                         // VkImageSubresourceRange    subresourceRange
				imageTransition.aspect,                   // VkImageAspectFlags         aspectMask
				0,                                        // uint32_t                   baseMipLevel
				VK_REMAINING_MIP_LEVELS,                  // uint32_t                   levelCount
				0,                                        // uint32_t                   baseArrayLayer
				VK_REMAINING_ARRAY_LAYERS                 // uint32_t                   layerCount
			}
			});
	}

	if (imageMemoryBarriers.size() > 0) 
	{
		vkCmdPipelineBarrier(commandBuffer, generatingStages, consumingStages, 0, 0, nullptr, 0, nullptr, static_cast<uint32_t>(imageMemoryBarriers.size()), imageMemoryBarriers.data());
	}
}

bool VulkanLoader::createImageView(VkDevice logicalDevice, VkImage image, VkImageViewType viewType, VkFormat format, VkImageAspectFlags aspect, VkImageView& imageView)
{
	VkImageViewCreateInfo imageViewCreateInfo = {
		VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO,   // VkStructureType            sType
		nullptr,                                    // const void               * pNext
		0,                                          // VkImageViewCreateFlags     flags
		image,                                      // VkImage                    image
		viewType,                                  // VkImageViewType            viewType
		format,                                     // VkFormat                   format
		{                                           // VkComponentMapping         components
			VK_COMPONENT_SWIZZLE_IDENTITY,              // VkComponentSwizzle         r
			VK_COMPONENT_SWIZZLE_IDENTITY,              // VkComponentSwizzle         g
			VK_COMPONENT_SWIZZLE_IDENTITY,              // VkComponentSwizzle         b
			VK_COMPONENT_SWIZZLE_IDENTITY               // VkComponentSwizzle         a
		},
		{                                           // VkImageSubresourceRange    subresourceRange
			aspect,                                     // VkImageAspectFlags         aspectMask
			0,                                          // uint32_t                   baseMipLevel
			VK_REMAINING_MIP_LEVELS,                    // uint32_t                   levelCount
			0,                                          // uint32_t                   baseArrayLayer
			VK_REMAINING_ARRAY_LAYERS                   // uint32_t                   layerCount
		}
	};

	VkResult result = vkCreateImageView(logicalDevice, &imageViewCreateInfo, nullptr, &imageView);
	if (result != VK_SUCCESS) 
	{
		// TODO : Use Numea System Log
		printf("Could not create an image view\n");
		return false;
	}
	return true;
}

bool VulkanLoader::create2DImageAndView(VkPhysicalDevice physicalDevice, VkDevice logicalDevice, VkFormat format, VkExtent2D size, uint32_t numMipmaps, uint32_t numLayers, VkSampleCountFlagBits samples, VkImageUsageFlags usage, VkImageAspectFlags aspect, VkImage& image, VkDeviceMemory& memoryObject, VkImageView& imageView)
{
	if (!createImage(logicalDevice, VK_IMAGE_TYPE_2D, format, { size.width, size.height, 1 }, numMipmaps, numLayers, samples, usage, false, image)) 
	{
		return false;
	}

	if (!allocateAndBindMemoryObjectToImage(physicalDevice, logicalDevice, image, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, memoryObject)) 
	{
		return false;
	}

	if (!createImageView(logicalDevice, image, VK_IMAGE_VIEW_TYPE_2D, format, aspect, imageView)) 
	{
		return false;
	}

	return true;
}

bool VulkanLoader::createLayered2DImageWithCubemapView(VkPhysicalDevice physicalDevice, VkDevice logicalDevice, uint32_t size, uint32_t numMipmaps, VkImageUsageFlags usage, VkImageAspectFlags aspect, VkImage& image, VkDeviceMemory& memoryObject, VkImageView& imageView)
{
	if (!createImage(logicalDevice, VK_IMAGE_TYPE_2D, VK_FORMAT_R8G8B8A8_UNORM, { size, size, 1 }, numMipmaps, 6, VK_SAMPLE_COUNT_1_BIT, usage, true, image)) 
	{
		return false;
	}

	if (!allocateAndBindMemoryObjectToImage(physicalDevice, logicalDevice, image, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, memoryObject)) 
	{
		return false;
	}

	if (!createImageView(logicalDevice, image, VK_IMAGE_VIEW_TYPE_CUBE, VK_FORMAT_R8G8B8A8_UNORM, aspect, imageView)) 
	{
		return false;
	}

	return true;
}

bool VulkanLoader::mapUpdateAndUnmapHostVisibleMemory(VkDevice logicalDevice, VkDeviceMemory memoryObject, VkDeviceSize offset, VkDeviceSize dataSize, void* data, bool unmap, void** pointer)
{
	VkResult result;
	void* localPointer;

	result = vkMapMemory(logicalDevice, memoryObject, offset, dataSize, 0, &localPointer);
	if (result != VK_SUCCESS)
	{
		// TODO : Use Numea System Log
		printf("Could not map memory object\n");
		return false;
	}

	std::memcpy(localPointer, data, static_cast<size_t>(dataSize));

	std::vector<VkMappedMemoryRange> memoryRanges = {
		{
			VK_STRUCTURE_TYPE_MAPPED_MEMORY_RANGE,  // VkStructureType    sType
			nullptr,                                // const void       * pNext
			memoryObject,                           // VkDeviceMemory     memory
			offset,                                 // VkDeviceSize       offset
			VK_WHOLE_SIZE                           // VkDeviceSize       size
		}
	};

	vkFlushMappedMemoryRanges(logicalDevice, static_cast<uint32_t>(memoryRanges.size()), memoryRanges.data());
	if (result != VK_SUCCESS)
	{
		// TODO : Use Numea System Log
		printf("Could not flush mapped memory\n");
		return false;
	}

	if (unmap) 
	{
		vkUnmapMemory(logicalDevice, memoryObject);
	}
	else if (pointer != nullptr) 
	{
		*pointer = localPointer;
	}

	return true;
}

void VulkanLoader::copyDataBetweenBuffers(VkCommandBuffer commandBuffer, VkBuffer sourceBuffer, VkBuffer destinationBuffer, std::vector<VkBufferCopy> regions)
{
	if (regions.size() > 0) 
	{
		vkCmdCopyBuffer(commandBuffer, sourceBuffer, destinationBuffer, static_cast<uint32_t>(regions.size()), regions.data());
	}
}

void VulkanLoader::copyDataFromBufferToImage(VkCommandBuffer commandBuffer, VkBuffer sourceBuffer, VkImage destinationImage, VkImageLayout imageLayout, std::vector<VkBufferImageCopy> regions)
{
	if (regions.size() > 0) 
	{
		vkCmdCopyBufferToImage(commandBuffer, sourceBuffer, destinationImage, imageLayout, static_cast<uint32_t>(regions.size()), regions.data());
	}
}

void VulkanLoader::copyDataFromImageToBuffer(VkCommandBuffer commandBuffer, VkImage sourceImage, VkImageLayout imageLayout, VkBuffer destinationBuffer, std::vector<VkBufferImageCopy> regions)
{
	if (regions.size() > 0) 
	{
		vkCmdCopyImageToBuffer(commandBuffer, sourceImage, imageLayout, destinationBuffer, static_cast<uint32_t>(regions.size()), regions.data());
	}
}

bool VulkanLoader::useStagingBufferToUpdateBufferWithDeviceLocalMemoryBound(VkPhysicalDevice physicalDevice, VkDevice logicalDevice, VkDeviceSize dataSize, void* data, VkBuffer destinationBuffer, VkDeviceSize destinationOffset, VkAccessFlags destinationBufferCurrentAccess, VkAccessFlags destinationBufferNewAccess, VkPipelineStageFlags destinationBufferGeneratingStages, VkPipelineStageFlags destinationBufferConsumingStages, VkQueue queue, VkCommandBuffer commandBuffer, std::vector<VkSemaphore> signalSemaphores)
{
	VkSmartBuffer stagingBuffer(logicalDevice);
	if (!createBuffer(logicalDevice, dataSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, stagingBuffer.getHandle())) 
	{
		return false;
	}

	VkSmartDeviceMemory memoryObject(logicalDevice);
	if (!allocateAndBindMemoryObjectToBuffer(physicalDevice, logicalDevice, stagingBuffer, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT, memoryObject.getHandle())) 
	{
		return false;
	}

	if (!mapUpdateAndUnmapHostVisibleMemory(logicalDevice, memoryObject, 0, dataSize, data, true, nullptr)) 
	{
		return false;
	}

	if (!beginCommandBufferRecordingOperation(commandBuffer, VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT, nullptr)) 
	{
		return false;
	}

	setBufferMemoryBarrier(commandBuffer, destinationBufferGeneratingStages, VK_PIPELINE_STAGE_TRANSFER_BIT, { { destinationBuffer, destinationBufferCurrentAccess, VK_ACCESS_TRANSFER_WRITE_BIT, VK_QUEUE_FAMILY_IGNORED, VK_QUEUE_FAMILY_IGNORED } });

	copyDataBetweenBuffers(commandBuffer, stagingBuffer, destinationBuffer, { { 0, destinationOffset, dataSize } });

	setBufferMemoryBarrier(commandBuffer, VK_PIPELINE_STAGE_TRANSFER_BIT, destinationBufferConsumingStages, { { destinationBuffer, VK_ACCESS_TRANSFER_WRITE_BIT, destinationBufferNewAccess, VK_QUEUE_FAMILY_IGNORED, VK_QUEUE_FAMILY_IGNORED } });

	if (!endCommandBufferRecordingOperation(commandBuffer)) 
	{
		return false;
	}

	VkSmartFence fence(logicalDevice);
	if (!createFence(logicalDevice, false, fence.getHandle()))
	{
		return false;
	}

	if (!submitCommandBuffersToQueue(queue, {}, { commandBuffer }, signalSemaphores, fence)) 
	{
		return false;
	}

	if (!waitForFences(logicalDevice, { fence }, VK_FALSE, 500000000)) {
		return false;
	}

	return true;
}

bool VulkanLoader::useStagingBufferToUpdateImageWithDeviceLocalMemoryBound(VkPhysicalDevice physicalDevice, VkDevice logicalDevice, VkDeviceSize dataSize, void* data, VkImage destinationImage, VkImageSubresourceLayers destinationImageSubresource, VkOffset3D destinationImageOffset, VkExtent3D destinationImageSize, VkImageLayout destinationImageCurrentLayout, VkImageLayout destinationImageNewLayout, VkAccessFlags destinationImageCurrentAccess, VkAccessFlags destinationImageNewAccess, VkImageAspectFlags destinationImageAspect, VkPipelineStageFlags destinationImageGeneratingStages, VkPipelineStageFlags destinationImageConsumingStages, VkQueue queue, VkCommandBuffer commandBuffer, std::vector<VkSemaphore> signalSemaphores)
{
	// TODO : VkDestroyer
	//VkDestroyer(VkBuffer) stagingBuffer;
	//InitVkDestroyer(logical_device, stagingBuffer);
	
	VkBuffer stagingBuffer;
	if (!createBuffer(logicalDevice, dataSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, stagingBuffer)) 
	{
		return false;
	}

	// TODO : VkDestroyer
	//VkDestroyer(VkDeviceMemory) memoryObject;
	//InitVkDestroyer(logical_device, memory_object);

	VkDeviceMemory memoryObject;
	if (!allocateAndBindMemoryObjectToBuffer(physicalDevice, logicalDevice, stagingBuffer, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT, memoryObject)) 
	{
		return false;
	}

	if (!mapUpdateAndUnmapHostVisibleMemory(logicalDevice, memoryObject, 0, dataSize, data, true, nullptr)) 
	{
		return false;
	}

	if (!beginCommandBufferRecordingOperation(commandBuffer, VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT, nullptr)) {
		return false;
	}

	setImageMemoryBarrier(commandBuffer, destinationImageGeneratingStages, VK_PIPELINE_STAGE_TRANSFER_BIT,
	{
		{
			destinationImage,                         // VkImage            Image
			destinationImageCurrentAccess,            // VkAccessFlags      CurrentAccess
			VK_ACCESS_TRANSFER_WRITE_BIT,             // VkAccessFlags      NewAccess
			destinationImageCurrentLayout,            // VkImageLayout      CurrentLayout
			VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,     // VkImageLayout      NewLayout
			VK_QUEUE_FAMILY_IGNORED,                  // uint32_t           CurrentQueueFamily
			VK_QUEUE_FAMILY_IGNORED,                  // uint32_t           NewQueueFamily
			destinationImageAspect                    // VkImageAspectFlags Aspect
		} 
	});

	copyDataFromBufferToImage(commandBuffer, stagingBuffer, destinationImage, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
	{
		{
			0,                                        // VkDeviceSize               bufferOffset
			0,                                        // uint32_t                   bufferRowLength
			0,                                        // uint32_t                   bufferImageHeight
			destinationImageSubresource,              // VkImageSubresourceLayers   imageSubresource
			destinationImageOffset,                   // VkOffset3D                 imageOffset
			destinationImageSize,                     // VkExtent3D                 imageExtent
		} 
	});

	setImageMemoryBarrier(commandBuffer, VK_PIPELINE_STAGE_TRANSFER_BIT, destinationImageConsumingStages,
	{
		{
			destinationImage,                         // VkImage            Image
			VK_ACCESS_TRANSFER_WRITE_BIT,             // VkAccessFlags      CurrentAccess
			destinationImageNewAccess,                // VkAccessFlags      NewAccess
			VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,     // VkImageLayout      CurrentLayout
			destinationImageNewLayout,                // VkImageLayout      NewLayout
			VK_QUEUE_FAMILY_IGNORED,                  // uint32_t           CurrentQueueFamily
			VK_QUEUE_FAMILY_IGNORED,                  // uint32_t           NewQueueFamily
			destinationImageAspect                    // VkImageAspectFlags Aspect
		} 
	});

	if (!endCommandBufferRecordingOperation(commandBuffer)) 
	{
		return false;
	}

	// TODO : VkDestroyer
	//VkDestroyer(VkFence) fence;
	//InitVkDestroyer(logical_device, fence);
	
	VkFence fence;
	if (!createFence(logicalDevice, false, fence)) 
	{
		return false;
	}

	if (!submitCommandBuffersToQueue(queue, {}, { commandBuffer }, signalSemaphores, fence)) 
	{
		return false;
	}

	if (!waitForFences(logicalDevice, { fence }, VK_FALSE, 500000000)) 
	{
		return false;
	}

	// TODO : VkDestroyer
	destroyBuffer(logicalDevice, stagingBuffer);
	freeMemoryObject(logicalDevice, memoryObject);
	destroyFence(logicalDevice, fence);

	return true;
}

void VulkanLoader::destroyImageView(VkDevice logicalDevice, VkImageView& imageView)
{
	if (imageView != VK_NULL_HANDLE)
	{
		vkDestroyImageView(logicalDevice, imageView, nullptr);
		imageView = VK_NULL_HANDLE;
	}
}

void VulkanLoader::destroyImage(VkDevice logicalDevice, VkImage& image)
{
	if (image != VK_NULL_HANDLE)
	{
		vkDestroyImage(logicalDevice, image, nullptr);
		image = VK_NULL_HANDLE;
	}
}

void VulkanLoader::destroyBufferView(VkDevice logicalDevice, VkBufferView& bufferView)
{
	if (bufferView != VK_NULL_HANDLE)
	{
		vkDestroyBufferView(logicalDevice, bufferView, nullptr);
		bufferView = VK_NULL_HANDLE;
	}
}

void VulkanLoader::freeMemoryObject(VkDevice logicalDevice, VkDeviceMemory& memoryObject)
{
	if (memoryObject != VK_NULL_HANDLE)
	{
		vkFreeMemory(logicalDevice, memoryObject, nullptr);
		memoryObject = VK_NULL_HANDLE;
	}
}

void VulkanLoader::destroyBuffer(VkDevice logicalDevice, VkBuffer& buffer)
{
	if (buffer != VK_NULL_HANDLE)
	{
		vkDestroyBuffer(logicalDevice, buffer, nullptr);
		buffer = VK_NULL_HANDLE;
	}
}

bool VulkanLoader::createSampler(VkDevice logicalDevice, VkFilter magFilter, VkFilter minFilter, VkSamplerMipmapMode mipmapMode, VkSamplerAddressMode uAddressMode, VkSamplerAddressMode vAddressMode, VkSamplerAddressMode wAddressMode, float lodBias, bool anisotropyEnable, float maxAnisotropy, bool compareEnable, VkCompareOp compareOperator, float minLod, float maxLod, VkBorderColor borderColor, bool unnormalizedCoords, VkSampler& sampler)
{
	VkSamplerCreateInfo samplerCreateInfo = {
		VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO,    // VkStructureType          sType
		nullptr,                                  // const void             * pNext
		0,                                        // VkSamplerCreateFlags     flags
		magFilter,                                // VkFilter                 magFilter
		minFilter,                                // VkFilter                 minFilter
		mipmapMode,                               // VkSamplerMipmapMode      mipmapMode
		uAddressMode,                             // VkSamplerAddressMode     addressModeU
		vAddressMode,                             // VkSamplerAddressMode     addressModeV
		wAddressMode,                             // VkSamplerAddressMode     addressModeW
		lodBias,                                  // float                    mipLodBias
		anisotropyEnable,                         // VkBool32                 anisotropyEnable
		maxAnisotropy,                            // float                    maxAnisotropy
		compareEnable,                            // VkBool32                 compareEnable
		compareOperator,                          // VkCompareOp              compareOp
		minLod,                                   // float                    minLod
		maxLod,                                   // float                    maxLod
		borderColor,                              // VkBorderColor            borderColor
		unnormalizedCoords                        // VkBool32                 unnormalizedCoordinates
	};

	VkResult result = vkCreateSampler(logicalDevice, &samplerCreateInfo, nullptr, &sampler);
	if (result != VK_SUCCESS)
	{
		// TODO : Use Numea System Log
		printf("Could not create sampler\n");
		return false;
	}
	return true;
}

bool VulkanLoader::createSampledImage(VkPhysicalDevice physicalDevice, VkDevice logicalDevice, VkImageType type, VkFormat format, VkExtent3D size, uint32_t numMipmaps, uint32_t numLayers, VkImageUsageFlags usage, bool cubemap, VkImageViewType viewType, VkImageAspectFlags aspect, bool linearFiltering, VkImage& sampledImage, VkDeviceMemory& memoryObject, VkImageView& sampledImageView)
{
	// TODO : Cache this ?
	VkFormatProperties formatProperties;
	vkGetPhysicalDeviceFormatProperties(physicalDevice, format, &formatProperties);

	if (!(formatProperties.optimalTilingFeatures & VK_FORMAT_FEATURE_SAMPLED_IMAGE_BIT)) 
	{
		// TODO : Use Numea System Log
		printf("Provided format is not supported for a sampled image\n");
		return false;
	}

	if (linearFiltering && !(formatProperties.optimalTilingFeatures & VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_LINEAR_BIT)) 
	{
		// TODO : Use Numea System Log
		printf("Provided format is not supported for a linear image filtering\n");
		return false;
	}

	if (!createImage(logicalDevice, type, format, size, numMipmaps, numLayers, VK_SAMPLE_COUNT_1_BIT, usage | VK_IMAGE_USAGE_SAMPLED_BIT, cubemap, sampledImage)) 
	{
		return false;
	}

	if (!allocateAndBindMemoryObjectToImage(physicalDevice, logicalDevice, sampledImage, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, memoryObject)) 
	{
		return false;
	}

	if (!createImageView(logicalDevice, sampledImage, viewType, format, aspect, sampledImageView)) 
	{
		return false;
	}

	return true;
}

bool VulkanLoader::createCombinedImageSampler(VkPhysicalDevice physicalDevice, VkDevice logicalDevice, VkImageType type, VkFormat format, VkExtent3D size, uint32_t numMipmaps, uint32_t numLayers, VkImageUsageFlags usage, bool cubemap, VkImageViewType viewType, VkImageAspectFlags aspect, VkFilter magFilter, VkFilter minFilter, VkSamplerMipmapMode mipmapMode, VkSamplerAddressMode uAddressMode, VkSamplerAddressMode vAddressMode, VkSamplerAddressMode wAddressMode, float lodBias, bool anisotropyEnable, float maxAnisotropy, bool compareEnable, VkCompareOp compareOperator, float minLod, float maxLod, VkBorderColor borderColor, bool unnormalizedCoords, VkSampler& sampler, VkImage& sampledImage, VkDeviceMemory& memoryObject, VkImageView& sampledImageView)
{
	if (!createSampler(logicalDevice, magFilter, minFilter, mipmapMode, uAddressMode, vAddressMode, wAddressMode, lodBias, anisotropyEnable, maxAnisotropy, compareEnable, compareOperator, minLod, maxLod, borderColor, unnormalizedCoords, sampler)) 
	{
		return false;
	}

	bool linearFiltering = (magFilter == VK_FILTER_LINEAR) || (minFilter == VK_FILTER_LINEAR) || (mipmapMode == VK_SAMPLER_MIPMAP_MODE_LINEAR);
	if (!createSampledImage(physicalDevice, logicalDevice, type, format, size, numMipmaps, numLayers, usage, cubemap, viewType, aspect, linearFiltering, sampledImage, memoryObject, sampledImageView)) 
	{
		return false;
	}

	return true;
}

bool VulkanLoader::createStorageImage(VkPhysicalDevice physicalDevice, VkDevice logicalDevice, VkImageType type, VkFormat format, VkExtent3D size, uint32_t numMipmaps, uint32_t numLayers, VkImageUsageFlags usage, VkImageViewType viewType, VkImageAspectFlags aspect, bool atomicOperations, VkImage& storageImage, VkDeviceMemory& memoryObject, VkImageView& storageImageView)
{
	// TODO : Cache this ?
	VkFormatProperties formatProperties;
	vkGetPhysicalDeviceFormatProperties(physicalDevice, format, &formatProperties);

	if (!(formatProperties.optimalTilingFeatures & VK_FORMAT_FEATURE_STORAGE_IMAGE_BIT)) 
	{
		// TODO : Use Numea System Log
		printf("Provided format is not supported for a storage image\n");
		return false;
	}
	if (atomicOperations && !(formatProperties.optimalTilingFeatures & VK_FORMAT_FEATURE_STORAGE_IMAGE_ATOMIC_BIT)) 
	{
		// TODO : Use Numea System Log
		printf("Provided format is not supported for atomic operations on storage images\n");
		return false;
	}

	if (!createImage(logicalDevice, type, format, size, numMipmaps, numLayers, VK_SAMPLE_COUNT_1_BIT, usage | VK_IMAGE_USAGE_STORAGE_BIT, false, storageImage)) 
	{
		return false;
	}

	if (!allocateAndBindMemoryObjectToImage(physicalDevice, logicalDevice, storageImage, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, memoryObject)) 
	{
		return false;
	}

	if (!createImageView(logicalDevice, storageImage, viewType, format, aspect, storageImageView)) 
	{
		return false;
	}
	return true;
}

bool VulkanLoader::createUniformTexelBuffer(VkPhysicalDevice physicalDevice, VkDevice logicalDevice, VkFormat format, VkDeviceSize size, VkImageUsageFlags usage, VkBuffer& uniformTexelBuffer, VkDeviceMemory& memoryObject, VkBuffer& uniformTexelBufferView)
{
	// TODO : Cache this ?
	VkFormatProperties formatProperties;
	vkGetPhysicalDeviceFormatProperties(physicalDevice, format, &formatProperties);

	if (!(formatProperties.bufferFeatures & VK_FORMAT_FEATURE_UNIFORM_TEXEL_BUFFER_BIT)) 
	{
		// TODO : Use Numea System Log
		printf("Provided format is not supported for a uniform texel buffer\n");
		return false;
	}

	if (!createBuffer(logicalDevice, size, usage | VK_BUFFER_USAGE_UNIFORM_TEXEL_BUFFER_BIT, uniformTexelBuffer)) 
	{
		return false;
	}

	if (!allocateAndBindMemoryObjectToBuffer(physicalDevice, logicalDevice, uniformTexelBuffer, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, memoryObject)) 
	{
		return false;
	}

	if (!createBufferView(logicalDevice, uniformTexelBuffer, format, 0, VK_WHOLE_SIZE, uniformTexelBufferView)) 
	{
		return false;
	}

	return true;
}

bool VulkanLoader::createStorageTexelBuffer(VkPhysicalDevice physicalDevice, VkDevice logicalDevice, VkFormat format, VkDeviceSize size, VkBufferUsageFlags usage, bool atomicOperations, VkBuffer& storageTexelBuffer, VkDeviceMemory& memoryObject, VkBufferView& storageTexelBufferView)
{
	// TODO : Cache this ?
	VkFormatProperties formatProperties;
	vkGetPhysicalDeviceFormatProperties(physicalDevice, format, &formatProperties);

	if (!(formatProperties.bufferFeatures & VK_FORMAT_FEATURE_STORAGE_TEXEL_BUFFER_BIT)) 
	{
		// TODO : Use Numea System Log
		printf("Provided format is not supported for a uniform texel buffer\n");
		return false;
	}

	if (atomicOperations && !(formatProperties.bufferFeatures & VK_FORMAT_FEATURE_STORAGE_TEXEL_BUFFER_ATOMIC_BIT)) 
	{
		// TODO : Use Numea System Log
		printf("Provided format is not supported for atomic operations on storage texel buffers\n");
		return false;
	}

	if (!createBuffer(logicalDevice, size, usage | VK_BUFFER_USAGE_STORAGE_TEXEL_BUFFER_BIT, storageTexelBuffer)) 
	{
		return false;
	}

	if (!allocateAndBindMemoryObjectToBuffer(physicalDevice, logicalDevice, storageTexelBuffer, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, memoryObject)) 
	{
		return false;
	}

	if (!createBufferView(logicalDevice, storageTexelBuffer, format, 0, VK_WHOLE_SIZE, storageTexelBufferView)) 
	{
		return false;
	}

	return true;
}

bool VulkanLoader::createUniformBuffer(VkPhysicalDevice physicalDevice, VkDevice logicalDevice, VkDeviceSize size, VkBufferUsageFlags usage, VkBuffer& uniformBuffer, VkDeviceMemory& memoryObject)
{
	if (!createBuffer(logicalDevice, size, usage | VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, uniformBuffer)) 
	{
		return false;
	}

	if (!allocateAndBindMemoryObjectToBuffer(physicalDevice, logicalDevice, uniformBuffer, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, memoryObject)) 
	{
		return false;
	}

	return true;
}

bool VulkanLoader::createStorageBuffer(VkPhysicalDevice physicalDevice, VkDevice logicalDevice, VkDeviceSize size, VkBufferUsageFlags usage, VkBuffer& storageBuffer, VkDeviceMemory& memoryObject)
{
	if (!createBuffer(logicalDevice, size, usage | VK_BUFFER_USAGE_STORAGE_BUFFER_BIT, storageBuffer)) 
	{
		return false;
	}

	if (!allocateAndBindMemoryObjectToBuffer(physicalDevice, logicalDevice, storageBuffer, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, memoryObject)) 
	{
		return false;
	}

	return true;
}

bool VulkanLoader::createInputAttachment(VkPhysicalDevice physicalDevice, VkDevice logicalDevice, VkImageType type, VkFormat format, VkExtent3D size, VkImageUsageFlags usage, VkImageViewType viewType, VkImageAspectFlags aspect, VkImage& inputAttachment, VkDeviceMemory& memoryObject, VkImageView& inputAttachmentImageView)
{
	// TODO : Cache this ?
	VkFormatProperties formatProperties;
	vkGetPhysicalDeviceFormatProperties(physicalDevice, format, &formatProperties);

	if ((aspect & VK_IMAGE_ASPECT_COLOR_BIT) && !(formatProperties.optimalTilingFeatures & VK_FORMAT_FEATURE_COLOR_ATTACHMENT_BIT)) 
	{
		// TODO : Use Numea System Log
		printf("Provided format is not supported for an input attachment\n");
		return false;
	}

	if ((aspect & (VK_IMAGE_ASPECT_DEPTH_BIT | VK_IMAGE_ASPECT_STENCIL_BIT)) && !(formatProperties.optimalTilingFeatures & VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT)) 
	{
		// TODO : Use Numea System Log
		printf("Provided format is not supported for an input attachment\n");
		return false;
	}

	if (!createImage(logicalDevice, type, format, size, 1, 1, VK_SAMPLE_COUNT_1_BIT, usage | VK_IMAGE_USAGE_INPUT_ATTACHMENT_BIT, false, inputAttachment)) 
	{
		return false;
	}

	if (!allocateAndBindMemoryObjectToImage(physicalDevice, logicalDevice, inputAttachment, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, memoryObject)) 
	{
		return false;
	}

	if (!createImageView(logicalDevice, inputAttachment, viewType, format, aspect, inputAttachmentImageView))
	{
		return false;
	}

	return true;
}

bool VulkanLoader::createDescriptorSetLayout(VkDevice logicalDevice, const std::vector<VkDescriptorSetLayoutBinding>& bindings, VkDescriptorSetLayout& descriptorSetLayout)
{
	VkDescriptorSetLayoutCreateInfo descriptorSetLayoutCreateInfo = {
		VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO,  // VkStructureType                      sType
		nullptr,                                              // const void                         * pNext
		0,                                                    // VkDescriptorSetLayoutCreateFlags     flags
		static_cast<uint32_t>(bindings.size()),               // uint32_t                             bindingCount
		bindings.data()                                       // const VkDescriptorSetLayoutBinding * pBindings
	};

	VkResult result = vkCreateDescriptorSetLayout(logicalDevice, &descriptorSetLayoutCreateInfo, nullptr, &descriptorSetLayout);
	if (result != VK_SUCCESS) 
	{
		// TODO : Use Numea System Log
		printf("Could not create a layout for descriptor sets\n");
		return false;
	}

	return true;
}

bool VulkanLoader::createDescriptorPool(VkDevice logicalDevice, bool freeIndividualSets, uint32_t maxSetsCount, const std::vector<VkDescriptorPoolSize>& descriptorTypes, VkDescriptorPool& descriptorPool)
{
	VkDescriptorPoolCreateInfo descriptorPoolCreateInfo = {
		VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO,                // VkStructureType                sType
		nullptr,                                                      // const void                   * pNext
		freeIndividualSets ?                                          // VkDescriptorPoolCreateFlags    flags
		VK_DESCRIPTOR_POOL_CREATE_FREE_DESCRIPTOR_SET_BIT : 0u,
		maxSetsCount,                                                 // uint32_t                       maxSets
		static_cast<uint32_t>(descriptorTypes.size()),                // uint32_t                       poolSizeCount
		descriptorTypes.data()                                        // const VkDescriptorPoolSize   * pPoolSizes
	};

	VkResult result = vkCreateDescriptorPool(logicalDevice, &descriptorPoolCreateInfo, nullptr, &descriptorPool);
	if (result != VK_SUCCESS)
	{
		// TODO : Use Numea System Log
		printf("Could not create a descriptor pool\n");
		return false;
	}

	return true;
}

bool VulkanLoader::allocateDescriptorSets(VkDevice logicalDevice, VkDescriptorPool descriptorPool, const std::vector<VkDescriptorSetLayout>& descriptorSetLayouts, std::vector<VkDescriptorSet>& descriptorSets)
{
	if (descriptorSetLayouts.size() > 0) 
	{
		VkDescriptorSetAllocateInfo descriptorSetAllocateInfo = {
			VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO,         // VkStructureType                  sType
			nullptr,                                                // const void                     * pNext
			descriptorPool,                                         // VkDescriptorPool                 descriptorPool
			static_cast<uint32_t>(descriptorSetLayouts.size()),     // uint32_t                         descriptorSetCount
			descriptorSetLayouts.data()                             // const VkDescriptorSetLayout    * pSetLayouts
		};

		descriptorSets.resize(descriptorSetLayouts.size());

		VkResult result = vkAllocateDescriptorSets(logicalDevice, &descriptorSetAllocateInfo, descriptorSets.data());
		if (result != VK_SUCCESS)
		{
			// TODO : Use Numea System Log
			printf("Could not allocate descriptor sets\n");
			return false;
		}

		return true;
	}
	return false;
}

void VulkanLoader::updateDescriptorSets(VkDevice logicalDevice, const std::vector<ImageDescriptorInfo>& imageDescriptorInfos, const std::vector<BufferDescriptorInfo>& bufferDescriptorInfos, const std::vector<TexelBufferDescriptorInfo>& texelBufferDescriptorInfos, const std::vector<CopyDescriptorInfo>& copyDescriptorInfos)
{
	std::vector<VkWriteDescriptorSet> writeDescriptors;
	std::vector<VkCopyDescriptorSet> copyDescriptors;

	// Image descriptors
	for (auto& imageDescriptor : imageDescriptorInfos) 
	{
		writeDescriptors.push_back({
			VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET,                                 // VkStructureType                  sType
			nullptr,                                                                // const void                     * pNext
			imageDescriptor.targetDescriptorSet,                                    // VkDescriptorSet                  dstSet
			imageDescriptor.targetDescriptorBinding,                                // uint32_t                         dstBinding
			imageDescriptor.targetArrayElement,                                     // uint32_t                         dstArrayElement
			static_cast<uint32_t>(imageDescriptor.imageInfos.size()),               // uint32_t                         descriptorCount
			imageDescriptor.targetDescriptorType,                                   // VkDescriptorType                 descriptorType
			imageDescriptor.imageInfos.data(),                                      // const VkDescriptorImageInfo    * pImageInfo
			nullptr,                                                                // const VkDescriptorBufferInfo   * pBufferInfo
			nullptr                                                                 // const VkBufferView             * pTexelBufferView
		});
	}

	// Buffer descriptors
	for (auto& bufferDescriptor : bufferDescriptorInfos) 
	{
		writeDescriptors.push_back({
			VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET,                                 // VkStructureType                  sType
			nullptr,                                                                // const void                     * pNext
			bufferDescriptor.targetDescriptorSet,                                  // VkDescriptorSet                  dstSet
			bufferDescriptor.targetDescriptorBinding,                              // uint32_t                         dstBinding
			bufferDescriptor.targetArrayElement,                                   // uint32_t                         dstArrayElement
			static_cast<uint32_t>(bufferDescriptor.bufferInfos.size()),            // uint32_t                         descriptorCount
			bufferDescriptor.targetDescriptorType,                                 // VkDescriptorType                 descriptorType
			nullptr,                                                                // const VkDescriptorImageInfo    * pImageInfo
			bufferDescriptor.bufferInfos.data(),                                   // const VkDescriptorBufferInfo   * pBufferInfo
			nullptr                                                                 // const VkBufferView             * pTexelBufferView
		});
	}

	// Texel buffer descriptors
	for (auto& texelBufferDescriptor : texelBufferDescriptorInfos) 
	{
		writeDescriptors.push_back({
			VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET,                                 // VkStructureType                  sType
			nullptr,                                                                // const void                     * pNext
			texelBufferDescriptor.targetDescriptorSet,                              // VkDescriptorSet                  dstSet
			texelBufferDescriptor.targetDescriptorBinding,                          // uint32_t                         dstBinding
			texelBufferDescriptor.targetArrayElement,                               // uint32_t                         dstArrayElement
			static_cast<uint32_t>(texelBufferDescriptor.texelBufferViews.size()),   // uint32_t                         descriptorCount
			texelBufferDescriptor.targetDescriptorType,                             // VkDescriptorType                 descriptorType
			nullptr,                                                                // const VkDescriptorImageInfo    * pImageInfo
			nullptr,                                                                // const VkDescriptorBufferInfo   * pBufferInfo
			texelBufferDescriptor.texelBufferViews.data()                           // const VkBufferView             * pTexelBufferView
		});
	}

	// Copy descriptors
	for (auto& copyDescriptor : copyDescriptorInfos) 
	{
		copyDescriptors.push_back({
			VK_STRUCTURE_TYPE_COPY_DESCRIPTOR_SET,                                  // VkStructureType    sType
			nullptr,                                                                // const void       * pNext
			copyDescriptor.sourceDescriptorSet,                                    // VkDescriptorSet    srcSet
			copyDescriptor.sourceDescriptorBinding,                                // uint32_t           srcBinding
			copyDescriptor.sourceArrayElement,                                     // uint32_t           srcArrayElement
			copyDescriptor.targetDescriptorSet,                                    // VkDescriptorSet    dstSet
			copyDescriptor.targetDescriptorBinding,                                // uint32_t           dstBinding
			copyDescriptor.targetArrayElement,                                     // uint32_t           dstArrayElement
			copyDescriptor.descriptorCount                                         // uint32_t           descriptorCount
		});
	}

	vkUpdateDescriptorSets(logicalDevice, static_cast<uint32_t>(writeDescriptors.size()), writeDescriptors.data(), static_cast<uint32_t>(copyDescriptors.size()), copyDescriptors.data());
}

void VulkanLoader::bindDescriptorSets(VkCommandBuffer commandBuffer, VkPipelineBindPoint pipelineType, VkPipelineLayout pipelineLayout, uint32_t indexForFirstSet, const std::vector<VkDescriptorSet>& descriptorSets, const std::vector<uint32_t>& dynamicOffsets)
{
	vkCmdBindDescriptorSets(commandBuffer, pipelineType, pipelineLayout, indexForFirstSet, static_cast<uint32_t>(descriptorSets.size()), descriptorSets.data(), static_cast<uint32_t>(dynamicOffsets.size()), dynamicOffsets.data());
}

bool VulkanLoader::createDescriptorsWithTextureAndUniformBuffer(VkPhysicalDevice physicalDevice, VkDevice logicalDevice, VkExtent3D sampledImageSize, uint32_t uniformBufferSize, VkSampler& sampler, VkImage& sampledImage, VkDeviceMemory& sampledImageMemoryObject, VkImageView& sampledImageView, VkBuffer& uniformBuffer, VkDeviceMemory& uniformBufferMemoryObject, VkDescriptorSetLayout& descriptorSetLayout, VkDescriptorPool& descriptorPool, std::vector<VkDescriptorSet>& descriptorSets)
{
	if (!createCombinedImageSampler(physicalDevice, logicalDevice, VK_IMAGE_TYPE_2D, VK_FORMAT_R8G8B8A8_UNORM, sampledImageSize, 1, 1, VK_IMAGE_USAGE_TRANSFER_DST_BIT,
		false, VK_IMAGE_VIEW_TYPE_2D, VK_IMAGE_ASPECT_COLOR_BIT, VK_FILTER_LINEAR, VK_FILTER_LINEAR, VK_SAMPLER_MIPMAP_MODE_NEAREST, VK_SAMPLER_ADDRESS_MODE_REPEAT,
		VK_SAMPLER_ADDRESS_MODE_REPEAT, VK_SAMPLER_ADDRESS_MODE_REPEAT, 0.0f, false, 1.0f, false, VK_COMPARE_OP_ALWAYS, 0.0f, 0.0f, VK_BORDER_COLOR_FLOAT_OPAQUE_BLACK, false,
		sampler, sampledImage, sampledImageMemoryObject, sampledImageView)) 
	{
		return false;
	}

	if (!createUniformBuffer(physicalDevice, logicalDevice, uniformBufferSize, VK_BUFFER_USAGE_TRANSFER_DST_BIT, uniformBuffer, uniformBufferMemoryObject)) 
	{
		return false;
	}

	std::vector<VkDescriptorSetLayoutBinding> bindings = {
		{
			0,                                                          // uint32_t             binding
			VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,                  // VkDescriptorType     descriptorType
			1,                                                          // uint32_t             descriptorCount
			VK_SHADER_STAGE_FRAGMENT_BIT,                               // VkShaderStageFlags   stageFlags
			nullptr                                                     // const VkSampler    * pImmutableSamplers
		},
		{
			1,                                                          // uint32_t             binding
			VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,                          // VkDescriptorType     descriptorType
			1,                                                          // uint32_t             descriptorCount
			VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT,  // VkShaderStageFlags   stageFlags
			nullptr                                                     // const VkSampler    * pImmutableSamplers
		}
	};

	if (!createDescriptorSetLayout(logicalDevice, bindings, descriptorSetLayout)) 
	{
		return false;
	}

	std::vector<VkDescriptorPoolSize> descriptorTypes = {
		{
			VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,                  // VkDescriptorType     type
			1                                                           // uint32_t             descriptorCount
		},
		{
			VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,                          // VkDescriptorType     type
			1                                                           // uint32_t             descriptorCount
		}
	};

	if (!createDescriptorPool(logicalDevice, false, 1, descriptorTypes, descriptorPool)) {
		return false;
	}

	if (!allocateDescriptorSets(logicalDevice, descriptorPool, { descriptorSetLayout }, descriptorSets)) 
	{
		return false;
	}

	std::vector<ImageDescriptorInfo> imageDescriptorInfos = {
		{
			descriptorSets[0],                          // VkDescriptorSet                      targetDescriptorSet
			0,                                          // uint32_t                             targetDescriptorBinding
			0,                                          // uint32_t                             targetArrayElement
			VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,  // VkDescriptorType                     targetDescriptorType
			{                                           // std::vector<VkDescriptorImageInfo>   imageInfos
				{
					sampler,                                  // VkSampler                            sampler
					sampledImageView,                         // VkImageView                          imageView
					VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL  // VkImageLayout                        imageLayout
				}
			}
		}
	};

	std::vector<BufferDescriptorInfo> bufferDescriptorInfos = {
		{
			descriptorSets[0],                          // VkDescriptorSet                      targetDescriptorSet
			1,                                          // uint32_t                             targetDescriptorBinding
			0,                                          // uint32_t                             targetArrayElement
			VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,          // VkDescriptorType                     targetDescriptorType
			{                                           // std::vector<VkDescriptorBufferInfo>  bufferInfos
				{
					uniformBuffer,                            // VkBuffer                             buffer
					0,                                        // VkDeviceSize                         offset
					VK_WHOLE_SIZE                             // VkDeviceSize                         range
				}
			}
		}
	};

	updateDescriptorSets(logicalDevice, imageDescriptorInfos, bufferDescriptorInfos, {}, {});

	return true;
}

bool VulkanLoader::freeDescriptorSets(VkDevice logicalDevice, VkDescriptorPool descriptorPool, std::vector<VkDescriptorSet>& descriptorSets)
{
	if (descriptorSets.size() > 0) 
	{
		VkResult result = vkFreeDescriptorSets(logicalDevice, descriptorPool, static_cast<uint32_t>(descriptorSets.size()), descriptorSets.data());
		descriptorSets.clear();

		if (result != VK_SUCCESS) 
		{
			// TODO : Use Numea System Log
			printf("Error occurred during freeing descriptor sets\n");
			return false;
		}
	}
	return true;
}

bool VulkanLoader::resetDescriptorPool(VkDevice logicalDevice, VkDescriptorPool descriptorPool)
{
	VkResult result = vkResetDescriptorPool(logicalDevice, descriptorPool, 0);
	if (result != VK_SUCCESS)
	{
		// TODO : Use Numea System Log
		printf("Error occurred during descriptor pool reset\n");
		return false;
	}

	return true;
}

void VulkanLoader::destroyDescriptorPool(VkDevice logicalDevice, VkDescriptorPool& descriptorPool)
{
	if (descriptorPool != VK_NULL_HANDLE)
	{
		vkDestroyDescriptorPool(logicalDevice, descriptorPool, nullptr);
		descriptorPool = VK_NULL_HANDLE;
	}
}

void VulkanLoader::destroyDescriptorSetLayout(VkDevice logicalDevice, VkDescriptorSetLayout& descriptorSetLayout)
{
	if (descriptorSetLayout != VK_NULL_HANDLE)
	{
		vkDestroyDescriptorSetLayout(logicalDevice, descriptorSetLayout, nullptr);
		descriptorSetLayout = VK_NULL_HANDLE;
	}
}

void VulkanLoader::destroySampler(VkDevice logicalDevice, VkSampler& sampler)
{
	if (sampler != VK_NULL_HANDLE)
	{
		vkDestroySampler(logicalDevice, sampler, nullptr);
		sampler = VK_NULL_HANDLE;
	}
}

void VulkanLoader::specifyAttachmentsDescriptions(const std::vector<VkAttachmentDescription>& attachmentsDescriptions)
{
	// TODO : What is this used for ???
}

void VulkanLoader::specifySubpassDescriptions(const std::vector<SubpassParameters>& subpassParameters, std::vector<VkSubpassDescription>& subpassDescriptions)
{
	subpassDescriptions.clear();

	for (auto& subpassDescription : subpassParameters) 
	{
		subpassDescriptions.push_back({
			0,                                                                      // VkSubpassDescriptionFlags        flags
			subpassDescription.pipelineType,                                        // VkPipelineBindPoint              pipelineBindPoint
			static_cast<uint32_t>(subpassDescription.inputAttachments.size()),      // uint32_t                         inputAttachmentCount
			subpassDescription.inputAttachments.data(),                             // const VkAttachmentReference    * pInputAttachments
			static_cast<uint32_t>(subpassDescription.colorAttachments.size()),      // uint32_t                         colorAttachmentCount
			subpassDescription.colorAttachments.data(),                             // const VkAttachmentReference    * pColorAttachments
			subpassDescription.resolveAttachments.data(),                           // const VkAttachmentReference    * pResolveAttachments
			subpassDescription.depthStencilAttachment,                              // const VkAttachmentReference    * pDepthStencilAttachment
			static_cast<uint32_t>(subpassDescription.preserveAttachments.size()),   // uint32_t                         preserveAttachmentCount
			subpassDescription.preserveAttachments.data()                           // const uint32_t                 * pPreserveAttachments
		});
	}
}

void VulkanLoader::specifyDependenciesBetweenSubpasses(const std::vector<VkSubpassDependency>& subpassesDependencies)
{
	// TODO : What is this used for ???
}

bool VulkanLoader::createRenderPass(VkDevice logicalDevice, const std::vector<VkAttachmentDescription>& attachmentsDescriptions, const std::vector<SubpassParameters>& subpassParameters, const std::vector<VkSubpassDependency>& subpassDependencies, VkRenderPass& renderPass)
{
	// TODO : What is this used for ???
	specifyAttachmentsDescriptions(attachmentsDescriptions);

	std::vector<VkSubpassDescription> subpassDescriptions;
	specifySubpassDescriptions(subpassParameters, subpassDescriptions);

	// TODO : What is this used for ???
	specifyDependenciesBetweenSubpasses(subpassDependencies);

	VkRenderPassCreateInfo renderPassCreateInfo = {
		VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO,                // VkStructureType                    sType
		nullptr,                                                  // const void                       * pNext
		0,                                                        // VkRenderPassCreateFlags            flags
		static_cast<uint32_t>(attachmentsDescriptions.size()),    // uint32_t                           attachmentCount
		attachmentsDescriptions.data(),                           // const VkAttachmentDescription    * pAttachments
		static_cast<uint32_t>(subpassDescriptions.size()),        // uint32_t                           subpassCount
		subpassDescriptions.data(),                               // const VkSubpassDescription       * pSubpasses
		static_cast<uint32_t>(subpassDependencies.size()),        // uint32_t                           dependencyCount
		subpassDependencies.data()                                // const VkSubpassDependency        * pDependencies
	};

	VkResult result = vkCreateRenderPass(logicalDevice, &renderPassCreateInfo, nullptr, &renderPass);
	if (result != VK_SUCCESS) 
	{
		// TODO : Use Numea System Log
		printf("Could not create a render pass\n");
		return false;
	}
	return true;
}

bool VulkanLoader::createFramebuffer(VkDevice logicalDevice, VkRenderPass renderPass, const std::vector<VkImageView>& attachments, uint32_t width, uint32_t height, uint32_t layers, VkFramebuffer& framebuffer)
{
	VkFramebufferCreateInfo framebufferCreateInfo = {
		VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO,    // VkStructureType              sType
		nullptr,                                      // const void                 * pNext
		0,                                            // VkFramebufferCreateFlags     flags
		renderPass,                                   // VkRenderPass                 renderPass
		static_cast<uint32_t>(attachments.size()),    // uint32_t                     attachmentCount
		attachments.data(),                           // const VkImageView          * pAttachments
		width,                                        // uint32_t                     width
		height,                                       // uint32_t                     height
		layers                                        // uint32_t                     layers
	};

	VkResult result = vkCreateFramebuffer(logicalDevice, &framebufferCreateInfo, nullptr, &framebuffer);
	if (result != VK_SUCCESS)
	{
		// TODO : Use Numea System Log
		printf("Could not create a framebuffer\n");
		return false;
	}
	return true;
}

bool VulkanLoader::prepareRenderPassForGeometryRenderingAndPostprocessSubpasses(VkDevice logicalDevice, VkRenderPass renderPass)
{
	std::vector<VkAttachmentDescription> attachmentsDescriptions = {
		{
			0,                                                // VkAttachmentDescriptionFlags     flags
			VK_FORMAT_R8G8B8A8_UNORM,                         // VkFormat                         format
			VK_SAMPLE_COUNT_1_BIT,                            // VkSampleCountFlagBits            samples
			VK_ATTACHMENT_LOAD_OP_CLEAR,                      // VkAttachmentLoadOp               loadOp
			VK_ATTACHMENT_STORE_OP_DONT_CARE,                 // VkAttachmentStoreOp              storeOp
			VK_ATTACHMENT_LOAD_OP_DONT_CARE,                  // VkAttachmentLoadOp               stencilLoadOp
			VK_ATTACHMENT_STORE_OP_DONT_CARE,                 // VkAttachmentStoreOp              stencilStoreOp
			VK_IMAGE_LAYOUT_UNDEFINED,                        // VkImageLayout                    initialLayout
			VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL,         // VkImageLayout                    finalLayout
		},
		{
			0,                                                // VkAttachmentDescriptionFlags     flags
			VK_FORMAT_D16_UNORM,                              // VkFormat                         format
			VK_SAMPLE_COUNT_1_BIT,                            // VkSampleCountFlagBits            samples
			VK_ATTACHMENT_LOAD_OP_CLEAR,                      // VkAttachmentLoadOp               loadOp
			VK_ATTACHMENT_STORE_OP_DONT_CARE,                 // VkAttachmentStoreOp              storeOp
			VK_ATTACHMENT_LOAD_OP_DONT_CARE,                  // VkAttachmentLoadOp               stencilLoadOp
			VK_ATTACHMENT_STORE_OP_DONT_CARE,                 // VkAttachmentStoreOp              stencilStoreOp
			VK_IMAGE_LAYOUT_UNDEFINED,                        // VkImageLayout                    initialLayout
			VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL, // VkImageLayout                    finalLayout
		},
		{
			0,                                                // VkAttachmentDescriptionFlags     flags
			VK_FORMAT_R8G8B8A8_UNORM,                         // VkFormat                         format
			VK_SAMPLE_COUNT_1_BIT,                            // VkSampleCountFlagBits            samples
			VK_ATTACHMENT_LOAD_OP_CLEAR,                      // VkAttachmentLoadOp               loadOp
			VK_ATTACHMENT_STORE_OP_STORE,                     // VkAttachmentStoreOp              storeOp
			VK_ATTACHMENT_LOAD_OP_DONT_CARE,                  // VkAttachmentLoadOp               stencilLoadOp
			VK_ATTACHMENT_STORE_OP_DONT_CARE,                 // VkAttachmentStoreOp              stencilStoreOp
			VK_IMAGE_LAYOUT_UNDEFINED,                        // VkImageLayout                    initialLayout
			VK_IMAGE_LAYOUT_PRESENT_SRC_KHR,                  // VkImageLayout                    finalLayout
		},
	};

	VkAttachmentReference depthStencilAttachment = {
		1,                                                  // uint32_t                             attachment
		VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL,   // VkImageLayout                        layout
	};

	std::vector<SubpassParameters> subpassParameters = {
		// #0 subpass
		{
			VK_PIPELINE_BIND_POINT_GRAPHICS,                  // VkPipelineBindPoint                  pipelineType
			{},                                               // std::vector<VkAttachmentReference>   inputAttachments
			{                                                 // std::vector<VkAttachmentReference>   colorAttachments
				{
					0,                                              // uint32_t                             attachment
					VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL        // VkImageLayout                        layout
				}
			},
			{},                                               // std::vector<VkAttachmentReference>   resolveAttachments
			&depthStencilAttachment,                          // const VkAttachmentReference        * depthStencilAttachment
			{}                                                // std::vector<uint32_t>                preserveAttachments
		},
		// #1 subpass
		{
			VK_PIPELINE_BIND_POINT_GRAPHICS,                  // VkPipelineBindPoint                  pipelineType
			{                                                 // std::vector<VkAttachmentReference>   inputAttachments
				{
					0,                                              // uint32_t                             attachment
					VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL        // VkImageLayout                        layout
				}
			},
			{                                                 // std::vector<VkAttachmentReference>   colorAttachments
				{
					2,                                              // uint32_t                             attachment
					VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL        // VkImageLayout                        layout
				}
			},
			{},                                               // std::vector<VkAttachmentReference>   resolveAttachments
			nullptr,                                          // const VkAttachmentReference        * depthStencilAttachment
			{}                                                // std::vector<uint32_t>                preserveAttachments
		}
	};

	std::vector<VkSubpassDependency> subpassDependencies = {
		{
			0,                                              // uint32_t                 srcSubpass
			1,                                              // uint32_t                 dstSubpass
			VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,  // VkPipelineStageFlags     srcStageMask
			VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT,          // VkPipelineStageFlags     dstStageMask
			VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT,           // VkAccessFlags            srcAccessMask
			VK_ACCESS_INPUT_ATTACHMENT_READ_BIT,            // VkAccessFlags            dstAccessMask
			VK_DEPENDENCY_BY_REGION_BIT                     // VkDependencyFlags        dependencyFlags
		}
	};

	if (!createRenderPass(logicalDevice, attachmentsDescriptions, subpassParameters, subpassDependencies, renderPass)) 
	{
		return false;
	}

	return true;
}

bool VulkanLoader::prepareRenderPassAndFramebufferWithColorAndDepthAttachments(VkPhysicalDevice physicalDevice, VkDevice logicalDevice, uint32_t width, uint32_t height, VkImage& colorImage, VkDeviceMemory& colorImageMemoryObject, VkImageView& colorImageView, VkImage& depthImage, VkDeviceMemory& depthImageMemoryObject, VkImageView& depthImageView, VkRenderPass& renderPass, VkFramebuffer& framebuffer)
{
	if (!create2DImageAndView(physicalDevice, logicalDevice, VK_FORMAT_R8G8B8A8_UNORM, { width, height }, 1, 1, VK_SAMPLE_COUNT_1_BIT,
		VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_SAMPLED_BIT, VK_IMAGE_ASPECT_COLOR_BIT, colorImage, colorImageMemoryObject, colorImageView)) 
	{
		return false;
	}

	if (!create2DImageAndView(physicalDevice, logicalDevice, VK_FORMAT_D16_UNORM, { width, height }, 1, 1, VK_SAMPLE_COUNT_1_BIT,
		VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT | VK_IMAGE_USAGE_SAMPLED_BIT, VK_IMAGE_ASPECT_DEPTH_BIT, depthImage, depthImageMemoryObject, depthImageView)) 
	{
		return false;
	}

	std::vector<VkAttachmentDescription> attachmentsDescriptions = {
		{
			0,                                                // VkAttachmentDescriptionFlags     flags
			VK_FORMAT_R8G8B8A8_UNORM,                         // VkFormat                         format
			VK_SAMPLE_COUNT_1_BIT,                            // VkSampleCountFlagBits            samples
			VK_ATTACHMENT_LOAD_OP_CLEAR,                      // VkAttachmentLoadOp               loadOp
			VK_ATTACHMENT_STORE_OP_STORE,                     // VkAttachmentStoreOp              storeOp
			VK_ATTACHMENT_LOAD_OP_DONT_CARE,                  // VkAttachmentLoadOp               stencilLoadOp
			VK_ATTACHMENT_STORE_OP_DONT_CARE,                 // VkAttachmentStoreOp              stencilStoreOp
			VK_IMAGE_LAYOUT_UNDEFINED,                        // VkImageLayout                    initialLayout
			VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL,         // VkImageLayout                    finalLayout
		},
		{
			0,                                                // VkAttachmentDescriptionFlags     flags
			VK_FORMAT_D16_UNORM,                              // VkFormat                         format
			VK_SAMPLE_COUNT_1_BIT,                            // VkSampleCountFlagBits            samples
			VK_ATTACHMENT_LOAD_OP_CLEAR,                      // VkAttachmentLoadOp               loadOp
			VK_ATTACHMENT_STORE_OP_STORE,                     // VkAttachmentStoreOp              storeOp
			VK_ATTACHMENT_LOAD_OP_DONT_CARE,                  // VkAttachmentLoadOp               stencilLoadOp
			VK_ATTACHMENT_STORE_OP_DONT_CARE,                 // VkAttachmentStoreOp              stencilStoreOp
			VK_IMAGE_LAYOUT_UNDEFINED,                        // VkImageLayout                    initialLayout
			VK_IMAGE_LAYOUT_DEPTH_STENCIL_READ_ONLY_OPTIMAL,  // VkImageLayout                    finalLayout
		}
	};

	VkAttachmentReference depthStencilAttachment = {
		1,                                                  // uint32_t                             attachment
		VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL,   // VkImageLayout                        layout
	};

	std::vector<SubpassParameters> subpassParameters = {
		{
			VK_PIPELINE_BIND_POINT_GRAPHICS,                  // VkPipelineBindPoint                  PipelineType
			{},                                               // std::vector<VkAttachmentReference>   InputAttachments
			{                                                 // std::vector<VkAttachmentReference>   ColorAttachments
				{
					0,                                              // uint32_t                             attachment
					VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL        // VkImageLayout                        layout
				}
			},
			{},                                               // std::vector<VkAttachmentReference>   ResolveAttachments
			&depthStencilAttachment,                        // const VkAttachmentReference        * DepthStencilAttachment
			{}                                                // std::vector<uint32_t>                PreserveAttachments
		}
	};

	std::vector<VkSubpassDependency> subpassDependencies = {
		{
			0,                                                // uint32_t                 srcSubpass
			VK_SUBPASS_EXTERNAL,                              // uint32_t                 dstSubpass
			VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,    // VkPipelineStageFlags     srcStageMask
			VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT,            // VkPipelineStageFlags     dstStageMask
			VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT,             // VkAccessFlags            srcAccessMask
			VK_ACCESS_SHADER_READ_BIT,                        // VkAccessFlags            dstAccessMask
			0                                                 // VkDependencyFlags        dependencyFlags
		}
	};

	if (!createRenderPass(logicalDevice, attachmentsDescriptions, subpassParameters, subpassDependencies, renderPass))
	{
		return false;
	}

	if (!createFramebuffer(logicalDevice, renderPass, { colorImageView, depthImageView }, width, height, 1, framebuffer)) 
	{
		return false;
	}

	return true;
}

void VulkanLoader::beginRenderPass(VkCommandBuffer commandBuffer, VkRenderPass renderPass, VkFramebuffer framebuffer, VkRect2D renderArea, const std::vector<VkClearValue>& clearValues, VkSubpassContents subpassContents)
{
	VkRenderPassBeginInfo renderPassBeginInfo = {
		VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO,     // VkStructureType        sType
		nullptr,                                      // const void           * pNext
		renderPass,                                   // VkRenderPass           renderPass
		framebuffer,                                  // VkFramebuffer          framebuffer
		renderArea,                                   // VkRect2D               renderArea
		static_cast<uint32_t>(clearValues.size()),    // uint32_t               clearValueCount
		clearValues.data()                            // const VkClearValue   * pClearValues
	};

	vkCmdBeginRenderPass(commandBuffer, &renderPassBeginInfo, subpassContents);
}

void VulkanLoader::progressToTheNextSubpass(VkCommandBuffer commandBuffer, VkSubpassContents subpassContents)
{
	vkCmdNextSubpass(commandBuffer, subpassContents);
}

void VulkanLoader::endRenderPass(VkCommandBuffer commandBuffer)
{
	vkCmdEndRenderPass(commandBuffer);
}

void VulkanLoader::destroyFramebuffer(VkDevice logicalDevice, VkFramebuffer& framebuffer)
{
	if (framebuffer != VK_NULL_HANDLE)
	{
		vkDestroyFramebuffer(logicalDevice, framebuffer, nullptr);
		framebuffer = VK_NULL_HANDLE;
	}
}

void VulkanLoader::destroyRenderPass(VkDevice logicalDevice, VkRenderPass& renderPass)
{
	if (renderPass != VK_NULL_HANDLE)
	{
		vkDestroyRenderPass(logicalDevice, renderPass, nullptr);
		renderPass = VK_NULL_HANDLE;
	}
}

bool VulkanLoader::createShaderModule(VkDevice logicalDevice, const std::vector<unsigned char>& sourceCode, VkShaderModule& shaderModule)
{
	VkShaderModuleCreateInfo shaderModuleCreateInfo = {
		VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO,              // VkStructureType              sType
		nullptr,                                                  // const void                 * pNext
		0,                                                        // VkShaderModuleCreateFlags    flags
		sourceCode.size(),                                        // size_t                       codeSize
		reinterpret_cast<uint32_t const *>(sourceCode.data())     // const uint32_t             * pCode
	};

	VkResult result = vkCreateShaderModule(logicalDevice, &shaderModuleCreateInfo, nullptr, &shaderModule);
	if (result != VK_SUCCESS)
	{
		// TODO : Use Numea System Log
		printf("Could not create a shader module\n");
		return false;
	}

	return true;
}

void VulkanLoader::specifyPipelineShaderStages(const std::vector<ShaderStageParameters>& shaderStageParams, std::vector<VkPipelineShaderStageCreateInfo>& shaderStageCreateInfos)
{
	shaderStageCreateInfos.clear();
	for (auto& shaderStage : shaderStageParams) 
	{
		shaderStageCreateInfos.push_back({
			VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO,  // VkStructureType                    sType
			nullptr,                                              // const void                       * pNext
			0,                                                    // VkPipelineShaderStageCreateFlags   flags
			shaderStage.shaderStage,                              // VkShaderStageFlagBits              stage
			shaderStage.shaderModule,                             // VkShaderModule                     module
			shaderStage.entryPointName,                           // const char                       * pName
			shaderStage.specializationInfo                        // const VkSpecializationInfo       * pSpecializationInfo
		});
	}
}

void VulkanLoader::specifyPipelineVertexInputState(const std::vector<VkVertexInputBindingDescription>& bindingDescriptions, const std::vector<VkVertexInputAttributeDescription>& attributeDescriptions, VkPipelineVertexInputStateCreateInfo& vertexInputStateCreateInfo)
{
	vertexInputStateCreateInfo = {
		VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO,    // VkStructureType                           sType
		nullptr,                                                      // const void                              * pNext
		0,                                                            // VkPipelineVertexInputStateCreateFlags     flags
		static_cast<uint32_t>(bindingDescriptions.size()),            // uint32_t                                  vertexBindingDescriptionCount
		bindingDescriptions.data(),                                   // const VkVertexInputBindingDescription   * pVertexBindingDescriptions
		static_cast<uint32_t>(attributeDescriptions.size()),          // uint32_t                                  vertexAttributeDescriptionCount
		attributeDescriptions.data()                                  // const VkVertexInputAttributeDescription * pVertexAttributeDescriptions
	};
}

void VulkanLoader::specifyPipelineInputAssemblyState(VkPrimitiveTopology topology, bool primitiveRestartEnable, VkPipelineInputAssemblyStateCreateInfo& inputAssemblyStateCreateInfo)
{
	inputAssemblyStateCreateInfo = {
		VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO,  // VkStructureType                           sType
		nullptr,                                                      // const void                              * pNext
		0,                                                            // VkPipelineInputAssemblyStateCreateFlags   flags
		topology,                                                     // VkPrimitiveTopology                       topology
		primitiveRestartEnable                                        // VkBool32                                  primitiveRestartEnable
	};
}

void VulkanLoader::specifyPipelineTessellationState(uint32_t patchControlPointsCount, VkPipelineTessellationStateCreateInfo& tessellationStateCreateInfo)
{
	tessellationStateCreateInfo = {
		VK_STRUCTURE_TYPE_PIPELINE_TESSELLATION_STATE_CREATE_INFO,    // VkStructureType                            sType
		nullptr,                                                      // const void                               * pNext
		0,                                                            // VkPipelineTessellationStateCreateFlags     flags
		patchControlPointsCount                                       // uint32_t                                   patchControlPoints
	};
}

void VulkanLoader::specifyPipelineViewportAndScissorTestState(const ViewportInfo& viewportInfos, VkPipelineViewportStateCreateInfo& viewportStateCreateInfo)
{
	uint32_t viewportCount = static_cast<uint32_t>(viewportInfos.viewports.size());
	uint32_t scissorCount = static_cast<uint32_t>(viewportInfos.scissors.size());

	viewportStateCreateInfo = {
		VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO,    // VkStructureType                      sType
		nullptr,                                                  // const void                         * pNext
		0,                                                        // VkPipelineViewportStateCreateFlags   flags
		viewportCount,                                            // uint32_t                             viewportCount
		viewportInfos.viewports.data(),                           // const VkViewport                   * pViewports
		scissorCount,                                             // uint32_t                             scissorCount
		viewportInfos.scissors.data()                             // const VkRect2D                     * pScissors
	};
}

void VulkanLoader::specifyPipelineRasterizationState(bool depthClampEnable, bool rasterizerDiscardEnable, VkPolygonMode polygonMode, VkCullModeFlags cullingMode, VkFrontFace frontFace, bool depthBiasEnable, float depthBiasConstantFactor, float depthBiasClamp, float depthBiasSlopeFactor, float lineWidth, VkPipelineRasterizationStateCreateInfo& rasterizationStateCreateInfo)
{
	rasterizationStateCreateInfo = {
		VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO, // VkStructureType                            sType
		nullptr,                                                    // const void                               * pNext
		0,                                                          // VkPipelineRasterizationStateCreateFlags    flags
		depthClampEnable,                                           // VkBool32                                   depthClampEnable
		rasterizerDiscardEnable,                                    // VkBool32                                   rasterizerDiscardEnable
		polygonMode,                                                // VkPolygonMode                              polygonMode
		cullingMode,                                                // VkCullModeFlags                            cullMode
		frontFace,                                                  // VkFrontFace                                frontFace
		depthBiasEnable,                                            // VkBool32                                   depthBiasEnable
		depthBiasConstantFactor,                                    // float                                      depthBiasConstantFactor
		depthBiasClamp,                                             // float                                      depthBiasClamp
		depthBiasSlopeFactor,                                       // float                                      depthBiasSlopeFactor
		lineWidth                                                   // float                                      lineWidth
	};
}

void VulkanLoader::specifyPipelineMultisampleState(VkSampleCountFlagBits sampleCount, bool perSampleShadingEnable, float minSampleShading, const VkSampleMask* sampleMasks, bool alphaToCoverageEnable, bool alphaToOneEnable, VkPipelineMultisampleStateCreateInfo& multisampleStateCreateInfo)
{
	multisampleStateCreateInfo = {
		VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO, // VkStructureType                          sType
		nullptr,                                                  // const void                             * pNext
		0,                                                        // VkPipelineMultisampleStateCreateFlags    flags
		sampleCount,                                             // VkSampleCountFlagBits                    rasterizationSamples
		perSampleShadingEnable,                                  // VkBool32                                 sampleShadingEnable
		minSampleShading,                                        // float                                    minSampleShading
		sampleMasks,                                             // const VkSampleMask                     * pSampleMask
		alphaToCoverageEnable,                                   // VkBool32                                 alphaToCoverageEnable
		alphaToOneEnable                                         // VkBool32                                 alphaToOneEnable
	};
}

void VulkanLoader::specifyPipelineDepthAndStencilState(bool depthTestEnable, bool depthWriteEnable, VkCompareOp depthCompareOp, bool depthBoundsTestEnable, float minDepthBounds, float maxDepthBounds, bool stencilTestEnable, VkStencilOpState frontStencilTestParameters, VkStencilOpState backStencilTestParameters, VkPipelineDepthStencilStateCreateInfo& depthAndStencilStateCreateInfo)
{
	depthAndStencilStateCreateInfo = {
		VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO,   // VkStructureType                            sType
		nullptr,                                                      // const void                               * pNext
		0,                                                            // VkPipelineDepthStencilStateCreateFlags     flags
		depthTestEnable,                                              // VkBool32                                   depthTestEnable
		depthWriteEnable,                                             // VkBool32                                   depthWriteEnable
		depthCompareOp,                                               // VkCompareOp                                depthCompareOp
		depthBoundsTestEnable,                                        // VkBool32                                   depthBoundsTestEnable
		stencilTestEnable,                                            // VkBool32                                   stencilTestEnable
		frontStencilTestParameters,                                   // VkStencilOpState                           front
		backStencilTestParameters,                                    // VkStencilOpState                           back
		minDepthBounds,                                               // float                                      minDepthBounds
		maxDepthBounds                                                // float                                      maxDepthBounds
	};
}

void VulkanLoader::specifyPipelineBlendState(bool logicOpEnable, VkLogicOp logicOp, const std::vector<VkPipelineColorBlendAttachmentState>& attachmentBlendStates, const std::array<float, 4>& blendConstants, VkPipelineColorBlendStateCreateInfo& blendStateCreateInfo)
{
	blendStateCreateInfo = {
		VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO,   // VkStructureType                              sType
		nullptr,                                                    // const void                                 * pNext
		0,                                                          // VkPipelineColorBlendStateCreateFlags         flags
		logicOpEnable,                                              // VkBool32                                     logicOpEnable
		logicOp,                                                    // VkLogicOp                                    logicOp
		static_cast<uint32_t>(attachmentBlendStates.size()),        // uint32_t                                     attachmentCount
		attachmentBlendStates.data(),                               // const VkPipelineColorBlendAttachmentState  * pAttachments
		{                                                           // float                                        blendConstants[4]
			blendConstants[0],
			blendConstants[1],
			blendConstants[2],
			blendConstants[3]
		}
	};
}

void VulkanLoader::specifyPipelineDynamicStates(const std::vector<VkDynamicState>& dynamicStates, VkPipelineDynamicStateCreateInfo& dynamicStateCreateInfo)
{
	dynamicStateCreateInfo = {
		VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO,     // VkStructureType                      sType
		nullptr,                                                  // const void                         * pNext
		0,                                                        // VkPipelineDynamicStateCreateFlags    flags
		static_cast<uint32_t>(dynamicStates.size()),              // uint32_t                             dynamicStateCount
		dynamicStates.data()                                      // const VkDynamicState               * pDynamicStates
	};
}

bool VulkanLoader::createPipelineLayout(VkDevice logicalDevice, const std::vector<VkDescriptorSetLayout>& descriptorSetLayouts, const std::vector<VkPushConstantRange>& pushConstantRanges, VkPipelineLayout& pipelineLayout)
{
	VkPipelineLayoutCreateInfo pipelineLayoutCreateInfo = {
		VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO,          // VkStructureType                  sType
		nullptr,                                                // const void                     * pNext
		0,                                                      // VkPipelineLayoutCreateFlags      flags
		static_cast<uint32_t>(descriptorSetLayouts.size()),     // uint32_t                         setLayoutCount
		descriptorSetLayouts.data(),                            // const VkDescriptorSetLayout    * pSetLayouts
		static_cast<uint32_t>(pushConstantRanges.size()),       // uint32_t                         pushConstantRangeCount
		pushConstantRanges.data()                               // const VkPushConstantRange      * pPushConstantRanges
	};

	VkResult result = vkCreatePipelineLayout(logicalDevice, &pipelineLayoutCreateInfo, nullptr, &pipelineLayout);
	if (result != VK_SUCCESS)
	{
		// TODO : Use Numea System Log
		printf("Could not create pipeline layout\n");
		return false;
	}

	return true;
}

void VulkanLoader::specifyGraphicsPipelineCreationParameters(VkPipelineCreateFlags additionalOptions, const std::vector<VkPipelineShaderStageCreateInfo>& shaderStageCreateInfos, const VkPipelineVertexInputStateCreateInfo& vertexInputStateCreateInfo, const VkPipelineInputAssemblyStateCreateInfo& inputAssemblyStateCreateInfo, const VkPipelineTessellationStateCreateInfo* tessellationStateCreateInfo, const VkPipelineViewportStateCreateInfo* viewportStateCreateInfo, const VkPipelineRasterizationStateCreateInfo & rasterizationStateCreateInfo, const VkPipelineMultisampleStateCreateInfo* multisampleStateCreateInfo, const VkPipelineDepthStencilStateCreateInfo* depthAndStencilStateCreateInfo, const VkPipelineColorBlendStateCreateInfo* blendStateCreateInfo, const VkPipelineDynamicStateCreateInfo* dynamicStateCreateInfo, VkPipelineLayout pipelineLayout, VkRenderPass renderPass, uint32_t subpass, VkPipeline basePipelineHandle, int32_t basePipelineIndex, VkGraphicsPipelineCreateInfo& graphicsPipelineCreateInfo)
{
	graphicsPipelineCreateInfo = {
		VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO,            // VkStructureType                                sType
		nullptr,                                                    // const void                                   * pNext
		additionalOptions,                                          // VkPipelineCreateFlags                          flags
		static_cast<uint32_t>(shaderStageCreateInfos.size()),       // uint32_t                                       stageCount
		shaderStageCreateInfos.data(),                              // const VkPipelineShaderStageCreateInfo        * pStages
		&vertexInputStateCreateInfo,                                // const VkPipelineVertexInputStateCreateInfo   * pVertexInputState
		&inputAssemblyStateCreateInfo,                              // const VkPipelineInputAssemblyStateCreateInfo * pInputAssemblyState
		tessellationStateCreateInfo,                                // const VkPipelineTessellationStateCreateInfo  * pTessellationState
		viewportStateCreateInfo,                                    // const VkPipelineViewportStateCreateInfo      * pViewportState
		&rasterizationStateCreateInfo,                              // const VkPipelineRasterizationStateCreateInfo * pRasterizationState
		multisampleStateCreateInfo,                                 // const VkPipelineMultisampleStateCreateInfo   * pMultisampleState
		depthAndStencilStateCreateInfo,                             // const VkPipelineDepthStencilStateCreateInfo  * pDepthStencilState
		blendStateCreateInfo,                                       // const VkPipelineColorBlendStateCreateInfo    * pColorBlendState
		dynamicStateCreateInfo,                                     // const VkPipelineDynamicStateCreateInfo       * pDynamicState
		pipelineLayout,                                             // VkPipelineLayout                               layout
		renderPass,                                                 // VkRenderPass                                   renderPass
		subpass,                                                    // uint32_t                                       subpass
		basePipelineHandle,                                         // VkPipeline                                     basePipelineHandle
		basePipelineIndex                                           // int32_t                                        basePipelineIndex
	};
}

bool VulkanLoader::createPipelineCacheObject(VkDevice logicalDevice, const std::vector<unsigned char>& cacheData, VkPipelineCache& pipelineCache)
{
	VkPipelineCacheCreateInfo pipelineCacheCreateInfo = {
		VK_STRUCTURE_TYPE_PIPELINE_CACHE_CREATE_INFO,     // VkStructureType                sType
		nullptr,                                          // const void                   * pNext
		0,                                                // VkPipelineCacheCreateFlags     flags
		static_cast<uint32_t>(cacheData.size()),          // size_t                         initialDataSize
		cacheData.data()                                  // const void                   * pInitialData
	};

	VkResult result = vkCreatePipelineCache(logicalDevice, &pipelineCacheCreateInfo, nullptr, &pipelineCache);
	if (result != VK_SUCCESS)
	{
		// TODO : Use Numea System Log
		printf("Could not create pipeline cache\n");
		return false;
	}

	return true;
}

bool VulkanLoader::retrieveDataFromPipelineCache(VkDevice logicalDevice, VkPipelineCache pipelineCache, std::vector<unsigned char>& pipelineCacheData)
{
	size_t dataSize = 0;
	VkResult result = VK_SUCCESS;

	result = vkGetPipelineCacheData(logicalDevice, pipelineCache, &dataSize, nullptr);
	if ((result != VK_SUCCESS) || (dataSize == 0)) 
	{
		// TODO : Use Numea System Log
		printf("Could not get the size of the pipeline cache\n");
		return false;
	}

	pipelineCacheData.resize(dataSize);

	result = vkGetPipelineCacheData(logicalDevice, pipelineCache, &dataSize, pipelineCacheData.data());
	if ((result != VK_SUCCESS) || (dataSize == 0)) 
	{
		// TODO : Use Numea System Log
		printf("Could not acquire pipeline cache data\n");
		return false;
	}
	return true;
}

bool VulkanLoader::mergeMultiplePipelineCacheObjects(VkDevice logicalDevice, VkPipelineCache targetPipelineCache, const std::vector<VkPipelineCache>& sourcePipelineCaches)
{
	if (sourcePipelineCaches.size() > 0) 
	{
		VkResult result = vkMergePipelineCaches(logicalDevice, targetPipelineCache, static_cast<uint32_t>(sourcePipelineCaches.size()), sourcePipelineCaches.data());
		if (result != VK_SUCCESS)
		{
			// TODO : Use Numea System Log
			printf("Could not merge pipeline cache objects\n");
			return false;
		}
		return true;
	}
	return false;
}

bool VulkanLoader::createGraphicsPipelines(VkDevice logicalDevice, const std::vector<VkGraphicsPipelineCreateInfo>& graphicsPipelineCreateInfos, VkPipelineCache pipelineCache, std::vector<VkPipeline>& graphicsPipelines)
{
	if (graphicsPipelineCreateInfos.size() > 0) 
	{
		graphicsPipelines.resize(graphicsPipelineCreateInfos.size());
		VkResult result = vkCreateGraphicsPipelines(logicalDevice, pipelineCache, static_cast<uint32_t>(graphicsPipelineCreateInfos.size()), graphicsPipelineCreateInfos.data(), nullptr, graphicsPipelines.data());
		if (result != VK_SUCCESS)
		{
			// TODO : Use Numea System Log
			printf("Could not create a graphics pipeline\n");
			return false;
		}
		return true;
	}
	return false;
}

bool VulkanLoader::createComputePipeline(VkDevice logicalDevice, VkPipelineCreateFlags additionalOptions, const VkPipelineShaderStageCreateInfo& computeShaderStage, VkPipelineLayout pipelineLayout, VkPipeline basePipelineHandle, VkPipelineCache pipelineCache, VkPipeline& computePipeline)
{
	VkComputePipelineCreateInfo computePipelineCreateInfo = {
		VK_STRUCTURE_TYPE_COMPUTE_PIPELINE_CREATE_INFO,   // VkStructureType                    sType
		nullptr,                                          // const void                       * pNext
		additionalOptions,                                // VkPipelineCreateFlags              flags
		computeShaderStage,                               // VkPipelineShaderStageCreateInfo    stage
		pipelineLayout,                                   // VkPipelineLayout                   layout
		basePipelineHandle,                               // VkPipeline                         basePipelineHandle
		-1                                                // int32_t                            basePipelineIndex
	};

	VkResult result = vkCreateComputePipelines(logicalDevice, pipelineCache, 1, &computePipelineCreateInfo, nullptr, &computePipeline);
	if (result != VK_SUCCESS)
	{
		// TODO : Use Numea System Log
		printf("Could not create compute pipeline\n");
		return false;
	}
	return true;
}

void VulkanLoader::bindPipelineObject(VkCommandBuffer commandBuffer, VkPipelineBindPoint pipelineType, VkPipeline pipeline)
{
	vkCmdBindPipeline(commandBuffer, pipelineType, pipeline);
}

bool VulkanLoader::createPipelineLayoutWithCombinedImageSamplerBufferAndPushConstantRanges(VkDevice logicalDevice, const std::vector<VkPushConstantRange>& pushConstantRanges, VkDescriptorSetLayout& descriptorSetLayout, VkPipelineLayout& pipelineLayout)
{
	std::vector<VkDescriptorSetLayoutBinding> descriptorSetLayoutBindings = {
		{
			0,                                  // uint32_t               binding
			VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE,   // VkDescriptorType       descriptorType
			1,                                  // uint32_t               descriptorCount
			VK_SHADER_STAGE_FRAGMENT_BIT,       // VkShaderStageFlags     stageFlags
			nullptr                             // const VkSampler      * pImmutableSamplers
		},
		{
			1,                                  // uint32_t               binding
			VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,  // VkDescriptorType       descriptorType
			1,                                  // uint32_t               descriptorCount
			VK_SHADER_STAGE_VERTEX_BIT,         // VkShaderStageFlags     stageFlags
			nullptr                             // const VkSampler      * pImmutableSamplers
		}
	};

	if (!createDescriptorSetLayout(logicalDevice, descriptorSetLayoutBindings, descriptorSetLayout)) 
	{
		return false;
	}

	if (!createPipelineLayout(logicalDevice, { descriptorSetLayout }, pushConstantRanges, pipelineLayout)) 
	{
		return false;
	}

	return true;
}

bool VulkanLoader::createGraphicsPipelineWithVertexAndFragmentShadersDepthTestEnabledAndWithDynamicViewportAndScissorTests(VkDevice logicalDevice, VkPipelineCreateFlags additionalOptions, const std::string& vertexShaderFilename, const std::string& fragmentShaderFilename, const std::vector<VkVertexInputBindingDescription>& vertexInputBindingDescriptions, const std::vector<VkVertexInputAttributeDescription>& vertexAttributeDescriptions, VkPrimitiveTopology primitiveTopology, bool primitiveRestartEnable, VkPolygonMode polygonMode, VkCullModeFlags cullingMode, VkFrontFace frontFace, bool logicOpEnable, VkLogicOp logicOp, const std::vector<VkPipelineColorBlendAttachmentState>& attachmentBlendStates, const std::array<float, 4>& blendConstants, VkPipelineLayout pipelineLayout, VkRenderPass renderPass, uint32_t subpass, VkPipeline basePipelineHandle, VkPipelineCache pipelineCache, std::vector<VkPipeline>& graphicsPipeline)
{
	std::vector<unsigned char> vertexShaderSpirv;
	if (!loadBinaryFile(vertexShaderFilename, vertexShaderSpirv)) 
	{
		return false;
	}

	VkSmartShaderModule vertexShaderModule(logicalDevice);
	if (!createShaderModule(logicalDevice, vertexShaderSpirv, vertexShaderModule.getHandle())) 
	{
		return false;
	}

	std::vector<unsigned char> fragmentShaderSpirv;
	if (!loadBinaryFile(fragmentShaderFilename, fragmentShaderSpirv)) 
	{
		return false;
	}

	VkSmartShaderModule fragmentShaderModule(logicalDevice);
	if (!createShaderModule(logicalDevice, fragmentShaderSpirv, fragmentShaderModule.getHandle())) 
	{
		return false;
	}

	std::vector<ShaderStageParameters> shaderStageParams = {
		{
			VK_SHADER_STAGE_VERTEX_BIT,       // VkShaderStageFlagBits        shaderStage
			vertexShaderModule.getHandle(),   // VkShaderModule               shaderModule
			"main",                           // char const                 * entryPointName
			nullptr                           // VkSpecializationInfo const * specializationInfo
		},
		{
			VK_SHADER_STAGE_FRAGMENT_BIT,     // VkShaderStageFlagBits        shaderStage
			fragmentShaderModule.getHandle(), // VkShaderModule               shaderModule
			"main",                           // char const                 * entryPointName
			nullptr                           // VkSpecializationInfo const * specializationInfo
		}
	};

	std::vector<VkPipelineShaderStageCreateInfo> shaderStageCreateInfos;
	specifyPipelineShaderStages(shaderStageParams, shaderStageCreateInfos);

	VkPipelineVertexInputStateCreateInfo vertexInputStateCreateInfo;
	specifyPipelineVertexInputState(vertexInputBindingDescriptions, vertexAttributeDescriptions, vertexInputStateCreateInfo);

	VkPipelineInputAssemblyStateCreateInfo inputAssemblyStateCreateInfo;
	specifyPipelineInputAssemblyState(primitiveTopology, primitiveRestartEnable, inputAssemblyStateCreateInfo);

	ViewportInfo viewportInfos = {
		{                     // std::vector<VkViewport>   Viewports
			{
				0.0f,               // float          x
				0.0f,               // float          y
				500.0f,             // float          width
				500.0f,             // float          height
				0.0f,               // float          minDepth
				1.0f                // float          maxDepth
			}
		},
		{                     // std::vector<VkRect2D>     Scissors
			{
				{                   // VkOffset2D     offset
					0,                  // int32_t        x
					0                   // int32_t        y
				},
				{                   // VkExtent2D     extent
					500,                // uint32_t       width
					500                 // uint32_t       height
				}
			}
		}
	};

	VkPipelineViewportStateCreateInfo viewportStateCreateInfo;
	specifyPipelineViewportAndScissorTestState(viewportInfos, viewportStateCreateInfo);

	VkPipelineRasterizationStateCreateInfo rasterizationStateCreateInfo;
	specifyPipelineRasterizationState(false, false, polygonMode, cullingMode, frontFace, false, 0.0f, 1.0f, 0.0f, 1.0f, rasterizationStateCreateInfo);

	VkPipelineMultisampleStateCreateInfo multisampleStateCreateInfo;
	specifyPipelineMultisampleState(VK_SAMPLE_COUNT_1_BIT, false, 0.0f, nullptr, false, false, multisampleStateCreateInfo);

	VkStencilOpState stencilTestParameters = {
		VK_STENCIL_OP_KEEP,   // VkStencilOp    failOp
		VK_STENCIL_OP_KEEP,   // VkStencilOp    passOp
		VK_STENCIL_OP_KEEP,   // VkStencilOp    depthFailOp
		VK_COMPARE_OP_ALWAYS, // VkCompareOp    compareOp
		0,                    // uint32_t       compareMask
		0,                    // uint32_t       writeMask
		0                     // uint32_t       reference
	};

	VkPipelineDepthStencilStateCreateInfo depthAndStencilStateCreateInfo;
	specifyPipelineDepthAndStencilState(true, true, VK_COMPARE_OP_LESS_OR_EQUAL, false, 0.0f, 1.0f, false, stencilTestParameters, stencilTestParameters, depthAndStencilStateCreateInfo);

	VkPipelineColorBlendStateCreateInfo blendStateCreateInfo;
	specifyPipelineBlendState(logicOpEnable, logicOp, attachmentBlendStates, blendConstants, blendStateCreateInfo);

	std::vector<VkDynamicState> dynamicStates = {
		VK_DYNAMIC_STATE_VIEWPORT,
		VK_DYNAMIC_STATE_SCISSOR
	};

	VkPipelineDynamicStateCreateInfo dynamicStateCreateInfo;
	specifyPipelineDynamicStates(dynamicStates, dynamicStateCreateInfo);

	VkGraphicsPipelineCreateInfo graphicsPipelineCreateInfo;
	specifyGraphicsPipelineCreationParameters(additionalOptions, shaderStageCreateInfos, vertexInputStateCreateInfo, inputAssemblyStateCreateInfo, nullptr, &viewportStateCreateInfo,
		rasterizationStateCreateInfo, &multisampleStateCreateInfo, &depthAndStencilStateCreateInfo, &blendStateCreateInfo, &dynamicStateCreateInfo, pipelineLayout, renderPass,
		subpass, basePipelineHandle, -1, graphicsPipelineCreateInfo);

	if (!createGraphicsPipelines(logicalDevice, { graphicsPipelineCreateInfo }, pipelineCache, graphicsPipeline)) 
	{
		return false;
	}

	return true;
}

bool VulkanLoader::createMultipleGraphicsPipelineOnMultipleThreads(VkDevice logicalDevice, const std::string& pipelineCacheFilename, const std::vector<std::vector<VkGraphicsPipelineCreateInfo>>& graphicsPipelinesCreateInfos, std::vector<std::vector<VkPipeline>>& graphicsPipelines)
{
	// Load cache from file (if available)
	std::vector<unsigned char> cacheData;
	loadBinaryFile(pipelineCacheFilename, cacheData);

	// Create cache for each thread, initialize its contents with data loaded from file
	std::vector<VkSmartPipelineCache> pipelineCaches(graphicsPipelinesCreateInfos.size());
	for (size_t i = 0; i < graphicsPipelinesCreateInfos.size(); i++) 
	{
		pipelineCaches[i] = VkSmartPipelineCache(logicalDevice);
		if (!createPipelineCacheObject(logicalDevice, cacheData, pipelineCaches[i].getHandle())) 
		{
			return false;
		}
	}

	// Create multiple threads, where each thread creates multiple pipelines using its own cache
	std::vector<std::thread> threads(graphicsPipelinesCreateInfos.size());
	for (size_t i = 0; i < graphicsPipelinesCreateInfos.size(); i++) 
	{
		graphicsPipelines[i].resize(graphicsPipelinesCreateInfos[i].size());
		threads[i] = std::thread(createGraphicsPipelines, logicalDevice, std::ref(graphicsPipelinesCreateInfos[i]), pipelineCaches[i].getHandle(), std::ref(graphicsPipelines[i]));
	}

	// Wait for all threads to finish
	for (size_t i = 0; i < graphicsPipelinesCreateInfos.size(); ++i) 
	{
		threads[i].join();
	}

	// Merge all the caches into one, retrieve its contents and store them in the file
	VkPipelineCache targetCache = pipelineCaches.back();
	std::vector<VkPipelineCache> sourceCaches(pipelineCaches.size() - 1);
	for (size_t i = 0; i < pipelineCaches.size() - 1; ++i) 
	{
		sourceCaches[i] = pipelineCaches[i];
	}

	if (!mergeMultiplePipelineCacheObjects(logicalDevice, targetCache, sourceCaches)) 
	{
		return false;
	}

	if (!retrieveDataFromPipelineCache(logicalDevice, targetCache, cacheData)) 
	{
		return false;
	}

	// TODO : TODO ? This was already by the author
	//if( !SaveBinaryFile( pipeline_cache_filename, cache_data ) ) {
	//  return false;
	//}

	return true;
}

void VulkanLoader::destroyPipeline(VkDevice logicalDevice, VkPipeline& pipeline)
{
	if (pipeline != VK_NULL_HANDLE)
	{
		vkDestroyPipeline(logicalDevice, pipeline, nullptr);
		pipeline = VK_NULL_HANDLE;
	}
}

void VulkanLoader::destroyPipelineCache(VkDevice logicalDevice, VkPipelineCache& pipelineCache)
{
	if (pipelineCache != VK_NULL_HANDLE)
	{
		vkDestroyPipelineCache(logicalDevice, pipelineCache, nullptr);
		pipelineCache = VK_NULL_HANDLE;
	}
}

void VulkanLoader::destroyPipelineLayout(VkDevice logicalDevice, VkPipelineLayout& pipelineLayout)
{
	if (pipelineLayout != VK_NULL_HANDLE)
	{
		vkDestroyPipelineLayout(logicalDevice, pipelineLayout, nullptr);
		pipelineLayout = VK_NULL_HANDLE;
	}
}

void VulkanLoader::destroyShaderModule(VkDevice logicalDevice, VkShaderModule& shaderModule)
{
	if (shaderModule != VK_NULL_HANDLE)
	{
		vkDestroyShaderModule(logicalDevice, shaderModule, nullptr);
		shaderModule = VK_NULL_HANDLE;
	}
}

void VulkanLoader::clearColorImage(VkCommandBuffer commandBuffer, VkImage image, VkImageLayout imageLayout, const std::vector<VkImageSubresourceRange>& imageSubresourceRanges, VkClearColorValue& clearColor)
{
	vkCmdClearColorImage(commandBuffer, image, imageLayout, &clearColor, static_cast<uint32_t>(imageSubresourceRanges.size()), imageSubresourceRanges.data());
}

void VulkanLoader::clearDepthStencilImage(VkCommandBuffer commandBuffer, VkImage image, VkImageLayout imageLayout, const std::vector<VkImageSubresourceRange>& imageSubresourceRanges, VkClearDepthStencilValue& clearValue)
{
	vkCmdClearDepthStencilImage(commandBuffer, image, imageLayout, &clearValue, static_cast<uint32_t>(imageSubresourceRanges.size()), imageSubresourceRanges.data());
}

void VulkanLoader::clearRenderPassAttachments(VkCommandBuffer commandBuffer, const std::vector<VkClearAttachment>& attachments, const std::vector<VkClearRect>& rects)
{
	vkCmdClearAttachments(commandBuffer, static_cast<uint32_t>(attachments.size()), attachments.data(), static_cast<uint32_t>(rects.size()), rects.data());
}

void VulkanLoader::bindVertexBuffers(VkCommandBuffer commandBuffer, uint32_t firstBinding, const std::vector<VertexBufferParameters>& buffersParameters)
{
	if (buffersParameters.size() > 0) 
	{
		std::vector<VkBuffer> buffers;
		std::vector<VkDeviceSize> offsets;
		for (auto& bufferParameters : buffersParameters) 
		{
			buffers.push_back(bufferParameters.buffer);
			offsets.push_back(bufferParameters.memoryOffset);
		}
		vkCmdBindVertexBuffers(commandBuffer, firstBinding, static_cast<uint32_t>(buffersParameters.size()), buffers.data(), offsets.data());
	}
}

void VulkanLoader::bindIndexBuffer(VkCommandBuffer commandBuffer, VkBuffer buffer, VkDeviceSize memoryOffset, VkIndexType indexType)
{
	vkCmdBindIndexBuffer(commandBuffer, buffer, memoryOffset, indexType);
}

void VulkanLoader::provideDataToShadersThroughPushConstants(VkCommandBuffer commandBuffer, VkPipelineLayout pipelineLayout, VkShaderStageFlags pipelineStages, uint32_t offset, uint32_t size, void* data)
{
	vkCmdPushConstants(commandBuffer, pipelineLayout, pipelineStages, offset, size, data);
}

void VulkanLoader::setViewportStateDynamically(VkCommandBuffer commandBuffer, uint32_t firstViewport, const std::vector<VkViewport>& viewports)
{
	vkCmdSetViewport(commandBuffer, firstViewport, static_cast<uint32_t>(viewports.size()), viewports.data());
}

void VulkanLoader::setScissorStateDynamically(VkCommandBuffer commandBuffer, uint32_t firstScissor, const std::vector<VkRect2D>& scissors)
{
	vkCmdSetScissor(commandBuffer, firstScissor, static_cast<uint32_t>(scissors.size()), scissors.data());
}

void VulkanLoader::setLineWidthStateDynamically(VkCommandBuffer commandBuffer, float lineWidth)
{
	vkCmdSetLineWidth(commandBuffer, lineWidth);
}

void VulkanLoader::setDepthBiasStateDynamically(VkCommandBuffer commandBuffer, float constantFactor, float clampValue, float slopeFactor)
{
	vkCmdSetDepthBias(commandBuffer, constantFactor, clampValue, slopeFactor);
}

void VulkanLoader::setBlendConstantsStateDynamically(VkCommandBuffer commandBuffer, const std::array<float, 4>& blendConstants)
{
	vkCmdSetBlendConstants(commandBuffer, blendConstants.data());
}

void VulkanLoader::drawGeometry(VkCommandBuffer commandBuffer, uint32_t vertexCount, uint32_t instanceCount, uint32_t firstVertex, uint32_t firstInstance)
{
	vkCmdDraw(commandBuffer, vertexCount, instanceCount, firstVertex, firstInstance);
}

void VulkanLoader::drawIndexedGeometry(VkCommandBuffer commandBuffer, uint32_t indexCount, uint32_t instanceCount, uint32_t firstIndex, uint32_t vertexOffset, uint32_t firstInstance)
{
	vkCmdDrawIndexed(commandBuffer, indexCount, instanceCount, firstIndex, vertexOffset, firstInstance);
}

void VulkanLoader::dispatchComputeWork(VkCommandBuffer commandBuffer, uint32_t xSize, uint32_t ySize, uint32_t zSize)
{
	vkCmdDispatch(commandBuffer, xSize, ySize, zSize);
}

void VulkanLoader::executeSecondaryCommandBufferInsidePrimaryCommandBuffer(VkCommandBuffer primaryCommandBuffer, const std::vector<VkCommandBuffer>& secondaryCommandBuffers)
{
	if (secondaryCommandBuffers.size() > 0) 
	{
		vkCmdExecuteCommands(primaryCommandBuffer, static_cast<uint32_t>(secondaryCommandBuffers.size()), secondaryCommandBuffers.data());
	}
}

bool VulkanLoader::recordCommandBufferThatDrawGeometryWithDynamicViewportAndScissorStates(VkCommandBuffer commandBuffer, VkImage swapchainImage, uint32_t presentQueueFamilyIndex, uint32_t graphicsQueueFamilyIndex, VkRenderPass renderPass, VkFramebuffer framebuffer, VkExtent2D framebufferSize, const std::vector<VkClearValue>& clearValues, VkPipeline graphicsPipeline, uint32_t firstVertexBufferBinding, const std::vector<VertexBufferParameters>& vertexBuffersParameters, VkPipelineLayout pipelineLayout, const std::vector<VkDescriptorSet>& descriptorSets, uint32_t indexForFirstDescriptorSet, const Mesh& geometry, uint32_t instanceCount, uint32_t firstInstance)
{
	if (!beginCommandBufferRecordingOperation(commandBuffer, VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT, nullptr)) 
	{
		return false;
	}

	if (presentQueueFamilyIndex != graphicsQueueFamilyIndex) 
	{
		ImageTransition imageTransitionBeforeDrawing = {
			swapchainImage,                           // VkImage              image
			VK_ACCESS_MEMORY_READ_BIT,                // VkAccessFlags        currentAccess
			VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT,     // VkAccessFlags        newAccess
			VK_IMAGE_LAYOUT_PRESENT_SRC_KHR,          // VkImageLayout        currentLayout
			VK_IMAGE_LAYOUT_PRESENT_SRC_KHR,          // VkImageLayout        newLayout
			presentQueueFamilyIndex,                  // uint32_t             currentQueueFamily
			graphicsQueueFamilyIndex,                 // uint32_t             newQueueFamily
			VK_IMAGE_ASPECT_COLOR_BIT                 // VkImageAspectFlags   aspect
		};

		setImageMemoryBarrier(commandBuffer, VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT, VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT, { imageTransitionBeforeDrawing });
	}

	beginRenderPass(commandBuffer, renderPass, framebuffer, { { 0, 0 }, framebufferSize }, clearValues, VK_SUBPASS_CONTENTS_INLINE);

	bindPipelineObject(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, graphicsPipeline);

	VkViewport viewport = {
		0.0f,                                         // float          x
		0.0f,                                         // float          y
		static_cast<float>(framebufferSize.width),    // float          width
		static_cast<float>(framebufferSize.height),   // float          height
		0.0f,                                         // float          minDepth
		1.0f,                                         // float          maxDepth
	};
	setViewportStateDynamically(commandBuffer, 0, { viewport });

	VkRect2D scissor = {
		{                                             // VkOffset2D     offset
			0,                                            // int32_t        x
			0                                             // int32_t        y
		},
		{                                             // VkExtent2D     extent
			framebufferSize.width,                         // uint32_t       width
			framebufferSize.height                         // uint32_t       height
		}
	};
	setScissorStateDynamically(commandBuffer, 0, { scissor });

	bindVertexBuffers(commandBuffer, firstVertexBufferBinding, vertexBuffersParameters);

	if (descriptorSets.size() > 0) 
	{
		bindDescriptorSets(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, pipelineLayout, indexForFirstDescriptorSet, descriptorSets, {});
	}

	for (size_t i = 0; i < geometry.parts.size(); ++i)
	{
		drawGeometry(commandBuffer, geometry.parts[i].vertexCount, instanceCount, geometry.parts[i].vertexOffset, firstInstance);
	}

	endRenderPass(commandBuffer);

	if (presentQueueFamilyIndex != graphicsQueueFamilyIndex) 
	{
		ImageTransition imageTransitionBeforePresent = {
			swapchainImage,                           // VkImage              Image
			VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT,     // VkAccessFlags        CurrentAccess
			VK_ACCESS_MEMORY_READ_BIT,                // VkAccessFlags        NewAccess
			VK_IMAGE_LAYOUT_PRESENT_SRC_KHR,          // VkImageLayout        CurrentLayout
			VK_IMAGE_LAYOUT_PRESENT_SRC_KHR,          // VkImageLayout        NewLayout
			graphicsQueueFamilyIndex,                 // uint32_t             CurrentQueueFamily
			presentQueueFamilyIndex,                  // uint32_t             NewQueueFamily
			VK_IMAGE_ASPECT_COLOR_BIT                 // VkImageAspectFlags   Aspect
		};

		setImageMemoryBarrier(commandBuffer, VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT, VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT, { imageTransitionBeforePresent });
	}

	if (!endCommandBufferRecordingOperation(commandBuffer)) 
	{
		return false;
	}

	return true;
}

bool VulkanLoader::recordCommandBuffersOnMultipleThreads(const std::vector<CommandBufferRecordingThreadParameters>& threadsParameters, VkQueue queue, std::vector<WaitSemaphoreInfo> waitSemaphoreInfos, std::vector<VkSemaphore> signalSemaphores, VkFence fence)
{
	std::vector<std::thread> threads(threadsParameters.size());
	for (size_t i = 0; i < threadsParameters.size(); i++) 
	{
		threads[i] = std::thread(threadsParameters[i].recordingFunction, threadsParameters[i].commandBuffer);
	}

	std::vector<VkCommandBuffer> commandBuffers(threadsParameters.size());
	for (size_t i = 0; i < threadsParameters.size(); i++) 
	{
		threads[i].join();
		commandBuffers[i] = threadsParameters[i].commandBuffer;
	}

	if (!submitCommandBuffersToQueue(queue, waitSemaphoreInfos, commandBuffers, signalSemaphores, fence)) 
	{
		return false;
	}

	return true;
}

bool VulkanLoader::prepareSingleFrameOfAnimation(VkDevice logicalDevice, VkQueue graphicsQueue, VkQueue presentQueue, VkSwapchainKHR swapchain, VkExtent2D swapchainSize, const std::vector<VkImageView>& swapchainImageViews, VkImageView depthAttachment, const std::vector<WaitSemaphoreInfo>& waitInfos, VkSemaphore imageAcquiredSemaphore, VkSemaphore readyToPresentSemaphore, VkFence finishedDrawingFence, std::function<bool(VkCommandBuffer, uint32_t, VkFramebuffer)> recordCommandBuffer, VkCommandBuffer commandBuffer, VkRenderPass renderPass, VkSmartFramebuffer& framebuffer)
{
	uint32_t imageIndex;
	if (!acquireSwapchainImage(logicalDevice, swapchain, imageAcquiredSemaphore, VK_NULL_HANDLE, imageIndex)) 
	{
		return false;
	}

	std::vector<VkImageView> attachments = { swapchainImageViews[imageIndex] };
	if (depthAttachment != VK_NULL_HANDLE) 
	{
		attachments.push_back(depthAttachment);
	}

	if (!createFramebuffer(logicalDevice, renderPass, attachments, swapchainSize.width, swapchainSize.height, 1, framebuffer.getHandle())) 
	{
		return false;
	}

	if (!recordCommandBuffer(commandBuffer, imageIndex, framebuffer.getHandle()))
	{
		return false;
	}

	std::vector<WaitSemaphoreInfo> waitSemaphoreInfos = waitInfos;
	waitSemaphoreInfos.push_back({
		imageAcquiredSemaphore,                       // VkSemaphore            semaphore
		VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT // VkPipelineStageFlags   waitingStage
	});

	if (!submitCommandBuffersToQueue(graphicsQueue, waitSemaphoreInfos, { commandBuffer }, { readyToPresentSemaphore }, finishedDrawingFence)) 
	{
		return false;
	}

	PresentInfo presentInfo = {
		swapchain,                                    // VkSwapchainKHR         swapchain
		imageIndex                                    // uint32_t               imageIndex
	};

	if (!presentImage(presentQueue, { readyToPresentSemaphore }, { presentInfo })) 
	{
		return false;
	}

	return true;
}

bool VulkanLoader::increasePerformanceThroughIncreasingTheNumberOfSeparatelyRenderedFrames(VkDevice logicalDevice, VkQueue graphicsQueue, VkQueue presentQueue, VkSwapchainKHR swapchain, VkExtent2D swapchainSize, const std::vector<VkImageView>& swapchainImageViews, VkRenderPass renderPass, const std::vector<WaitSemaphoreInfo>& waitInfos, std::function<bool(VkCommandBuffer, uint32_t, VkFramebuffer)> recordCommandBuffer, std::vector<FrameResources>& frameResources)
{
	static uint32_t frameIndex = 0;
	FrameResources& currentFrame = frameResources[frameIndex];

	if (!waitForFences(logicalDevice, { currentFrame.drawingFinishedFence.getHandle() }, false, 2000000000))
	{
		return false;
	}

	if (!resetFences(logicalDevice, { currentFrame.drawingFinishedFence.getHandle() }))
	{
		return false;
	}

	currentFrame.framebuffer = VkSmartFramebuffer(logicalDevice);

	if (!prepareSingleFrameOfAnimation(logicalDevice, graphicsQueue, presentQueue, swapchain, swapchainSize, swapchainImageViews,
		currentFrame.depthAttachment.getHandle(), waitInfos, currentFrame.imageAcquiredSemaphore.getHandle(), currentFrame.readyToPresentSemaphore.getHandle(),
		currentFrame.drawingFinishedFence.getHandle(), recordCommandBuffer, currentFrame.commandBuffer, renderPass, currentFrame.framebuffer))
	{
		return false;
	}

	frameIndex = (frameIndex + 1) % frameResources.size();

	return true;
}

bool VulkanLoader::isExtensionSupported(const std::vector<VkExtensionProperties>& availableExtensions, char const * const extension)
{
	for (auto& availableExtension : availableExtensions)
	{
		if (strstr(availableExtension.extensionName, extension))
		{
			return true;
		}
	}
	return false;
}

const char* VulkanLoader::toErrorString(VkResult result)
{
	#define CASE(name) case name##: return #name; break;

	switch (result)
	{
		CASE(VK_SUCCESS);
		CASE(VK_NOT_READY);
		CASE(VK_TIMEOUT);
		CASE(VK_EVENT_SET);
		CASE(VK_EVENT_RESET);
		CASE(VK_INCOMPLETE);
		CASE(VK_ERROR_OUT_OF_HOST_MEMORY);
		CASE(VK_ERROR_OUT_OF_DEVICE_MEMORY);
		CASE(VK_ERROR_INITIALIZATION_FAILED);
		CASE(VK_ERROR_DEVICE_LOST);
		CASE(VK_ERROR_MEMORY_MAP_FAILED);
		CASE(VK_ERROR_LAYER_NOT_PRESENT);
		CASE(VK_ERROR_EXTENSION_NOT_PRESENT);
		CASE(VK_ERROR_FEATURE_NOT_PRESENT);
		CASE(VK_ERROR_INCOMPATIBLE_DRIVER);
		CASE(VK_ERROR_TOO_MANY_OBJECTS);
		CASE(VK_ERROR_FORMAT_NOT_SUPPORTED);
		CASE(VK_ERROR_FRAGMENTED_POOL);
		//CASE(VK_ERROR_OUT_OF_POOL_MEMORY);
		//CASE(VK_ERROR_INVALID_EXTERNAL_HANDLE);
		CASE(VK_ERROR_SURFACE_LOST_KHR);
		CASE(VK_ERROR_NATIVE_WINDOW_IN_USE_KHR);
		CASE(VK_SUBOPTIMAL_KHR);
		CASE(VK_ERROR_OUT_OF_DATE_KHR);
		CASE(VK_ERROR_INCOMPATIBLE_DISPLAY_KHR);
		CASE(VK_ERROR_VALIDATION_FAILED_EXT);
		CASE(VK_ERROR_INVALID_SHADER_NV);
		//CASE(VK_ERROR_FRAGMENTATION_EXT);
		//CASE(VK_ERROR_NOT_PERMITTED_EXT);
		//CASE(VK_ERROR_OUT_OF_POOL_MEMORY_KHR);
		//CASE(VK_ERROR_INVALID_EXTERNAL_HANDLE_KHR);

		default: return std::to_string(result).c_str(); break;
	}

	#undef CASE

	return nullptr;
}

bool VulkanLoader::loadBinaryFile(const std::string& filename, std::vector<unsigned char>& contents)
{
	contents.clear();

	std::ifstream file(filename, std::ios::binary);
	if (file.fail()) 
	{
		// TODO : Use Numea System Log
		printf("Could not open '%s' file\n", filename.c_str());
		return false;
	}

	std::streampos begin;
	std::streampos end;
	begin = file.tellg();
	file.seekg(0, std::ios::end);
	end = file.tellg();

	if ((end - begin) == 0) 
	{
		// TODO : Use Numea System Log
		printf("The '%s' file is empty\n", filename.c_str());
		return false;
	}

	contents.resize(static_cast<size_t>(end - begin));
	file.seekg(0, std::ios::beg);
	file.read(reinterpret_cast<char*>(contents.data()), end - begin);
	file.close();

	return true;
}

void VulkanLoader::describePhysicalDevice(VkPhysicalDevice physicalDevice)
{
	std::vector<VkExtensionProperties> availableDeviceExtensions;
	queryAvailableDeviceExtensions(physicalDevice, availableDeviceExtensions);

	VkPhysicalDeviceFeatures physicalDeviceFeatures;
	VkPhysicalDeviceProperties physicalDeviceProperties;
	queryFeaturesAndPropertiesOfPhysicalDevice(physicalDevice, physicalDeviceFeatures, physicalDeviceProperties);

	std::vector<VkQueueFamilyProperties> availableQueueFamilies;
	queryAvailableQueueFamiliesAndTheirProperties(physicalDevice, availableQueueFamilies);

	printf("\t%s : \n", physicalDeviceProperties.deviceName);

	printf("\t\tAvailable Device Extensions : \n");
	for (auto& extension : availableDeviceExtensions)
	{
		printf("\t\t\t%s\n", extension.extensionName);
	}

	printf("\t\tAvailable Queue Families : \n");
	for (auto& queueFamily : availableQueueFamilies)
	{
		printf("\t\t\tType : %d, Count : %d\n", queueFamily.queueFlags, queueFamily.queueCount);
	}
}

bool VulkanLoader::areFeaturesSupported(VkPhysicalDeviceFeatures* deviceFeatures, VkPhysicalDeviceFeatures* desiredFeatures)
{
	// TODO : assert(deviceFeatures != nullptr);

	if (desiredFeatures == nullptr)
	{
		return true;
	}

	// Geometry shader
	if (desiredFeatures->geometryShader == VK_TRUE && deviceFeatures->geometryShader == VK_FALSE)
	{
		return false;
	}

	// TODO : Tests more features
	return true;
}

bool VulkanLoader::createDevices(const std::vector<const char*>& desiredExtensions, VkPhysicalDeviceFeatures* desiredFeatures, VkPhysicalDevice& physicalDevice, VkDevice& device)
{
	// TODO : Lot of things to do inside here

	std::vector<VkPhysicalDevice> availablePhysicalDevices;
	//if (!queryAvailablePhysicalDevices(availablePhysicalDevices))
	//{
	//	return false;
	//}

	for (auto& physicalDevice : availablePhysicalDevices)
	{
		std::vector<VkQueueFamilyProperties> queueFamilies;
		if (!queryAvailableQueueFamiliesAndTheirProperties(physicalDevice, queueFamilies))
		{
			return false;
		}

		VkPhysicalDeviceFeatures deviceFeatures;
		VkPhysicalDeviceProperties deviceProperties;
		queryFeaturesAndPropertiesOfPhysicalDevice(physicalDevice, deviceFeatures, deviceProperties);

		if (!areFeaturesSupported(&deviceFeatures, desiredFeatures))
		{
			continue;
		}

		uint32_t graphicsQueueFamilyIndex;
		if (!selectQueueFamilyWithDesiredCapabilities(physicalDevice, queueFamilies, VK_QUEUE_GRAPHICS_BIT, graphicsQueueFamilyIndex))
		{
			continue;
		}

		uint32_t computeQueueFamilyIndex;
		if (!selectQueueFamilyWithDesiredCapabilities(physicalDevice, queueFamilies, VK_QUEUE_COMPUTE_BIT, computeQueueFamilyIndex))
		{
			continue;
		}

		// TODO : Allow using multiple surface
		uint32_t presentQueueFamilyIndex;
		//if (!selectQueueFamilyThatSupportsPresentationToSurface(physicalDevice, queueFamilies, presentationSurface, presentQueueFamilyIndex))
		{
			continue;
		}

		std::vector<QueueInfo> requestedQueues = { { graphicsQueueFamilyIndex,{ 1.0f } } };
		if (graphicsQueueFamilyIndex != computeQueueFamilyIndex)
		{
			requestedQueues.push_back({ computeQueueFamilyIndex,{ 1.0f } });
		}
		if ((graphicsQueueFamilyIndex != presentQueueFamilyIndex) && (computeQueueFamilyIndex != presentQueueFamilyIndex))
		{
			requestedQueues.push_back({ presentQueueFamilyIndex,{ 1.0f } });
		}

		//if (!createLogicalDevice(physicalDevice, requestedQueues, desiredExtensions, &deviceFeatures))
		{
			printf("d\n");
			continue;
		}
		//else
		{
			physicalDevice = physicalDevice;

			/*
			if (!loadDeviceLevelFunctions(desiredExtensions))
			{
				return false;
			}

			getDeviceQueue(graphicsQueueFamilyIndex, 0, sGraphicsQueue);
			getDeviceQueue(computeQueueFamilyIndex, 0, sComputeQueue);
			*/

			return true;
		}
	}

	// TODO : Use Numea System Log
	printf("Can't find one valid physical device\n");
	return false;
}

} // namespace nu
