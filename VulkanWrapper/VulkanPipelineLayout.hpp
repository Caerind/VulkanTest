#pragma once

#include "VulkanFunctions.hpp"

VULKAN_NAMESPACE_BEGIN

class VulkanPipelineLayout : public VulkanDeviceObject<VulkanObjectType_PipelineLayout>
{
	public:
		static VulkanPipelineLayoutPtr createPipelineLayout(const std::vector<VkDescriptorSetLayout>& descriptorSetLayouts, const std::vector<VkPushConstantRange> pushConstantRanges);

		~VulkanPipelineLayout();

		bool isInitialized() const;
		const VkPipelineLayout& getHandle() const;

	private:
		VulkanPipelineLayout(const std::vector<VkDescriptorSetLayout>& descriptorSetLayouts, const std::vector<VkPushConstantRange> pushConstantRanges);

		bool init();
		bool release();

		VkPipelineLayout mPipelineLayout;

		std::vector<VkDescriptorSetLayout> mDescriptorSetLayouts;
		std::vector<VkPushConstantRange> mPushConstantRanges;
};

VULKAN_NAMESPACE_END