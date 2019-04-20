#pragma once

#include "VulkanFunctions.hpp"

VULKAN_NAMESPACE_BEGIN

class VulkanComputePipeline : public VulkanDeviceObject<VulkanObjectType_ComputePipeline>
{
	public:
		static VulkanComputePipelinePtr createComputePipeline(VulkanShaderModule* computeShader, VulkanPipelineLayout* layout, VulkanPipelineCache* cache, VkPipelineCreateFlags additionalOptions);

		~VulkanComputePipeline();

		bool isInitialized() const;
		const VkPipeline& getHandle() const;

	private:
		VulkanComputePipeline(VulkanShaderModule* computeShader, VulkanPipelineLayout* layout, VulkanPipelineCache* cache, VkPipelineCreateFlags additionalOptions);

		bool init();
		bool release();

		VkPipeline mComputePipeline;

		VkPipelineShaderStageCreateInfo mComputeShader;
		VulkanPipelineLayout* mLayout;
		VulkanPipelineCache* mCache;
		VkPipelineCreateFlags mAdditionalOptions;
};

VULKAN_NAMESPACE_END