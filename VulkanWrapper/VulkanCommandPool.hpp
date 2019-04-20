#pragma once

#include "VulkanFunctions.hpp"

VULKAN_NAMESPACE_BEGIN

class VulkanCommandPool : public VulkanDeviceObject<VulkanObjectType_CommandPool>
{
	public:
		// TODO : Store settings as attributes ?
		static VulkanCommandPoolPtr createCommandPool(VkCommandPoolCreateFlags parameters, VulkanU32 queueFamily);

		~VulkanCommandPool();

        // TODO : Store the unique ptr inside pool ?
		VulkanCommandBufferPtr allocatePrimaryCommandBuffer();
		VulkanCommandBufferPtr allocateSecondaryCommandBuffer();
		// TODO : Allocate n at once
		
		bool reset(); // TODO : Add release resources parameter ?

		// TODO : Free command buffers ?

		bool isInitialized() const;
		const VkCommandPool& getHandle() const;

	private:
		VulkanCommandPool();

		bool init(VkCommandPoolCreateFlags parameters, VulkanU32 queueFamily);
		bool release();

		VkCommandPool mCommandPool;
};

VULKAN_NAMESPACE_END