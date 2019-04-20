#pragma once

#include "VulkanFunctions.hpp"

VULKAN_NAMESPACE_BEGIN

class VulkanQueueParameters
{
	public:
		enum QueueFlag
		{
			Graphics = 1 << 0,
			Compute = 1 << 1,
			Transfer = 1 << 2,
			SparseBinding = 1 << 3,
			Present = 1 << 4
		};

		VulkanQueueParameters();

		void addQueueInfo(VulkanU32 count, VulkanU32 flags);

	private:
		friend class VulkanDevice;
		friend class VulkanQueueManager;

		struct QueueUserInfo
		{
			QueueUserInfo(VulkanU32 pIndex, VulkanU32 pCount, VulkanU32 pFlags);

			VulkanU32 index;
			VulkanU32 count;
			VulkanU32 flags;

			VkQueueFlags convertFlagsToVkQueueFlags(bool* present);
		};

		VulkanU32 getCurrentIndex() const;

		std::vector<QueueUserInfo> mQueueInfos;
};

class VulkanQueueManager
{
	public:
		VulkanQueue* getQueue(VulkanU32 index);
		const VulkanQueue* getQueue(VulkanU32 index) const;
		VulkanU32 getQueueCount() const;

	private:
		friend class VulkanDevice;
		VulkanQueueManager(VulkanDevice& device);
		~VulkanQueueManager();

		bool initialize(const VulkanQueueParameters& queueParameters, VulkanSurface* presentationSurface, std::vector<VkDeviceQueueCreateInfo>& queueCreateInfos);
		bool initializeQueues();
		bool release();

		struct QueueInternalInfo
		{
			QueueInternalInfo(VulkanU32 pFamily, VulkanU32 pCount);

			VulkanU32 family;
			VulkanU32 count;
			std::vector<float> priorities;
		};

	private:
		VulkanDevice& mDevice;
		std::vector<VulkanQueuePtr> mQueues;
		std::vector<QueueInternalInfo> mQueueInternalInfos;
};

VULKAN_NAMESPACE_END