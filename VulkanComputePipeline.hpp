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
class ShaderModule;
class ComputePipeline
{
	public:
		typedef std::unique_ptr<ComputePipeline> Ptr;

		static ComputePipeline::Ptr createComputePipeline(Device& device, ShaderModule* computeShader, PipelineLayout* layout, PipelineCache* cache, VkPipelineCreateFlags additionalOptions);

		~ComputePipeline();

		bool isInitialized() const;
		const VkPipeline& getHandle() const;
		const Device& getDeviceHandle() const;

	private:
		ComputePipeline(Device& device, ShaderModule* computeShader, PipelineLayout* layout, PipelineCache* cache, VkPipelineCreateFlags additionalOptions);

		bool init();
		bool release();

		Device& mDevice;
		VkPipeline mComputePipeline;

		VkPipelineShaderStageCreateInfo mComputeShader;
		PipelineLayout* mLayout;
		PipelineCache* mCache;
		VkPipelineCreateFlags mAdditionalOptions;
};

} // namespace Vulkan
} // namespace nu

#endif // NU_VULKAN_COMPUTE_PIPELINE_HPP