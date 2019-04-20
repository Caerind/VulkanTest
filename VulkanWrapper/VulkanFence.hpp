#pragma once

#include "VulkanFunctions.hpp"

VULKAN_NAMESPACE_BEGIN

class VulkanFence : public VulkanDeviceObject<VulkanObjectType_Fence>
{
	public:
		~VulkanFence();

		// TODO : Friendlier Time for U64 ?
		bool wait(VulkanU64 timeout);

		bool reset();

		// TODO : Really precise ? If not : remove or improve
		bool isSignaled() const;

		const VkFence& getHandle() const;

	private:
		friend class VulkanDevice;
		static VulkanFencePtr createFence(bool signaled);

		VulkanFence(bool signaled);

		bool init();
		void release();

	private:
		VkFence mFence;

		bool mSignaled;
};

VULKAN_NAMESPACE_END