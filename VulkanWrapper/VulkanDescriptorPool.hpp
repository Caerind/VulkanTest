#ifndef NU_VULKAN_DESCRIPTOR_POOL_HPP
#define NU_VULKAN_DESCRIPTOR_POOL_HPP

#include "VulkanFunctions.hpp"

#include <memory>
#include <vector>

#include "VulkanDescriptorSet.hpp"
#include "VulkanDescriptorSetLayout.hpp"

namespace nu
{
namespace Vulkan
{

class Device;
class DescriptorPool
{
	public:
		typedef std::unique_ptr<DescriptorPool> Ptr;

		static DescriptorPool::Ptr createDescriptorPool(Device& device, bool freeIndividualSets, uint32_t maxSetsCount, const std::vector<VkDescriptorPoolSize>& descriptorTypes);

		~DescriptorPool();

		DescriptorSet::Ptr allocateDescriptorSet(DescriptorSetLayout* descriptorSetLayout);
		bool allocateDescriptorSets(const std::vector<DescriptorSetLayout*> descriptorSetLayouts, std::vector<DescriptorSet::Ptr>& descriptorSets);

		// TODO : Free more than one at once

		bool freeIndividualSets() const;

		bool reset();

		bool isInitialized() const;
		const VkDescriptorPool& getHandle() const;
		const VkDevice& getDeviceHandle() const;

	private:
		DescriptorPool(Device& device, bool freeIndividualSets, uint32_t maxSetsCount, const std::vector<VkDescriptorPoolSize>& descriptorTypes);

		bool init();
		bool release();

		Device& mDevice;
		VkDescriptorPool mDescriptorPool;

		bool mFreeIndividualSets;
		uint32_t mMaxSetsCount;
		std::vector<VkDescriptorPoolSize> mDescriptorTypes;
};

} // namespace Vulkan
} // namespace nu

#endif // NU_VULKAN_DESCRIPTOR_POOL_HPP