#ifndef NU_VULKAN_COMPUTE_PIPELINE_HPP
#define NU_VULKAN_COMPUTE_PIPELINE_HPP

#include "VulkanFunctions.hpp"

#include <memory>

#include "VulkanPipelineLayout.hpp"
#include "VulkanPipelineCache.hpp"

namespace nu
{
namespace Vulkan
{

class Device;
class ComputePipeline
{
	public:
		typedef std::unique_ptr<ComputePipeline> Ptr;

		static ComputePipeline::Ptr createComputePipeline(Device& device, const VkPipelineShaderStageCreateInfo& computeShaders, PipelineLayout* layout, PipelineCache* cache, VkPipelineCreateFlags additionalOptions);

		~ComputePipeline();

		bool isInitialized() const;
		const VkPipeline& getHandle() const;
		const Device& getDeviceHandle() const;

	private:
		ComputePipeline(Device& device, const VkPipelineShaderStageCreateInfo& computeShaders, PipelineLayout* layout, PipelineCache* cache, VkPipelineCreateFlags additionalOptions);

		bool init();
		bool release();

		Device& mDevice;
		VkPipeline mComputePipeline;

		VkPipelineShaderStageCreateInfo mComputeShaders;
		PipelineLayout* mLayout;
		PipelineCache* mCache;
		VkPipelineCreateFlags mAdditionalOptions;
};

} // namespace Vulkan
} // namespace nu

#endif // NU_VULKAN_COMPUTE_PIPELINE_HPP