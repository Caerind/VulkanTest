#include "VulkanInstance.hpp"

#include <functional>

#include "VulkanLoader.hpp"
#include "VulkanPhysicalDevice.hpp"
#include "VulkanSurface.hpp"

VULKAN_NAMESPACE_BEGIN

VulkanInstance& VulkanInstance::get()
{
	VULKAN_ASSERT(sInstance != nullptr);
	return *sInstance;
}

bool VulkanInstance::initialize(const std::vector<const char*>& desiredExtensions, const std::vector<const char*>& desiredLayers)
{
	if (sInstance != nullptr)
	{
		VULKAN_LOG_WARNING("Instance already initialized");
		return true;
	}
	sInstance = new VulkanInstance();
	if (sInstance != nullptr)
	{
		if (!sInstance->initializeInternal(desiredExtensions, desiredLayers))
		{
			uninitialize();
		}
	}
	return sInstance != nullptr;
}

bool VulkanInstance::uninitialize()
{
	if (sInstance == nullptr)
	{
		VULKAN_LOG_WARNING("Instance not initialized");
		return true;
	}
	
	sInstance->uninitializeInternal();
	
	delete sInstance;
	sInstance = nullptr;
	
	return true;
}

bool VulkanInstance::initialized()
{
	return sInstance != nullptr;
}

VulkanPhysicalDevice& VulkanInstance::getPhysicalDevice(VulkanU32 index)
{
	return *mPhysicalDevices[index];
}

const VulkanPhysicalDevice& VulkanInstance::getPhysicalDevice(VulkanU32 index) const
{
	return *mPhysicalDevices[index];
}

/*
const std::vector<VulkanPhysicalDevicePtr>& VulkanInstance::getPhysicalDevices() const
{
	return mPhysicalDevices;
}
*/

VulkanU32 VulkanInstance::getPhysicalDeviceCount() const
{
	return (VulkanU32)mPhysicalDevices.size();
}

VulkanSurfacePtr VulkanInstance::createSurface(const VulkanWindowParameters& windowParameters)
{
	return VulkanSurface::createSurface(windowParameters);
}

const VkInstance& VulkanInstance::getHandle() const
{
	return mInstance;
}

const std::vector<const char*>& VulkanInstance::getExtensions() const
{
	return mExtensions;
}

const std::vector<const char*>& VulkanInstance::getLayers() const
{
	return mLayers;
}

bool VulkanInstance::isInstanceExtensionSupported(const char* extensionName)
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

bool VulkanInstance::isInstanceLayerSupported(const char* layerName)
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

VulkanInstance::VulkanInstance()
	: mInstance(VK_NULL_HANDLE)
	, mDebugMessenger(VK_NULL_HANDLE)
	, mExtensions()
	, mLayers()
	, mPhysicalDevices()
{
}

VulkanInstance::~VulkanInstance()
{
	uninitializeInternal();
}

bool VulkanInstance::initializeInternal(const std::vector<const char*>& desiredExtensions, const std::vector<const char*>& desiredLayers)
{	
	if (!VulkanLoader::ensureDynamicLibraryLoaded())
	{
		return false;
	}
	if (!VulkanLoader::ensureExportedFunctionLoaded())
	{
		return false;
	}
	if (!VulkanLoader::ensureGlobalLevelFunctionsLoaded())
	{
		return false;
	}
	
	// Extensions
	mExtensions.reserve(desiredExtensions.size());
	for (const char* extension : desiredExtensions)
	{
		mExtensions.push_back(extension);
	}
	#if (defined VULKAN_KHR)
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
	#endif
	#if (defined VULKAN_BUILD_DEBUG)
	{
		mExtensions.emplace_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
	}
	#endif
	{
		bool missingExtension = false;
		for (auto& desiredExtension : mExtensions)
		{
			if (!isInstanceExtensionSupported(desiredExtension))
			{
				VULKAN_LOG_ERROR("Could not find a desired instance extension : %s", desiredExtension);
				missingExtension = true;
			}
		}
		if (missingExtension)
		{
			return false;
		}
	}
	
	// Layers
	mLayers.reserve(desiredLayers.size());
	for (const char* layer : desiredLayers)
	{
		mLayers.push_back(layer);
	}
	#if (defined VULKAN_BUILD_DEBUG)
	{
		mLayers.emplace_back("VK_LAYER_LUNARG_standard_validation"); // TODO : Macro for this ?
	}
	#endif
	{
		bool missingLayer = false;
		for (auto& desiredLayer : mLayers)
		{
			if (!isInstanceLayerSupported(desiredLayer))
			{
				VULKAN_LOG_ERROR("Could not find a desired instance layer : %s", desiredLayer);
				missingLayer = true;
			}
		}
		if (missingLayer)
		{
			return false;
		}
	}

	// TODO : Get application name and version
	// TODO : Get engine name and version
	VkApplicationInfo applicationInfo = {
		VK_STRUCTURE_TYPE_APPLICATION_INFO,                // VkStructureType           sType
		nullptr,                                           // const void              * pNext
		"VulkanTest",                              // const char              * pApplicationName
		VK_MAKE_VERSION(1, 0, 0),                          // uint32_t                  applicationVersion
		"VulkanTest",                                    // const char              * pEngineName
		VK_MAKE_VERSION(1, 0, 0),                          // uint32_t                  engineVersion
		VK_MAKE_VERSION(1, 0, 0)                           // uint32_t                  apiVersion
	};

	VkInstanceCreateInfo instanceCreateInfo = {
		VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO,             // VkStructureType           sType
		nullptr,                                            // const void              * pNext
		0,                                                  // VkInstanceCreateFlags     flags
		&applicationInfo,                                   // const VkApplicationInfo * pApplicationInfo
		static_cast<VulkanU32>(mLayers.size()),              // uint32_t                  enabledLayerCount
		mLayers.data(),                                     // const char * const      * ppEnabledLayerNames
		static_cast<VulkanU32>(mExtensions.size()),          // uint32_t                  enabledExtensionCount
		mExtensions.data()                                  // const char * const      * ppEnabledExtensionNames
	};

	VkResult result = vkCreateInstance(&instanceCreateInfo, nullptr, &mInstance);
	if (result != VK_SUCCESS || mInstance == VK_NULL_HANDLE)
	{
		VULKAN_LOG_ERROR("Could not create Vulkan instance");
		return false;
	}

	if (!VulkanLoader::ensureInstanceLevelFunctionsLoaded(*this))
	{
		return false;
	}

	#if (defined VULKAN_BUILD_DEBUG || defined VULKAN_BUILD_RELEASE)
	{
		VkDebugUtilsMessageSeverityFlagsEXT messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT
			| VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT; 
		#if (defined VULKAN_BUILD_DEBUG)
			messageSeverity |= VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT
				| VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT;
		#endif

		VkDebugUtilsMessageTypeFlagsEXT messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT
			| VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT
			| VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
		
		VkDebugUtilsMessengerCreateInfoEXT messenger = {
			VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT,	// sType
			nullptr,													// pNext
			0,															// flags
			messageSeverity,											// messageSeverity
			messageType,												// messageType
			VulkanInstance::debugCallback,								// pfnCallback
			nullptr,													// pUserData
		};

		result = vkCreateDebugUtilsMessengerEXT(mInstance, &messenger, nullptr, &mDebugMessenger);
		if (result != VK_SUCCESS || mDebugMessenger == VK_NULL_HANDLE)
		{
			VULKAN_LOG_ERROR("Could not create DebugMessenger");
			return false;
		}
	}
	#endif

	queryAvailablePhysicalDevices();

	return true;
}

bool VulkanInstance::uninitializeInternal()
{
	if (mDebugMessenger != VK_NULL_HANDLE)
	{
		vkDestroyDebugUtilsMessengerEXT(mInstance, mDebugMessenger, nullptr);
		mDebugMessenger = VK_NULL_HANDLE;
	}

	if (mInstance != VK_NULL_HANDLE)
	{
		vkDestroyInstance(mInstance, nullptr);
		mInstance = VK_NULL_HANDLE;
	}
	
	return true;
}

bool VulkanInstance::queryAvailablePhysicalDevices()
{
	mPhysicalDevices.clear();

	VulkanU32 devicesCount;
	std::vector<VkPhysicalDevice> physicalDevices;
	
	VkResult result = vkEnumeratePhysicalDevices(mInstance, &devicesCount, nullptr);
	if (result != VK_SUCCESS || devicesCount == 0)
	{
		VULKAN_LOG_ERROR("Could not get the number of available physical devices");
		return false;
	}

	physicalDevices.resize(devicesCount);
	
	result = vkEnumeratePhysicalDevices(mInstance, &devicesCount, physicalDevices.data());
	if (result != VK_SUCCESS || devicesCount == 0)
	{
		VULKAN_LOG_ERROR("Could not enumerate physical devices");
		return false;
	}

	for (VulkanU32 i = 0; i < devicesCount; i++)
	{
		mPhysicalDevices.emplace_back(new VulkanPhysicalDevice(physicalDevices[i]));
	}

	return true;
}

VKAPI_ATTR VkBool32 VKAPI_CALL VulkanInstance::debugCallback(VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity, VkDebugUtilsMessageTypeFlagsEXT messageType, const VkDebugUtilsMessengerCallbackDataEXT * pCallbackData, void * pUserData)
{
	const char* type = "";
	if (messageType == VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT)
	{
		type = "[VALI] ";
	}
	else if (messageType == VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT)
	{
		type = "[PERF] ";
	}

	if (messageSeverity == VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT)
	{
		VULKAN_LOG_ERROR("%s%s", type, pCallbackData->pMessage);
	}
	else if (messageSeverity == VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT)
	{
		VULKAN_LOG_WARNING("%s%s", type, pCallbackData->pMessage);
	}
	else
	{
		VULKAN_LOG_INFO("%s%s", type, pCallbackData->pMessage);
	}

	return VK_FALSE;
}

bool VulkanInstance::queryAvailableInstanceExtensions()
{
	sAvailableInstanceExtensions.clear();

	VulkanU32 extensionCount = 0;
	VkResult result = VK_SUCCESS;

	result = vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, nullptr);
	if (result != VK_SUCCESS || extensionCount == 0)
	{
		VULKAN_LOG_ERROR("Could not get the number of instance extensions");
		return false;
	}

	sAvailableInstanceExtensions.resize(extensionCount);
	
	result = vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, sAvailableInstanceExtensions.data());
	if (result != VK_SUCCESS || extensionCount == 0)
	{
		VULKAN_LOG_ERROR("Could not enumerate instance extensions");
		return false;
	}

	return true;
}

bool VulkanInstance::queryAvailableInstanceLayers()
{
	sAvailableInstanceLayers.clear();

	VulkanU32 layerCount = 0;
	VkResult result = VK_SUCCESS;

	result = vkEnumerateInstanceLayerProperties(&layerCount, nullptr);
	if (result != VK_SUCCESS || layerCount == 0)
	{
		VULKAN_LOG_ERROR("Could not get the number of instance layers");
		return false;
	}

	sAvailableInstanceLayers.resize(layerCount);
	
	result = vkEnumerateInstanceLayerProperties(&layerCount, sAvailableInstanceLayers.data());
	if (result != VK_SUCCESS || layerCount == 0)
	{
		VULKAN_LOG_ERROR("Could not enumerate instance layers");
		return false;
	}

	return true;
}

std::vector<VkExtensionProperties> VulkanInstance::sAvailableInstanceExtensions;
std::vector<VkLayerProperties> VulkanInstance::sAvailableInstanceLayers;
VulkanInstance* VulkanInstance::sInstance = nullptr;

VULKAN_NAMESPACE_END