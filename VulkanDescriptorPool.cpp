#include "VulkanDescriptorPool.hpp"

#include "VulkanDevice.hpp"

namespace nu
{
namespace Vulkan
{

DescriptorPool::Ptr DescriptorPool::createDescriptorPool(Device& device, bool freeIndividualSets, uint32_t maxSetsCount, const std::vector<VkDescriptorPoolSize>& descriptorTypes)
{
	DescriptorPool::Ptr descriptorPool(new DescriptorPool(device, freeIndividualSets, maxSetsCount, descriptorTypes));
	if (descriptorPool != nullptr)
	{
		if (!descriptorPool->init())
		{
			descriptorPool.reset();
		}
	}
	return descriptorPool;
}

DescriptorPool::~DescriptorPool()
{
	ObjectTracker::unregisterObject(ObjectType_DescriptorPool);

	release();
}

DescriptorSet::Ptr DescriptorPool::allocateDescriptorSet(DescriptorSetLayout* descriptorSetLayout)
{
	DescriptorSet::Ptr descriptorSet(new DescriptorSet(*this, descriptorSetLayout));
	if (descriptorSet != nullptr)
	{
		if (!descriptorSet->init())
		{
			descriptorSet.reset();
		}
	}
	return descriptorSet;
}

bool DescriptorPool::allocateDescriptorSets(const std::vector<DescriptorSetLayout*> descriptorSetLayouts, std::vector<DescriptorSet::Ptr>& descriptorSets)
{
	// TODO : Allocate more than one at once
	return false;
}

bool DescriptorPool::freeIndividualSets() const
{
	return mFreeIndividualSets;
}

bool DescriptorPool::reset()
{
	VkResult result = vkResetDescriptorPool(mDevice.getHandle(), mDescriptorPool, 0);
	if (result != VK_SUCCESS)
	{
		// TODO : Use Numea System Log
		printf("Error occurred during descriptor pool reset\n");
		return false;
	}
	return true;
}

bool DescriptorPool::isInitialized() const
{
	return mDescriptorPool != VK_NULL_HANDLE;
}

const VkDescriptorPool& DescriptorPool::getHandle() const
{
	return mDescriptorPool;
}

const VkDevice& DescriptorPool::getDeviceHandle() const
{
	return mDevice.getHandle();
}

DescriptorPool::DescriptorPool(Device& device, bool freeIndividualSets, uint32_t maxSetsCount, const std::vector<VkDescriptorPoolSize>& descriptorTypes)
	: mDevice(device)
	, mDescriptorPool(VK_NULL_HANDLE)
	, mFreeIndividualSets(freeIndividualSets)
	, mMaxSetsCount(maxSetsCount)
	, mDescriptorTypes(descriptorTypes)
{
	ObjectTracker::registerObject(ObjectType_DescriptorPool);
}

bool DescriptorPool::init()
{
	VkDescriptorPoolCreateInfo descriptorPoolCreateInfo = {
		VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO,                // VkStructureType                sType
		nullptr,                                                      // const void                   * pNext
		mFreeIndividualSets ?                                         // VkDescriptorPoolCreateFlags    flags
		VK_DESCRIPTOR_POOL_CREATE_FREE_DESCRIPTOR_SET_BIT : 0u,
		mMaxSetsCount,                                                // uint32_t                       maxSets
		static_cast<uint32_t>(mDescriptorTypes.size()),               // uint32_t                       poolSizeCount
		mDescriptorTypes.data()                                       // const VkDescriptorPoolSize   * pPoolSizes
	};

	VkResult result = vkCreateDescriptorPool(mDevice.getHandle(), &descriptorPoolCreateInfo, nullptr, &mDescriptorPool);
	if (result != VK_SUCCESS || mDescriptorPool == VK_NULL_HANDLE)
	{
		// TODO : Use Numea System Log
		printf("Could not create a descriptor pool\n");
		return false;
	}

	return true;
}

bool DescriptorPool::release()
{
	if (mDescriptorPool != VK_NULL_HANDLE)
	{
		vkDestroyDescriptorPool(mDevice.getHandle(), mDescriptorPool, nullptr);
		mDescriptorPool = VK_NULL_HANDLE;
		return true;
	}
	return false;
}

} // namespace Vulkan
} // namespace nu