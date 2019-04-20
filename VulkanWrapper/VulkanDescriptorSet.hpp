#pragma once

#include "VulkanFunctions.hpp"

VULKAN_NAMESPACE_BEGIN

class VulkanDescriptorSet : public VulkanObject<VulkanObjectType_DescriptorSet>
{
	public:
		~VulkanDescriptorSet();

		bool isInitialized() const;
		const VkDescriptorSet& getHandle() const;
		const VulkanDescriptorPool& getDescriptorPool() const;

	private:
		friend class VulkanDescriptorPool;
		VulkanDescriptorSet(VulkanDescriptorPool& descriptorPool, VulkanDescriptorSetLayout* descriptorSetLayout);

		bool init();
		bool release();

		VulkanDescriptorPool& mDescriptorPool;
		VulkanDescriptorSetLayout* mDescriptorSetLayout;
		VkDescriptorSet mDescriptorSet;
};

VULKAN_NAMESPACE_END