#pragma once

#include "VulkanFunctions.hpp"

VULKAN_NAMESPACE_BEGIN

class VulkanDescriptorPool : public VulkanDeviceObject<VulkanObjectType_DescriptorPool>
{
	public:
		static VulkanDescriptorPoolPtr createDescriptorPool(bool freeIndividualSets, VulkanU32 maxSetsCount, const std::vector<VkDescriptorPoolSize>& descriptorTypes);

		~VulkanDescriptorPool();

		// TODO : Should Pool manage the unique ptr ?
		VulkanDescriptorSetPtr allocateDescriptorSet(VulkanDescriptorSetLayout* descriptorSetLayout);
		bool allocateDescriptorSets(const std::vector<VulkanDescriptorSetLayout*> descriptorSetLayouts, std::vector<VulkanDescriptorSetPtr>& descriptorSets);

		// TODO : Free more than one at once

		bool freeIndividualSets() const;
		// TODO : Get maxSetsCount ?
		// TODO : Get descriptorTypes ?

		bool reset();

		bool isInitialized() const;
		const VkDescriptorPool& getHandle() const;

	private:
		VulkanDescriptorPool(bool freeIndividualSets, VulkanU32 maxSetsCount, const std::vector<VkDescriptorPoolSize>& descriptorTypes);

		bool init();
		bool release();

		VkDescriptorPool mDescriptorPool;

		bool mFreeIndividualSets;
		VulkanU32 mMaxSetsCount;
		std::vector<VkDescriptorPoolSize> mDescriptorTypes;
};

VULKAN_NAMESPACE_END