#ifndef NU_VULKAN_GRAPHICS_PIPELINE_HPP
#define NU_VULKAN_GRAPHICS_PIPELINE_HPP

#include "VulkanFunctions.hpp"

#include <memory>

#include "VulkanPipelineLayout.hpp"
#include "VulkanPipelineCache.hpp"
#include "VulkanRenderPass.hpp"

#include "VulkanShaderModule.hpp"

namespace nu
{
namespace Vulkan
{

class Device;
class GraphicsPipeline
{
	public:
		typedef std::unique_ptr<GraphicsPipeline> Ptr;

		~GraphicsPipeline();

		void setSubpass(uint32_t subpass);

		void addShaderModule(ShaderModule* shaderModule);

		void addVertexBinding(uint32_t binding, uint32_t stride, VkVertexInputRate inputRate = VK_VERTEX_INPUT_RATE_VERTEX);
		void addVertexBinding(const VkVertexInputBindingDescription& description);

		void addVertexAttribute(uint32_t location, uint32_t binding, VkFormat format, uint32_t offset);
		void addVertexAttribute(const VkVertexInputAttributeDescription& description);

		void setInputAssemblyState(VkPrimitiveTopology topology, bool primitiveRestartEnable);
		void setInputAssemblyState(const VkPipelineInputAssemblyStateCreateInfo& inputAssemblyState);
		void setTopology(VkPrimitiveTopology topology);
		void setPrimitiveRestart(bool primitiveRestartEnable);

		void setTessellationState(const VkPipelineTessellationStateCreateInfo& tessellationState);
		void setPatchControlPoints(uint32_t patchControlPoints);

		// TODO : Multiple Viewports/Scissors
		void setViewport(float x, float y, float width, float height, float minDepth, float maxDepth);
		void setViewport(const VkViewport& viewport);
		void setScissor(int32_t x, int32_t y, uint32_t width, uint32_t height);
		void setScissor(const VkRect2D& scissor);

		void setRasterizationState(bool depthClampEnable, bool rasterizerDiscardEnable, VkPolygonMode polygonMode, VkCullModeFlags cullMode, VkFrontFace frontFace, bool depthBiasEnable, float depthBiasConstantFactor, float depthBiasClamp, float depthBiasSlopeFactor, float lineWidth);
		void setRasterizationState(const VkPipelineRasterizationStateCreateInfo& rasterizationState);
		void setDepthClampEnable(bool depthClampEnable);
		void setRasterizerDiscardEnable(bool rasterizerDiscardEnable);
		void setPolygonMode(VkPolygonMode polygonMode);
		void setCullMode(VkCullModeFlags cullMode);
		void setFrontFace(VkFrontFace frontFace);
		void setDepthBiasEnable(bool depthBiasEnable);
		void setDepthBiasConstantFactor(float depthBiasConstantFactor);
		void setDepthBiasClamp(float depthBiasClamp);
		void setDepthBiasSlopeFactor(float depthBiasSlopeFactor);
		void setLineWidth(float lineWidth);

		void setMultisampleState(VkSampleCountFlagBits rasterizationSamples, bool sampleShadingEnable, float minSampleShading, const VkSampleMask* sampleMask, bool alphaToCoverageEnable, bool alphaToOneEnable);
		void setMultisampleState(const VkPipelineMultisampleStateCreateInfo& multisampleState);
		void setRasterizationSamples(VkSampleCountFlagBits rasterizationSamples);
		void setSampleShadingEnable(bool sampleShadingEnable);
		void setMinSampleShading(float minSampleShading);
		void setSampleMask(const VkSampleMask* sampleMask);
		void setAlphaToCoverageEnable(bool alphaToCoverageEnable);
		void setAlphaToOneEnable(bool alphaToOneEnable);

		void setDepthStencilState(bool depthTestEnable, bool depthWriteEnable, VkCompareOp depthCompareOp, bool depthBoundsTestEnable, bool stencilTestEnable, VkStencilOpState stencilFront, VkStencilOpState stencilBack, float minDepthBounds, float maxDepthBounds);
		void setDepthStencilState(const VkPipelineDepthStencilStateCreateInfo& depthStencilState);
		void setDepthTestEnable(bool depthTestEnable);
		void setDepthWriteEnable(bool depthWriteEnable);
		void setDepthCompareOp(VkCompareOp depthCompareOp);
		void setDepthBoundsTestEnable(bool depthBoundsTestEnable);
		void setStencilTestEnable(bool stencilTestEnable);
		void setStencilFront(VkStencilOpState stencilFront);
		void setStencilBack(VkStencilOpState stencilBack);
		void setMinDepthBounds(float minDepthBounds);
		void setMaxDepthBounds(float maxDepthBounds);

		void addBlend(bool blendEnable, VkBlendFactor srcColorBlendFactor, VkBlendFactor dstColorBlendFactor, VkBlendOp colorBlendOp, VkBlendFactor srcAlphaBlendFactor, VkBlendFactor dstAlphaBlendFactor, VkBlendOp alphaBlendOp, VkColorComponentFlags colorWriteMask);
		void addBlend(const VkPipelineColorBlendAttachmentState& blendAttachment);
		void addBlend(bool blendEnable);
		void setBlendEnable(bool blendEnable);
		void setBlendSrcColorBlendFactor(VkBlendFactor srcColorBlendFactor);
		void setBlendDstColorBlendFactor(VkBlendFactor dstColorBlendFactor);
		void setBlendColorBlendOp(VkBlendOp colorBlendOp);
		void setBlendSrcAlphaBlendFactor(VkBlendFactor srcAlphaBlendFactor);
		void setBlendDstAlphaBlendFactor(VkBlendFactor dstAlphaBlendFactor);
		void setBlendAlphaBlendOp(VkBlendOp alphaBlendOp);
		void setBlendColorWriteMask(VkColorComponentFlags colorWriteMask);

		void setBlendState(bool logicOpEnable, VkLogicOp logicOp, float r, float g, float b, float a);
		void setBlendState(const VkPipelineColorBlendStateCreateInfo& blendState);
		void setBlendLogicOpEnable(bool logicOpEnable);
		void setBlendLogicOp(VkLogicOp logicOp);
		void setBlendConstants(float r, float g, float b, float a);

		void addDynamicState(VkDynamicState dynamicState);

		// TODO : Create multiple pipelines at once
		bool create();
		bool destroy();

		bool isCreated() const;
		const VkPipeline& getHandle() const;

	private:
		friend class Device;
		GraphicsPipeline(Device& device, PipelineLayout& layout, RenderPass& renderPass, PipelineCache* cache = nullptr);

	private:
		Device& mDevice;
		PipelineLayout& mLayout;
		RenderPass& mRenderPass;
		PipelineCache* mCache;
		VkPipeline mGraphicsPipeline;
		uint32_t mSubpass;

		std::vector<VkPipelineShaderStageCreateInfo> mShaderStages;
		std::vector<VkVertexInputBindingDescription> mVertexBindingDescriptions;
		std::vector<VkVertexInputAttributeDescription> mVertexAttributeDescriptions;
		std::unique_ptr<VkPipelineInputAssemblyStateCreateInfo> mInputAssemblyState;
		std::unique_ptr<VkPipelineTessellationStateCreateInfo> mTessellationState;
		VkViewport mViewport;
		VkRect2D mScissor;
		std::unique_ptr<VkPipelineRasterizationStateCreateInfo> mRasterizationState; 
		std::unique_ptr<VkPipelineMultisampleStateCreateInfo> mMultisampleState;
		std::unique_ptr<VkPipelineDepthStencilStateCreateInfo> mDepthStencilState;
		std::vector<VkPipelineColorBlendAttachmentState> mBlendAttachments;
		VkPipelineColorBlendStateCreateInfo mBlendState;
		std::vector<VkDynamicState> mDynamicStates;

	private:
		void ensuresInputAssemblyStateInitialized();
		void ensuresTessellationStateInitialized();
		void ensuresRasterizationStateInitialized();
		void ensuresMultisampleStateInitialized();
		void ensuresDepthStencilStateInitialized();

		static const VkPipelineInputAssemblyStateCreateInfo sDefaultInputAssemblyState;
		static const VkPipelineRasterizationStateCreateInfo sDefaultRasterizationState;
		static const VkPipelineMultisampleStateCreateInfo sDefaultMultisampleState;
		static const VkPipelineDepthStencilStateCreateInfo sDefaultDepthStencilState;
};

} // namespace Vulkan
} // namespace nu

#endif // NU_VULKAN_GRAPHICS_PIPELINE_HPP