#include "VulkanPipelineLayout.hpp"

#include "VulkanDevice.hpp"

VULKAN_NAMESPACE_BEGIN

VulkanPipelineLayoutPtr VulkanPipelineLayout::createPipelineLayout(const std::vector<VkDescriptorSetLayout>& descriptorSetLayouts, const std::vector<VkPushConstantRange> pushConstantRanges)
{
	VulkanPipelineLayoutPtr pipelineLayout(new VulkanPipelineLayout(descriptorSetLayouts, pushConstantRanges));
	if (pipelineLayout != nullptr)
	{
		if (!pipelineLayout->init())
		{
			pipelineLayout.reset();
		}
	}
	return pipelineLayout;
}

VulkanPipelineLayout::~VulkanPipelineLayout()
{
	release();

	VULKAN_OBJECTTRACKER_UNREGISTER();
}

bool VulkanPipelineLayout::isInitialized() const
{
	return mPipelineLayout != VK_NULL_HANDLE;
}

const VkPipelineLayout& VulkanPipelineLayout::getHandle() const
{
	return mPipelineLayout;
}

VulkanPipelineLayout::VulkanPipelineLayout(const std::vector<VkDescriptorSetLayout>& descriptorSetLayouts, const std::vector<VkPushConstantRange> pushConstantRanges)
	: mPipelineLayout(VK_NULL_HANDLE)
	, mDescriptorSetLayouts(descriptorSetLayouts)
	, mPushConstantRanges(pushConstantRanges)
{
	VULKAN_OBJECTTRACKER_REGISTER();
}

bool VulkanPipelineLayout::init()
{
	VkPipelineLayoutCreateInfo pipelineLayoutCreateInfo = {
		VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO,          // VkStructureType                  sType
		nullptr,                                                // const void                     * pNext
		0,                                                      // VkPipelineLayoutCreateFlags      flags
		static_cast<uint32_t>(mDescriptorSetLayouts.size()),    // uint32_t                         setLayoutCount
		mDescriptorSetLayouts.data(),                           // const VkDescriptorSetLayout    * pSetLayouts
		static_cast<uint32_t>(mPushConstantRanges.size()),      // uint32_t                         pushConstantRangeCount
		mPushConstantRanges.data()                              // const VkPushConstantRange      * pPushConstantRanges
	};

	VkResult result = vkCreatePipelineLayout(getDeviceHandle(), &pipelineLayoutCreateInfo, nullptr, &mPipelineLayout);
	if (result != VK_SUCCESS || mPipelineLayout == VK_NULL_HANDLE)
	{
		// TODO : Use Numea System Log
		printf("Could not create a pipelineLayout\n");
		return false;
	}

	return true;
}

bool VulkanPipelineLayout::release()
{
	if (mPipelineLayout != VK_NULL_HANDLE)
	{
		vkDestroyPipelineLayout(getDeviceHandle(), mPipelineLayout, nullptr);
		mPipelineLayout = VK_NULL_HANDLE;
		return true;
	}
	return false;
}

VULKAN_NAMESPACE_END