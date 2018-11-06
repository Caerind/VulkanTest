#ifndef NU_VULKAN_PIPELINE_LAYOUT_HPP
#define NU_VULKAN_PIPELINE_LAYOUT_HPP

#include "VulkanFunctions.hpp"

#include <memory>
#include <vector>

namespace nu
{
namespace Vulkan
{

class Device;
class PipelineLayout
{
	public:
		typedef std::unique_ptr<PipelineLayout> Ptr;

		static PipelineLayout::Ptr createPipelineLayout(Device& device, const std::vector<VkDescriptorSetLayout>& descriptorSetLayouts, const std::vector<VkPushConstantRange> pushConstantRanges);

		~PipelineLayout();

		bool isInitialized() const;
		const VkPipelineLayout& getHandle() const;
		const VkDevice& getDeviceHandle() const;

	private:
		PipelineLayout(Device& device, const std::vector<VkDescriptorSetLayout>& descriptorSetLayouts, const std::vector<VkPushConstantRange> pushConstantRanges);

		bool init();
		bool release();

		Device& mDevice;
		VkPipelineLayout mPipelineLayout;

		std::vector<VkDescriptorSetLayout> mDescriptorSetLayouts;
		std::vector<VkPushConstantRange> mPushConstantRanges;
};

} // namespace Vulkan
} // namespace nu

#endif // NU_VULKAN_PIPELINE_LAYOUT_HPP