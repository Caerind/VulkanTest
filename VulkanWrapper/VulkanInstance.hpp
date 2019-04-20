#pragma once

#include "VulkanFunctions.hpp"

// TODO : Add InstanceObjectTracker ? Manage its objects ?
// TODO : Many VulkanInstance ? Like VulkanDevice ?

VULKAN_NAMESPACE_BEGIN

class VulkanInstance : public VulkanObject<VulkanObjectType_Instance>
{
	public:
		static VulkanInstance& get();
		static bool initialize(const std::vector<const char*>& desiredExtensions = {}, const std::vector<const char*>& desiredLayers = {});
		static bool uninitialize();
		static bool initialized();
		
	public:
		VulkanPhysicalDevice& getPhysicalDevice(VulkanU32 index); // TODO : Should only be used by VulkanDevice, make it private/friend ?
		const VulkanPhysicalDevice& getPhysicalDevice(VulkanU32 index) const; // TODO : Remove this ?
		//const std::vector<VulkanPhysicalDevice>& getPhysicalDevices() const;
		VulkanU32 getPhysicalDeviceCount() const; // TODO : Remove this ?
		
		// TODO : Create device from here with VulkanPhysicalDevice or index?

		VulkanSurfacePtr createSurface(const VulkanWindowParameters& windowParameters);

		const VkInstance& getHandle() const;

		const std::vector<const char*>& getExtensions() const;
		const std::vector<const char*>& getLayers() const;

		static bool isInstanceExtensionSupported(const char* extensionName);
		static bool isInstanceLayerSupported(const char* layerName);

	private:
		VulkanInstance();
		~VulkanInstance();
		
		// NonCopyable and NonMovable
		VulkanInstance(const VulkanInstance& other) = delete;
		VulkanInstance(VulkanInstance&& other) = delete;
		VulkanInstance& operator=(const VulkanInstance& other) = delete;
		VulkanInstance& operator=(VulkanInstance&& other) = delete;

		bool initializeInternal(const std::vector<const char*>& desiredExtensions, const std::vector<const char*>& desiredLayers);
		bool uninitializeInternal();

		bool queryAvailablePhysicalDevices();

		static VKAPI_ATTR VkBool32 VKAPI_CALL debugCallback(VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity, VkDebugUtilsMessageTypeFlagsEXT messageType, const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData, void* pUserData);

		static bool queryAvailableInstanceExtensions();
		static bool queryAvailableInstanceLayers();

	private:
		VkInstance mInstance;
		VkDebugUtilsMessengerEXT mDebugMessenger;

		std::vector<const char*> mExtensions;
		std::vector<const char*> mLayers;

		std::vector<VulkanPhysicalDevicePtr> mPhysicalDevices;

		static std::vector<VkExtensionProperties> sAvailableInstanceExtensions;
		static std::vector<VkLayerProperties> sAvailableInstanceLayers;
		
	private:
		static VulkanInstance* sInstance;
};

VULKAN_NAMESPACE_END