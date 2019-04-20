#include "VulkanDescriptorPool.hpp"

#include "VulkanDescriptorSet.hpp"
#include "VulkanDescriptorSetLayout.hpp"
#include "VulkanDevice.hpp"

VULKAN_NAMESPACE_BEGIN

VulkanDescriptorPoolPtr VulkanDescriptorPool::createDescriptorPool(bool freeIndividualSets, VulkanU32 maxSetsCount, const std::vector<VkDescriptorPoolSize>& descriptorTypes)
{
	VulkanDescriptorPoolPtr descriptorPool(new VulkanDescriptorPool(freeIndividualSets, maxSetsCount, descriptorTypes));
	if (descriptorPool != nullptr)
	{
		if (!descriptorPool->init())
		{
			descriptorPool.reset();
		}
	}
	return descriptorPool;
}

VulkanDescriptorPool::~VulkanDescriptorPool()
{
	release();
	
	VULKAN_OBJECTTRACKER_UNREGISTER();
}

VulkanDescriptorSetPtr VulkanDescriptorPool::allocateDescriptorSet(VulkanDescriptorSetLayout* descriptorSetLayout)
{
	VulkanDescriptorSetPtr descriptorSet(new VulkanDescriptorSet(*this, descriptorSetLayout));
	if (descriptorSet != nullptr)
	{
		if (!descriptorSet->init())
		{
			descriptorSet.reset();
		}
	}
	return descriptorSet;
}

bool VulkanDescriptorPool::allocateDescriptorSets(const std::vector<VulkanDescriptorSetLayout*> descriptorSetLayouts, std::vector<VulkanDescriptorSetPtr>& descriptorSets)
{
	// TODO : Allocate more than one at once
	return false;
}

bool VulkanDescriptorPool::freeIndividualSets() const
{
	return mFreeIndividualSets;
}

bool VulkanDescriptorPool::reset()
{
	// TODO : What is the 0 parameters ?
	VkResult result = vkResetDescriptorPool(getDeviceHandle(), mDescriptorPool, 0);
	if (result != VK_SUCCESS)
	{
		// TODO : Log more
		VULKAN_LOG_ERROR("Error occurred during descriptor pool reset");
		return false;
	}
	return true;
}

bool VulkanDescriptorPool::isInitialized() const
{
	return mDescriptorPool != VK_NULL_HANDLE;
}

const VkDescriptorPool& VulkanDescriptorPool::getHandle() const
{
	return mDescriptorPool;
}

VulkanDescriptorPool::VulkanDescriptorPool(bool freeIndividualSets, VulkanU32 maxSetsCount, const std::vector<VkDescriptorPoolSize>& descriptorTypes)
	: mDescriptorPool(VK_NULL_HANDLE)
	, mFreeIndividualSets(freeIndividualSets)
	, mMaxSetsCount(maxSetsCount)
	, mDescriptorTypes(descriptorTypes)
{
	VULKAN_OBJECTTRACKER_REGISTER();
}

bool VulkanDescriptorPool::init()
{
	VkDescriptorPoolCreateInfo descriptorPoolCreateInfo = {
		VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO,                // VkStructureType                sType
		nullptr,                                                      // const void                   * pNext
		mFreeIndividualSets ?                                         // VkDescriptorPoolCreateFlags    flags
		VK_DESCRIPTOR_POOL_CREATE_FREE_DESCRIPTOR_SET_BIT : 0u,
		mMaxSetsCount,                                                // uint32_t                       maxSets
		static_cast<VulkanU32>(mDescriptorTypes.size()),               // uint32_t                       poolSizeCount
		mDescriptorTypes.data()                                       // const VkDescriptorPoolSize   * pPoolSizes
	};

	VkResult result = vkCreateDescriptorPool(getDeviceHandle(), &descriptorPoolCreateInfo, nullptr, &mDescriptorPool);
	if (result != VK_SUCCESS || mDescriptorPool == VK_NULL_HANDLE)
	{
		VULKAN_LOG_ERROR("Could not create a descriptor pool");
		return false;
	}

	return true;
}

bool VulkanDescriptorPool::release()
{
	if (mDescriptorPool != VK_NULL_HANDLE)
	{
		vkDestroyDescriptorPool(getDeviceHandle(), mDescriptorPool, nullptr);
		mDescriptorPool = VK_NULL_HANDLE;
		return true;
	}
	return false;
}

VULKAN_NAMESPACE_END