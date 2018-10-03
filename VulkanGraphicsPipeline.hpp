#ifndef NU_VULKAN_GRAPHICS_PIPELINE_HPP
#define NU_VULKAN_GRAPHICS_PIPELINE_HPP

#include "VulkanFunctions.hpp"

#include <memory>

#include "VulkanPipelineLayout.hpp"
#include "VulkanPipelineCache.hpp"

namespace nu
{
namespace Vulkan
{

class Device;
class GraphicsPipeline
{
	public:
		typedef std::unique_ptr<GraphicsPipeline> Ptr;

		static GraphicsPipeline::Ptr createGraphicsPipeline(Device& device, const VkGraphicsPipelineCreateInfo& createInfo, PipelineCache* cache);

		~GraphicsPipeline();

		bool isInitialized() const;
		const VkPipeline& getHandle() const;
		const Device& getDeviceHandle() const;

	private:
		GraphicsPipeline(Device& device, const VkGraphicsPipelineCreateInfo& createInfo, PipelineCache* cache);

		bool init();
		bool release();

		Device& mDevice;
		VkPipeline mGraphicsPipeline;

		VkGraphicsPipelineCreateInfo mCreateInfo;
		PipelineCache* mCache;
};

} // namespace Vulkan
} // namespace nu

#endif // NU_VULKAN_GRAPHICS_PIPELINE_HPP