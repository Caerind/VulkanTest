#ifndef NU_VULKAN_DESCRIPTOR_SET_LAYOUT_HPP
#define NU_VULKAN_DESCRIPTOR_SET_LAYOUT_HPP

#include "VulkanFunctions.hpp"

#include <memory>
#include <vector>

namespace nu
{
namespace Vulkan
{

class Device;
class DescriptorSetLayout
{
	public:
		typedef std::unique_ptr<DescriptorSetLayout> Ptr;

		static DescriptorSetLayout::Ptr createDescriptorSetLayout(Device& device, const std::vector<VkDescriptorSetLayoutBinding>& bindings);

		~DescriptorSetLayout();

		bool isInitialized() const;
		const VkDescriptorSetLayout& getHandle() const;
		const VkDevice& getDeviceHandle() const;

	private:
		DescriptorSetLayout(Device& device, const std::vector<VkDescriptorSetLayoutBinding>& bindings);

		bool init();
		bool release();

		Device& mDevice;
		VkDescriptorSetLayout mDescriptorSetLayout;
		std::vector<VkDescriptorSetLayoutBinding> mBindings;
};

} // namespace Vulkan
} // namespace nu

#endif // NU_VULKAN_DESCRIPTOR_SET_LAYOUT_HPP