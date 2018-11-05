#include "VulkanComputePipeline.hpp"

#include "VulkanDevice.hpp"
#include "VulkanShaderModule.hpp"

namespace nu
{
namespace Vulkan
{

ComputePipeline::Ptr ComputePipeline::createComputePipeline(Device& device, ShaderModule* computeShader, PipelineLayout* layout, PipelineCache* cache, VkPipelineCreateFlags additionalOptions)
{
	ComputePipeline::Ptr computePipeline(new ComputePipeline(device, computeShader, layout, cache, additionalOptions));
	if (computePipeline != nullptr)
	{
		if (!computePipeline->init())
		{
			computePipeline.reset();
		}
	}
	return computePipeline;
}

ComputePipeline::~ComputePipeline()
{
	ObjectTracker::unregisterObject(ObjectType_ComputePipeline);

	release();
}

bool ComputePipeline::isInitialized() const
{
	return mComputePipeline != VK_NULL_HANDLE;
}

const VkPipeline& ComputePipeline::getHandle() const
{
	return mComputePipeline;
}

const Device& ComputePipeline::getDeviceHandle() const
{
	return mDevice;
}

ComputePipeline::ComputePipeline(Device& device, ShaderModule* computeShader, PipelineLayout* layout, PipelineCache* cache, VkPipelineCreateFlags additionalOptions)
	: mDevice(device)
	, mComputePipeline(VK_NULL_HANDLE)
	, mComputeShader(computeShader->getShaderStage())
	, mLayout(layout)
	, mCache(cache)
	, mAdditionalOptions(additionalOptions)
{
	ObjectTracker::registerObject(ObjectType_ComputePipeline);
}

bool ComputePipeline::init()
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

	VkResult result = vkCreateComputePipelines(mDevice.getHandle(), cacheHandle, 1, &computePipelineCreateInfo, nullptr, &mComputePipeline);
	if (result != VK_SUCCESS || mComputePipeline == VK_NULL_HANDLE)
	{
		// TODO : Use Numea System Log
		printf("Could not create a compute pipeline\n");
		return false;
	}

	return true;
}

bool ComputePipeline::release()
{
	if (mComputePipeline != VK_NULL_HANDLE)
	{
		vkDestroyPipeline(mDevice.getHandle(), mComputePipeline, nullptr);
		mComputePipeline = VK_NULL_HANDLE;
		return true;
	}
	return false;
}

} // namespace Vulkan
} // namespace nu