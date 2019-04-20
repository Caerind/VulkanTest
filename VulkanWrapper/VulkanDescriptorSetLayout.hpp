#pragma once

#include "VulkanFunctions.hpp"

VULKAN_NAMESPACE_BEGIN

class VulkanDescriptorSetLayout : public VulkanDeviceObject<VulkanObjectType_DescriptorSetLayout>
{
	public:
		static VulkanDescriptorSetLayoutPtr createDescriptorSetLayout(const std::vector<VkDescriptorSetLayoutBinding>& bindings);

		~VulkanDescriptorSetLayout();

		bool isInitialized() const;
		const VkDescriptorSetLayout& getHandle() const;

	private:
		VulkanDescriptorSetLayout(const std::vector<VkDescriptorSetLayoutBinding>& bindings);

		bool init();
		bool release();

		VkDescriptorSetLayout mDescriptorSetLayout;
		std::vector<VkDescriptorSetLayoutBinding> mBindings;
};

VULKAN_NAMESPACE_END