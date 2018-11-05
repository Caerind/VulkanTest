#include "VulkanDescriptorSet.hpp"

#include "VulkanDescriptorPool.hpp"
#include "VulkanDescriptorSetLayout.hpp"

namespace nu
{
namespace Vulkan
{

DescriptorSet::~DescriptorSet()
{
	ObjectTracker::unregisterObject(ObjectType_DescriptorSet);

	release();
}

bool DescriptorSet::isInitialized() const
{
	return mDescriptorSet != VK_NULL_HANDLE;
}

const VkDescriptorSet& DescriptorSet::getHandle() const
{
	return mDescriptorSet;
}

const DescriptorPool& DescriptorSet::getDescriptorPool() const
{
	return mDescriptorPool;
}

DescriptorSet::DescriptorSet(DescriptorPool& descriptorPool, DescriptorSetLayout* descriptorSetLayout)
	: mDescriptorPool(descriptorPool)
	, mDescriptorSetLayout(descriptorSetLayout)
	, mDescriptorSet(VK_NULL_HANDLE)
{
	ObjectTracker::registerObject(ObjectType_DescriptorSet);
}

bool DescriptorSet::init()
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
		// TODO : Use Numea System Log
		printf("Could not allocate descriptor set\n");
		return false;
	}

	return true;
}

bool DescriptorSet::release()
{
	if (mDescriptorSet != VK_NULL_HANDLE)
	{
		if (mDescriptorPool.freeIndividualSets())
		{
			VkResult result = vkFreeDescriptorSets(mDescriptorPool.getDeviceHandle(), mDescriptorPool.getHandle(), 1, &mDescriptorSet);
			if (result != VK_SUCCESS)
			{
				// TODO : Use Numea System Log
				printf("Error occurred during freeing descriptor set\n");
				return false;
			}
		}
		mDescriptorSet = VK_NULL_HANDLE;
		return true;
	}
	return false;
}

} // namespace Vulkan
} // namespace nu