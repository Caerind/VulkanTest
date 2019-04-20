#include "VulkanComputePipeline.hpp"

#include "VulkanDevice.hpp"
#include "VulkanPipelineCache.hpp"
#include "VulkanPipelineLayout.hpp"
#include "VulkanShaderModule.hpp"

VULKAN_NAMESPACE_BEGIN

VulkanComputePipelinePtr VulkanComputePipeline::createComputePipeline(VulkanShaderModule* computeShader, VulkanPipelineLayout* layout, VulkanPipelineCache* cache, VkPipelineCreateFlags additionalOptions)
{
	VulkanComputePipelinePtr computePipeline(new VulkanComputePipeline(computeShader, layout, cache, additionalOptions));
	if (computePipeline != nullptr)
	{
		if (!computePipeline->init())
		{
			computePipeline.reset();
		}
	}
	return computePipeline;
}

VulkanComputePipeline::~VulkanComputePipeline()
{
	release();
	
	VULKAN_OBJECTTRACKER_UNREGISTER();
}

bool VulkanComputePipeline::isInitialized() const
{
	return mComputePipeline != VK_NULL_HANDLE;
}

const VkPipeline& VulkanComputePipeline::getHandle() const
{
	return mComputePipeline;
}

VulkanComputePipeline::VulkanComputePipeline(VulkanShaderModule* computeShader, VulkanPipelineLayout* layout, VulkanPipelineCache* cache, VkPipelineCreateFlags additionalOptions)
	: mComputePipeline(VK_NULL_HANDLE)
	, mComputeShader(computeShader->getShaderStage())
	, mLayout(layout)
	, mCache(cache)
	, mAdditionalOptions(additionalOptions)
{
	VULKAN_OBJECTTRACKER_REGISTER();
}

bool VulkanComputePipeline::init()
{
	VkComputePipelineCreateInfo computePipelineCreateInfo = {
		VK_STRUCTURE_TYPE_COMPUTE_PIPELINE_CREATE_INFO,   // VkStructureType                    sType
		nullptr,                                          // const void                       * pNext
		mAdditionalOptions,                               // VkPipelineCreateFlags              flags
		mComputeShader,                                   // VkPipelineShaderStageCreateInfo    stage
		mLayout->getHandle(),                             // VkPipelineLayout                   layout
		VK_NULL_HANDLE,                                   // VkPipeline                         basePipelineHandle
		-1                                                // int32_t                            basePipelineIndex
	};

	VkPipelineCache cacheHandle = (mCache != nullptr) ? mCache->getHandle() : VK_NULL_HANDLE;

	VkResult result = vkCreateComputePipelines(getDeviceHandle(), cacheHandle, 1, &computePipelineCreateInfo, nullptr, &mComputePipeline);
	if (result != VK_SUCCESS || mComputePipeline == VK_NULL_HANDLE)
	{
		VULKAN_LOG_ERROR("Could not create a compute pipeline");
		return false;
	}

	return true;
}

bool VulkanComputePipeline::release()
{
	if (mComputePipeline != VK_NULL_HANDLE)
	{
		vkDestroyPipeline(getDeviceHandle(), mComputePipeline, nullptr);
		mComputePipeline = VK_NULL_HANDLE;
		return true;
	}
	return false;
}

VULKAN_NAMESPACE_END