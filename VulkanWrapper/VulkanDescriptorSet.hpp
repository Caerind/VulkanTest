#ifndef NU_VULKAN_DESCRIPTOR_SET_HPP
#define NU_VULKAN_DESCRIPTOR_SET_HPP

#include "VulkanFunctions.hpp"

#include <memory>

namespace nu
{
namespace Vulkan
{

class DescriptorPool;
class DescriptorSetLayout;
class DescriptorSet
{
	public:
		typedef std::unique_ptr<DescriptorSet> Ptr;

		~DescriptorSet();

		bool isInitialized() const;
		const VkDescriptorSet& getHandle() const;
		const DescriptorPool& getDescriptorPool() const;

	private:
		friend class DescriptorPool;
		DescriptorSet(DescriptorPool& descriptorPool, DescriptorSetLayout* descriptorSetLayout);

		bool init();
		bool release();

		DescriptorPool& mDescriptorPool;
		DescriptorSetLayout* mDescriptorSetLayout;
		VkDescriptorSet mDescriptorSet;
};

} // namespace Vulkan
} // namespace nu

#endif // NU_VULKAN_DESCRIPTOR_SET_HPP