#include "VulkanDescriptorSet.hpp"

#include "VulkanDescriptorPool.hpp"
#include "VulkanDescriptorSetLayout.hpp"
#include "VulkanDevice.hpp"

VULKAN_NAMESPACE_BEGIN

VulkanDescriptorSet::~VulkanDescriptorSet()
{
	release();
	
	VULKAN_OBJECTTRACKER_UNREGISTER();
}

bool VulkanDescriptorSet::isInitialized() const
{
	return mDescriptorSet != VK_NULL_HANDLE;
}

const VkDescriptorSet& VulkanDescriptorSet::getHandle() const
{
	return mDescriptorSet;
}

const VulkanDescriptorPool& VulkanDescriptorSet::getDescriptorPool() const
{
	return mDescriptorPool;
}

VulkanDescriptorSet::VulkanDescriptorSet(VulkanDescriptorPool& descriptorPool, VulkanDescriptorSetLayout* descriptorSetLayout)
	: mDescriptorPool(descriptorPool)
	, mDescriptorSetLayout(descriptorSetLayout)
	, mDescriptorSet(VK_NULL_HANDLE)
{
	VULKAN_OBJECTTRACKER_REGISTER();
}

bool VulkanDescriptorSet::init()
{
	VkDescriptorSetAllocateInfo descriptorSetAllocateInfo = {
		VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO,         // VkStructureType                  sType
		nullptr,                                                // const void                     * pNext
		mDescriptorPool.getHandle(),                            // VkDescriptorPool                 descriptorPool
		1u,                                                     // uint32_t                         descriptorSetCount
		&mDescriptorSetLayout->getHandle()                      // const VkDescriptorSetLayout    * pSetLayouts
	};

	VkResult result = vkAllocateDescriptorSets(mDescriptorPool.getDeviceHandle(), &descriptorSetAllocateInfo, &mDescriptorSet);
	if (result != VK_SUCCESS)
	{
		VULKAN_LOG_ERROR("Could not allocate descriptor set");
		return false;
	}

	return true;
}

bool VulkanDescriptorSet::release()
{
	if (mDescriptorSet != VK_NULL_HANDLE)
	{
		if (mDescriptorPool.freeIndividualSets())
		{
			VkResult result = vkFreeDescriptorSets(mDescriptorPool.getDeviceHandle(), mDescriptorPool.getHandle(), 1, &mDescriptorSet);
			if (result != VK_SUCCESS)
			{
				// TODO : Log more
				VULKAN_LOG_ERROR("Error occurred during freeing descriptor set");
				return false;
			}
		}
		mDescriptorSet = VK_NULL_HANDLE;
		return true;
	}
	return false;
}

VULKAN_NAMESPACE_END