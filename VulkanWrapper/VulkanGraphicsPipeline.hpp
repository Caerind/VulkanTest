#pragma once

#include "VulkanFunctions.hpp"

// TODO : Move the current pipeline creation process to another class.
//		  The current approach is very repetitive and not optimized for creating similar pipelines

VULKAN_NAMESPACE_BEGIN

class VulkanGraphicsPipeline : public VulkanObject<VulkanObjectType_GraphicsPipeline>
{
	public:
		~VulkanGraphicsPipeline();

		void setSubpass(VulkanU32 subpass);

		void addShaderModule(VulkanShaderModule* shaderModule);

		void addVertexBinding(VulkanU32 binding, VulkanU32 stride, VkVertexInputRate inputRate = VK_VERTEX_INPUT_RATE_VERTEX);
		void addVertexBinding(const VkVertexInputBindingDescription& description);

		void addVertexAttribute(VulkanU32 location, VulkanU32 binding, VkFormat format, VulkanU32 offset);
		void addVertexAttribute(const VkVertexInputAttributeDescription& description);

		void setInputAssemblyState(VkPrimitiveTopology topology, bool primitiveRestartEnable);
		void setInputAssemblyState(const VkPipelineInputAssemblyStateCreateInfo& inputAssemblyState);
		void setTopology(VkPrimitiveTopology topology);
		void setPrimitiveRestart(bool primitiveRestartEnable);

		void setTessellationState(const VkPipelineTessellationStateCreateInfo& tessellationState);
		void setPatchControlPoints(VulkanU32 patchControlPoints);

		// TODO : Multiple Viewports/Scissors
		void setViewport(VulkanF32 x, VulkanF32 y, VulkanF32 width, VulkanF32 height, VulkanF32 minDepth, VulkanF32 maxDepth);
		void setViewport(const VkViewport& viewport);
		void setScissor(VulkanI32 x, VulkanI32 y, VulkanU32 width, VulkanU32 height);
		void setScissor(const VkRect2D& scissor);

		void setRasterizationState(bool depthClampEnable, bool rasterizerDiscardEnable, VkPolygonMode polygonMode, VkCullModeFlags cullMode, VkFrontFace frontFace, bool depthBiasEnable, VulkanF32 depthBiasConstantFactor, VulkanF32 depthBiasClamp, VulkanF32 depthBiasSlopeFactor, VulkanF32 lineWidth);
		void setRasterizationState(const VkPipelineRasterizationStateCreateInfo& rasterizationState);
		void setDepthClampEnable(bool depthClampEnable);
		void setRasterizerDiscardEnable(bool rasterizerDiscardEnable);
		void setPolygonMode(VkPolygonMode polygonMode);
		void setCullMode(VkCullModeFlags cullMode);
		void setFrontFace(VkFrontFace frontFace);
		void setDepthBiasEnable(bool depthBiasEnable);
		void setDepthBiasConstantFactor(VulkanF32 depthBiasConstantFactor);
		void setDepthBiasClamp(VulkanF32 depthBiasClamp);
		void setDepthBiasSlopeFactor(VulkanF32 depthBiasSlopeFactor);
		void setLineWidth(VulkanF32 lineWidth);

		void setMultisampleState(VkSampleCountFlagBits rasterizationSamples, bool sampleShadingEnable, VulkanF32 minSampleShading, const VkSampleMask* sampleMask, bool alphaToCoverageEnable, bool alphaToOneEnable);
		void setMultisampleState(const VkPipelineMultisampleStateCreateInfo& multisampleState);
		void setRasterizationSamples(VkSampleCountFlagBits rasterizationSamples);
		void setSampleShadingEnable(bool sampleShadingEnable);
		void setMinSampleShading(VulkanF32 minSampleShading);
		void setSampleMask(const VkSampleMask* sampleMask);
		void setAlphaToCoverageEnable(bool alphaToCoverageEnable);
		void setAlphaToOneEnable(bool alphaToOneEnable);

		void setDepthStencilState(bool depthTestEnable, bool depthWriteEnable, VkCompareOp depthCompareOp, bool depthBoundsTestEnable, bool stencilTestEnable, VkStencilOpState stencilFront, VkStencilOpState stencilBack, VulkanF32 minDepthBounds, VulkanF32 maxDepthBounds);
		void setDepthStencilState(const VkPipelineDepthStencilStateCreateInfo& depthStencilState);
		void setDepthTestEnable(bool depthTestEnable);
		void setDepthWriteEnable(bool depthWriteEnable);
		void setDepthCompareOp(VkCompareOp depthCompareOp);
		void setDepthBoundsTestEnable(bool depthBoundsTestEnable);
		void setStencilTestEnable(bool stencilTestEnable);
		void setStencilFront(VkStencilOpState stencilFront);
		void setStencilBack(VkStencilOpState stencilBack);
		void setMinDepthBounds(VulkanF32 minDepthBounds);
		void setMaxDepthBounds(VulkanF32 maxDepthBounds);

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

		void setBlendState(bool logicOpEnable, VkLogicOp logicOp, VulkanF32 r, VulkanF32 g, VulkanF32 b, VulkanF32 a);
		void setBlendState(const VkPipelineColorBlendStateCreateInfo& blendState);
		void setBlendLogicOpEnable(bool logicOpEnable);
		void setBlendLogicOp(VkLogicOp logicOp);
		void setBlendConstants(VulkanF32 r, VulkanF32 g, VulkanF32 b, VulkanF32 a);

		void addDynamicState(VkDynamicState dynamicState);

		// TODO : Create multiple pipelines at once
		bool create();
		bool destroy();

		bool isCreated() const;
		const VkPipeline& getHandle() const;

	private:
		friend class VulkanDevice;
		VulkanGraphicsPipeline(VulkanDevice& device, VulkanPipelineLayout& layout, VulkanRenderPass& renderPass, VulkanPipelineCache* cache = nullptr);

	private:
		VulkanDevice& mDevice;
		VulkanPipelineLayout& mLayout;
		VulkanRenderPass& mRenderPass;
		VulkanPipelineCache* mCache;
		VkPipeline mGraphicsPipeline;
		VulkanU32 mSubpass;

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

VULKAN_NAMESPACE_END