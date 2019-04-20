#pragma once

#include "VulkanFunctions.hpp"

VULKAN_NAMESPACE_BEGIN

class VulkanSemaphore : public VulkanDeviceObject<VulkanObjectType_Semaphore>
{
	public:
		~VulkanSemaphore();

		const VkSemaphore& getHandle() const;

	private:
		friend class VulkanDevice;
		static VulkanSemaphorePtr createSemaphore();

		VulkanSemaphore();

		bool init();
		void release();

	private:
		VkSemaphore mSemaphore;
};

VULKAN_NAMESPACE_END