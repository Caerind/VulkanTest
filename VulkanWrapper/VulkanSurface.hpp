#pragma once

#include "VulkanFunctions.hpp"

VULKAN_NAMESPACE_BEGIN

class VulkanSurface : public VulkanInstanceObject<VulkanObjectType_Surface>
{
	public:
		~VulkanSurface();

		const VulkanWindowParameters& getWindowParameters() const;
		VulkanPlatformConnectionType getPlatformConnection() const;
		VulkanPlatformWindowType getPlatformWindow() const;

		const VkSurfaceKHR& getHandle() const;
		
	private:
		friend class VulkanInstance;
		static VulkanSurfacePtr createSurface(const VulkanWindowParameters& windowParameters);

		VulkanSurface(const VulkanWindowParameters& windowParameters);
		
		// NonCopyable
		VulkanSurface(const VulkanSurface& other) = delete;
		VulkanSurface& operator=(const VulkanSurface& other) = delete;

		bool init();
		void release();

		VkSurfaceKHR mSurface;

		VulkanWindowParameters mWindowParameters;
};

VULKAN_NAMESPACE_END