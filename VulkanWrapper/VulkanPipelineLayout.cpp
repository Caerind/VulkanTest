#include "VulkanPipelineLayout.hpp"

#include "VulkanDevice.hpp"

namespace nu
{
namespace Vulkan
{

PipelineLayout::Ptr PipelineLayout::createPipelineLayout(Device& device, const std::vector<VkDescriptorSetLayout>& descriptorSetLayouts, const std::vector<VkPushConstantRange> pushConstantRanges)
{
	PipelineLayout::Ptr pipelineLayout(new PipelineLayout(device, descriptorSetLayouts, pushConstantRanges));
	if (pipelineLayout != nullptr)
	{
		if (!pipelineLayout->init())
		{
			pipelineLayout.reset();
		}
	}
	return pipelineLayout;
}

PipelineLayout::~PipelineLayout()
{
	ObjectTracker::unregisterObject(ObjectType_PipelineLayout);

	release();
}

bool PipelineLayout::isInitialized() const
{
	return mPipelineLayout != VK_NULL_HANDLE;
}

const VkPipelineLayout& PipelineLayout::getHandle() const
{
	return mPipelineLayout;
}

const VkDevice& PipelineLayout::getDeviceHandle() const
{
	return mDevice.getHandle();
}

PipelineLayout::PipelineLayout(Device& device, const std::vector<VkDescriptorSetLayout>& descriptorSetLayouts, const std::vector<VkPushConstantRange> pushConstantRanges)
	: mDevice(device)
	, mPipelineLayout(VK_NULL_HANDLE)
	, mDescriptorSetLayouts(descriptorSetLayouts)
	, mPushConstantRanges(pushConstantRanges)
{
	ObjectTracker::registerObject(ObjectType_PipelineLayout);
}

bool PipelineLayout::init()
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

	VkResult result = vkCreatePipelineLayout(mDevice.getHandle(), &pipelineLayoutCreateInfo, nullptr, &mPipelineLayout);
	if (result != VK_SUCCESS || mPipelineLayout == VK_NULL_HANDLE)
	{
		// TODO : Use Numea System Log
		printf("Could not create a pipelineLayout\n");
		return false;
	}

	return true;
}

bool PipelineLayout::release()
{
	if (mPipelineLayout != VK_NULL_HANDLE)
	{
		vkDestroyPipelineLayout(mDevice.getHandle(), mPipelineLayout, nullptr);
		mPipelineLayout = VK_NULL_HANDLE;
		return true;
	}
	return false;
}

} // namespace Vulkan
} // namespace nu