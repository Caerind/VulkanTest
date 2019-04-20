#include "VulkanGraphicsPipeline.hpp"

#include "VulkanDevice.hpp"
#include "VulkanPipelineLayout.hpp"
#include "VulkanPipelineCache.hpp"
#include "VulkanRenderPass.hpp"
#include "VulkanShaderModule.hpp"

VULKAN_NAMESPACE_BEGIN

VulkanGraphicsPipeline::~VulkanGraphicsPipeline()
{
	destroy();

	VULKAN_OBJECTTRACKER_UNREGISTER();
}

void VulkanGraphicsPipeline::setSubpass(VulkanU32 subpass)
{
	mSubpass = subpass;
}

void VulkanGraphicsPipeline::addShaderModule(VulkanShaderModule* shaderModule)
{
	if (shaderModule != nullptr)
	{
		shaderModule->addShaderStages(mShaderStages);
	}
}

void VulkanGraphicsPipeline::addVertexBinding(VulkanU32 binding, VulkanU32 stride, VkVertexInputRate inputRate)
{
	mVertexBindingDescriptions.push_back({ binding, stride, inputRate });
}

void VulkanGraphicsPipeline::addVertexBinding(const VkVertexInputBindingDescription& description)
{
	mVertexBindingDescriptions.push_back(description);
}

void VulkanGraphicsPipeline::addVertexAttribute(VulkanU32 location, VulkanU32 binding, VkFormat format, VulkanU32 offset)
{
	mVertexAttributeDescriptions.push_back({ location, binding, format, offset });
}

void VulkanGraphicsPipeline::addVertexAttribute(const VkVertexInputAttributeDescription& description)
{
	mVertexAttributeDescriptions.push_back(description);
}

void VulkanGraphicsPipeline::setInputAssemblyState(VkPrimitiveTopology topology, bool primitiveRestartEnable)
{
	ensuresInputAssemblyStateInitialized();

	mInputAssemblyState->topology = topology;
	mInputAssemblyState->primitiveRestartEnable = primitiveRestartEnable;
}

void VulkanGraphicsPipeline::setInputAssemblyState(const VkPipelineInputAssemblyStateCreateInfo& inputAssemblyState)
{
	ensuresInputAssemblyStateInitialized();

	*mInputAssemblyState = inputAssemblyState;
}

void VulkanGraphicsPipeline::setTopology(VkPrimitiveTopology topology)
{
	ensuresInputAssemblyStateInitialized();

	mInputAssemblyState->topology = topology;
}

void VulkanGraphicsPipeline::setPrimitiveRestart(bool primitiveRestartEnable)
{
	ensuresInputAssemblyStateInitialized();

	mInputAssemblyState->primitiveRestartEnable = primitiveRestartEnable;
}

void VulkanGraphicsPipeline::setTessellationState(const VkPipelineTessellationStateCreateInfo& tessellationState)
{
	ensuresTessellationStateInitialized();

	*mTessellationState = tessellationState;
}

void VulkanGraphicsPipeline::setPatchControlPoints(VulkanU32 patchControlPoints)
{
	ensuresTessellationStateInitialized();

	mTessellationState->patchControlPoints = patchControlPoints;
}

void VulkanGraphicsPipeline::setViewport(float x, float y, float width, float height, float minDepth, float maxDepth)
{
	mViewport = { x, y, width, height, minDepth, maxDepth };
}

void VulkanGraphicsPipeline::setViewport(const VkViewport& viewport)
{
	mViewport = viewport;
}

void VulkanGraphicsPipeline::setScissor(VulkanI32 x, VulkanI32 y, VulkanU32 width, VulkanU32 height)
{
	mScissor = { { x, y }, { width, height } };
}

void VulkanGraphicsPipeline::setScissor(const VkRect2D& scissor)
{
	mScissor = scissor;
}

void VulkanGraphicsPipeline::setRasterizationState(bool depthClampEnable, bool rasterizerDiscardEnable, VkPolygonMode polygonMode, VkCullModeFlags cullMode, VkFrontFace frontFace, bool depthBiasEnable, float depthBiasConstantFactor, float depthBiasClamp, float depthBiasSlopeFactor, float lineWidth)
{
	ensuresRasterizationStateInitialized();

	*mRasterizationState = {
		VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO, // VkStructureType                            sType
		nullptr,                                                    // const void                               * pNext
		0,                                                          // VkPipelineRasterizationStateCreateFlags    flags
		depthClampEnable,                                           // Vkbool32                                   depthClampEnable
		rasterizerDiscardEnable,                                    // Vkbool32                                   rasterizerDiscardEnable
		polygonMode,                                                // VkPolygonMode                              polygonMode
		cullMode,                                                   // VkCullModeFlags                            cullMode
		frontFace,                                                  // VkFrontFace                                frontFace
		depthBiasEnable,                                            // Vkbool32                                   depthBiasEnable
		depthBiasConstantFactor,                                    // float                                      depthBiasConstantFactor
		depthBiasClamp,                                             // float                                      depthBiasClamp
		depthBiasSlopeFactor,                                       // float                                      depthBiasSlopeFactor
		lineWidth                                                   // float                                      lineWidth
	};
}

void VulkanGraphicsPipeline::setRasterizationState(const VkPipelineRasterizationStateCreateInfo& rasterizationState)
{
	ensuresRasterizationStateInitialized();

	*mRasterizationState = rasterizationState;
}

void VulkanGraphicsPipeline::setDepthClampEnable(bool depthClampEnable)
{
	ensuresRasterizationStateInitialized();

	mRasterizationState->depthClampEnable = depthClampEnable;
}

void VulkanGraphicsPipeline::setRasterizerDiscardEnable(bool rasterizerDiscardEnable)
{
	ensuresRasterizationStateInitialized();

	mRasterizationState->rasterizerDiscardEnable = rasterizerDiscardEnable;
}

void VulkanGraphicsPipeline::setPolygonMode(VkPolygonMode polygonMode)
{
	ensuresRasterizationStateInitialized();

	mRasterizationState->polygonMode = polygonMode;
}

void VulkanGraphicsPipeline::setCullMode(VkCullModeFlags cullMode)
{
	ensuresRasterizationStateInitialized();

	mRasterizationState->cullMode = cullMode;
}

void VulkanGraphicsPipeline::setFrontFace(VkFrontFace frontFace)
{
	ensuresRasterizationStateInitialized();

	mRasterizationState->frontFace = frontFace;
}

void VulkanGraphicsPipeline::setDepthBiasEnable(bool depthBiasEnable)
{
	ensuresRasterizationStateInitialized();

	mRasterizationState->depthBiasEnable = depthBiasEnable;
}

void VulkanGraphicsPipeline::setDepthBiasConstantFactor(float depthBiasConstantFactor)
{
	ensuresRasterizationStateInitialized();

	mRasterizationState->depthBiasConstantFactor = depthBiasConstantFactor;
}

void VulkanGraphicsPipeline::setDepthBiasClamp(float depthBiasClamp)
{
	ensuresRasterizationStateInitialized();

	mRasterizationState->depthBiasClamp = depthBiasClamp;
}

void VulkanGraphicsPipeline::setDepthBiasSlopeFactor(float depthBiasSlopeFactor)
{
	ensuresRasterizationStateInitialized();

	mRasterizationState->depthBiasSlopeFactor = depthBiasSlopeFactor;
}

void VulkanGraphicsPipeline::setLineWidth(float lineWidth)
{
	ensuresRasterizationStateInitialized();

	mRasterizationState->lineWidth = lineWidth;
}

void VulkanGraphicsPipeline::setMultisampleState(VkSampleCountFlagBits rasterizationSamples, bool sampleShadingEnable, float minSampleShading, const VkSampleMask* sampleMask, bool alphaToCoverageEnable, bool alphaToOneEnable)
{
	ensuresMultisampleStateInitialized();

	*mMultisampleState = {
		VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO, // VkStructureType                          sType
		nullptr,                                                  // const void                             * pNext
		0,                                                        // VkPipelineMultisampleStateCreateFlags    flags
		rasterizationSamples,                                     // VkSampleCountFlagBits                    rasterizationSamples
		sampleShadingEnable,                                      // Vkbool32                                 sampleShadingEnable
		minSampleShading,                                         // float                                    minSampleShading
		sampleMask,                                               // const VkSampleMask                     * pSampleMask
		alphaToCoverageEnable,                                    // Vkbool32                                 alphaToCoverageEnable
		alphaToOneEnable                                          // Vkbool32                                 alphaToOneEnable
	};
}

void VulkanGraphicsPipeline::setMultisampleState(const VkPipelineMultisampleStateCreateInfo& multisampleState)
{
	ensuresMultisampleStateInitialized();

	*mMultisampleState = multisampleState;
}

void VulkanGraphicsPipeline::setRasterizationSamples(VkSampleCountFlagBits rasterizationSamples)
{
	ensuresMultisampleStateInitialized();

	mMultisampleState->rasterizationSamples = rasterizationSamples;
}

void VulkanGraphicsPipeline::setSampleShadingEnable(bool sampleShadingEnable)
{
	ensuresMultisampleStateInitialized();

	mMultisampleState->sampleShadingEnable = sampleShadingEnable;
}

void VulkanGraphicsPipeline::setMinSampleShading(float minSampleShading)
{
	ensuresMultisampleStateInitialized();

	mMultisampleState->minSampleShading = minSampleShading;
}

void VulkanGraphicsPipeline::setSampleMask(const VkSampleMask* sampleMask)
{
	ensuresMultisampleStateInitialized();

	mMultisampleState->pSampleMask = sampleMask;
}

void VulkanGraphicsPipeline::setAlphaToCoverageEnable(bool alphaToCoverageEnable)
{
	ensuresMultisampleStateInitialized();

	mMultisampleState->alphaToCoverageEnable = alphaToCoverageEnable;
}

void VulkanGraphicsPipeline::setAlphaToOneEnable(bool alphaToOneEnable)
{
	ensuresMultisampleStateInitialized();

	mMultisampleState->alphaToOneEnable = alphaToOneEnable;
}

void VulkanGraphicsPipeline::setDepthStencilState(bool depthTestEnable, bool depthWriteEnable, VkCompareOp depthCompareOp, bool depthBoundsTestEnable, bool stencilTestEnable, VkStencilOpState stencilFront, VkStencilOpState stencilBack, float minDepthBounds, float maxDepthBounds)
{
	ensuresDepthStencilStateInitialized();

	*mDepthStencilState = {
		VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO,   // VkStructureType                            sType
		nullptr,                                                      // const void                               * pNext
		0,                                                            // VkPipelineDepthStencilStateCreateFlags     flags
		depthTestEnable,                                              // Vkbool32                                   depthTestEnable
		depthWriteEnable,                                             // Vkbool32                                   depthWriteEnable
		depthCompareOp,                                               // VkCompareOp                                depthCompareOp
		depthBoundsTestEnable,                                        // Vkbool32                                   depthBoundsTestEnable
		stencilTestEnable,                                            // Vkbool32                                   stencilTestEnable
		stencilFront,                                                 // VkStencilOpState                           front
		stencilBack,                                                  // VkStencilOpState                           back
		minDepthBounds,                                               // float                                      minDepthBounds
		maxDepthBounds                                                // float                                      maxDepthBounds
	};
}

void VulkanGraphicsPipeline::setDepthStencilState(const VkPipelineDepthStencilStateCreateInfo& depthStencilState)
{
	ensuresDepthStencilStateInitialized();

	*mDepthStencilState = depthStencilState;
}

void VulkanGraphicsPipeline::setDepthTestEnable(bool depthTestEnable)
{
	ensuresDepthStencilStateInitialized();

	mDepthStencilState->depthTestEnable = depthTestEnable;
}

void VulkanGraphicsPipeline::setDepthWriteEnable(bool depthWriteEnable)
{
	ensuresDepthStencilStateInitialized();

	mDepthStencilState->depthWriteEnable = depthWriteEnable;
}

void VulkanGraphicsPipeline::setDepthCompareOp(VkCompareOp depthCompareOp)
{
	ensuresDepthStencilStateInitialized();

	mDepthStencilState->depthCompareOp = depthCompareOp;
}

void VulkanGraphicsPipeline::setDepthBoundsTestEnable(bool depthBoundsTestEnable)
{
	ensuresDepthStencilStateInitialized();

	mDepthStencilState->depthBoundsTestEnable = depthBoundsTestEnable;
}

void VulkanGraphicsPipeline::setStencilTestEnable(bool stencilTestEnable)
{
	ensuresDepthStencilStateInitialized();

	mDepthStencilState->stencilTestEnable = stencilTestEnable;
}

void VulkanGraphicsPipeline::setStencilFront(VkStencilOpState stencilFront)
{
	ensuresDepthStencilStateInitialized();

	mDepthStencilState->front = stencilFront;
}

void VulkanGraphicsPipeline::setStencilBack(VkStencilOpState stencilBack)
{
	ensuresDepthStencilStateInitialized();

	mDepthStencilState->back = stencilBack;
}

void VulkanGraphicsPipeline::setMinDepthBounds(float minDepthBounds)
{
	ensuresDepthStencilStateInitialized();

	mDepthStencilState->minDepthBounds = minDepthBounds;
}

void VulkanGraphicsPipeline::setMaxDepthBounds(float maxDepthBounds)
{
	ensuresDepthStencilStateInitialized();

	mDepthStencilState->maxDepthBounds = maxDepthBounds;
}

void VulkanGraphicsPipeline::addBlend(bool blendEnable, VkBlendFactor srcColorBlendFactor, VkBlendFactor dstColorBlendFactor, VkBlendOp colorBlendOp, VkBlendFactor srcAlphaBlendFactor, VkBlendFactor dstAlphaBlendFactor, VkBlendOp alphaBlendOp, VkColorComponentFlags colorWriteMask)
{
	VkPipelineColorBlendAttachmentState blend = {
		blendEnable,                    // Vkbool32                 blendEnable
		srcColorBlendFactor,            // VkBlendFactor            srcColorBlendFactor
		dstColorBlendFactor,            // VkBlendFactor            dstColorBlendFactor
		colorBlendOp,                   // VkBlendOp                colorBlendOp
		srcAlphaBlendFactor,            // VkBlendFactor            srcAlphaBlendFactor
		dstAlphaBlendFactor,            // VkBlendFactor            dstAlphaBlendFactor
		alphaBlendOp,                   // VkBlendOp                alphaBlendOp
		colorWriteMask                  // VkColorComponentFlags    colorWriteMask
	};
	mBlendAttachments.push_back(blend);
}

void VulkanGraphicsPipeline::addBlend(const VkPipelineColorBlendAttachmentState& blendAttachment)
{
	mBlendAttachments.push_back(blendAttachment);
}

void VulkanGraphicsPipeline::addBlend(bool blendEnable)
{
	VkPipelineColorBlendAttachmentState blend = {
		blendEnable,                    // Vkbool32                 blendEnable
		VK_BLEND_FACTOR_ONE,            // VkBlendFactor            srcColorBlendFactor
		VK_BLEND_FACTOR_ZERO,           // VkBlendFactor            dstColorBlendFactor
		VK_BLEND_OP_ADD,                // VkBlendOp                colorBlendOp
		VK_BLEND_FACTOR_ONE,            // VkBlendFactor            srcAlphaBlendFactor
		VK_BLEND_FACTOR_ZERO,           // VkBlendFactor            dstAlphaBlendFactor
		VK_BLEND_OP_ADD,                // VkBlendOp                alphaBlendOp
		VK_COLOR_COMPONENT_R_BIT |      // VkColorComponentFlags    colorWriteMask
		VK_COLOR_COMPONENT_G_BIT |
		VK_COLOR_COMPONENT_B_BIT |
		VK_COLOR_COMPONENT_A_BIT
	};
	mBlendAttachments.push_back(blend);
}

void VulkanGraphicsPipeline::setBlendEnable(bool blendEnable)
{
	mBlendAttachments.back().blendEnable = blendEnable;
}

void VulkanGraphicsPipeline::setBlendSrcColorBlendFactor(VkBlendFactor srcColorBlendFactor)
{
	mBlendAttachments.back().srcColorBlendFactor = srcColorBlendFactor;
}

void VulkanGraphicsPipeline::setBlendDstColorBlendFactor(VkBlendFactor dstColorBlendFactor)
{
	mBlendAttachments.back().dstColorBlendFactor = dstColorBlendFactor;
}

void VulkanGraphicsPipeline::setBlendColorBlendOp(VkBlendOp colorBlendOp)
{
	mBlendAttachments.back().colorBlendOp = colorBlendOp;
}

void VulkanGraphicsPipeline::setBlendSrcAlphaBlendFactor(VkBlendFactor srcAlphaBlendFactor)
{
	mBlendAttachments.back().srcAlphaBlendFactor = srcAlphaBlendFactor;
}

void VulkanGraphicsPipeline::setBlendDstAlphaBlendFactor(VkBlendFactor dstAlphaBlendFactor)
{
	mBlendAttachments.back().dstAlphaBlendFactor = dstAlphaBlendFactor;
}

void VulkanGraphicsPipeline::setBlendAlphaBlendOp(VkBlendOp alphaBlendOp)
{
	mBlendAttachments.back().alphaBlendOp = alphaBlendOp;
}

void VulkanGraphicsPipeline::setBlendColorWriteMask(VkColorComponentFlags colorWriteMask)
{
	mBlendAttachments.back().colorWriteMask = colorWriteMask;
}

void VulkanGraphicsPipeline::setBlendState(bool logicOpEnable, VkLogicOp logicOp, float r, float g, float b, float a)
{
	mBlendState = {
		VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO,   // VkStructureType                              sType
		nullptr,                                                    // const void                                 * pNext
		0,                                                          // VkPipelineColorBlendStateCreateFlags         flags
		logicOpEnable,                                              // Vkbool32                                     logicOpEnable
		logicOp,                                                    // VkLogicOp                                    logicOp
		0,                                                          // VulkanU32                                     attachmentCount
		nullptr,                                                    // const VkPipelineColorBlendAttachmentState  * pAttachments
		{                                                           // float                                        blendConstants[4]
			r,
			g,
			b,
			a
		}
	};
}

void VulkanGraphicsPipeline::setBlendState(const VkPipelineColorBlendStateCreateInfo& blendState)
{
	mBlendState = blendState;
}

void VulkanGraphicsPipeline::setBlendLogicOpEnable(bool logicOpEnable)
{
	mBlendState.logicOpEnable = logicOpEnable;
}

void VulkanGraphicsPipeline::setBlendLogicOp(VkLogicOp logicOp)
{
	mBlendState.logicOp = logicOp;
}

void VulkanGraphicsPipeline::setBlendConstants(float r, float g, float b, float a)
{
	mBlendState.blendConstants[0] = r;
	mBlendState.blendConstants[1] = g;
	mBlendState.blendConstants[2] = b;
	mBlendState.blendConstants[3] = a;
}

void VulkanGraphicsPipeline::addDynamicState(VkDynamicState dynamicState)
{
	mDynamicStates.push_back(dynamicState);
}

bool VulkanGraphicsPipeline::create()
{
	VkPipelineVertexInputStateCreateInfo vertexInputState = {
		VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO,    // VkStructureType                           sType
		nullptr,                                                      // const void                              * pNext
		0,                                                            // VkPipelineVertexInputStateCreateFlags     flags
		static_cast<VulkanU32>(mVertexBindingDescriptions.size()),     // VulkanU32                                  vertexBindingDescriptionCount
		mVertexBindingDescriptions.data(),                            // const VkVertexInputBindingDescription   * pVertexBindingDescriptions
		static_cast<VulkanU32>(mVertexAttributeDescriptions.size()),   // VulkanU32                                  vertexAttributeDescriptionCount
		mVertexAttributeDescriptions.data()                           // const VkVertexInputAttributeDescription * pVertexAttributeDescriptions
	};

	VkPipelineViewportStateCreateInfo viewportState = {
		VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO,    // VkStructureType                      sType
		nullptr,                                                  // const void                         * pNext
		0,                                                        // VkPipelineViewportStateCreateFlags   flags
		1,                                                        // VulkanU32                             viewportCount
		&mViewport,                                               // const VkViewport                   * pViewports
		1,                                                        // VulkanU32                             scissorCount
		&mScissor                                                 // const VkRect2D                     * pScissors
	};

	VkPipelineDynamicStateCreateInfo dynamicStates = {
		VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO,     // VkStructureType                      sType
		nullptr,                                                  // const void                         * pNext
		0,                                                        // VkPipelineDynamicStateCreateFlags    flags
		static_cast<VulkanU32>(mDynamicStates.size()),             // VulkanU32                             dynamicStateCount
		mDynamicStates.data()                                     // const VkDynamicState               * pDynamicStates
	};

	if (mBlendAttachments.empty())
	{
		VkPipelineColorBlendAttachmentState blend = {
			false,                          // Vkbool32                 blendEnable
			VK_BLEND_FACTOR_ONE,            // VkBlendFactor            srcColorBlendFactor
			VK_BLEND_FACTOR_ZERO,           // VkBlendFactor            dstColorBlendFactor
			VK_BLEND_OP_ADD,                // VkBlendOp                colorBlendOp
			VK_BLEND_FACTOR_ONE,            // VkBlendFactor            srcAlphaBlendFactor
			VK_BLEND_FACTOR_ZERO,           // VkBlendFactor            dstAlphaBlendFactor
			VK_BLEND_OP_ADD,                // VkBlendOp                alphaBlendOp
			VK_COLOR_COMPONENT_R_BIT |      // VkColorComponentFlags    colorWriteMask
			VK_COLOR_COMPONENT_G_BIT |
			VK_COLOR_COMPONENT_B_BIT |
			VK_COLOR_COMPONENT_A_BIT
		};
		mBlendAttachments.push_back(blend);
	}
	auto count = (VulkanU32)mBlendAttachments.size();
	mBlendState.attachmentCount = count;
	mBlendState.pAttachments = count ? mBlendAttachments.data() : nullptr;

	VkGraphicsPipelineCreateInfo pipelineCreateInfo = {
		VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO,            // VkStructureType                                sType
		nullptr,                                                    // const void                                   * pNext
		0,                                                          // VkPipelineCreateFlags                          flags
		static_cast<VulkanU32>(mShaderStages.size()),                // VulkanU32                                       stageCount
		mShaderStages.data(),                                       // const VkPipelineShaderStageCreateInfo        * pStages
		&vertexInputState,                                          // const VkPipelineVertexInputStateCreateInfo   * pVertexInputState
		mInputAssemblyState ? mInputAssemblyState.get() : &sDefaultInputAssemblyState,  // const VkPipelineInputAssemblyStateCreateInfo * pInputAssemblyState
		mTessellationState ? mTessellationState.get() : nullptr,    // const VkPipelineTessellationStateCreateInfo  * pTessellationState
		&viewportState,                                             // const VkPipelineViewportStateCreateInfo      * pViewportState
		mRasterizationState ? mRasterizationState.get() : &sDefaultRasterizationState,  // const VkPipelineRasterizationStateCreateInfo * pRasterizationState
		mMultisampleState ? mMultisampleState.get() : &sDefaultMultisampleState,        // const VkPipelineMultisampleStateCreateInfo   * pMultisampleState
		mDepthStencilState ? mDepthStencilState.get() : &sDefaultDepthStencilState,     // const VkPipelineDepthStencilStateCreateInfo  * pDepthStencilState
		&mBlendState,                                               // const VkPipelineColorBlendStateCreateInfo    * pColorBlendState
		mDynamicStates.empty() ? nullptr : &dynamicStates,          // const VkPipelineDynamicStateCreateInfo       * pDynamicState
		mLayout.getHandle(),                                        // VkPipelineLayout                               layout
		mRenderPass.getHandle(),                                    // VkRenderPass                                   renderPass
		mSubpass,                                                   // VulkanU32                                       subpass
		VK_NULL_HANDLE,                                             // VkPipeline                                     basePipelineHandle
		-1                                                          // VulkanI32                                        basePipelineIndex
	};

	VkPipelineCache cacheHandle = (mCache != nullptr) ? mCache->getHandle() : VK_NULL_HANDLE;

	VkResult result = vkCreateGraphicsPipelines(mDevice.getHandle(), cacheHandle, 1, &pipelineCreateInfo, nullptr, &mGraphicsPipeline);
	if (result != VK_SUCCESS || mGraphicsPipeline == VK_NULL_HANDLE)
	{
		VULKAN_LOG_ERROR("Could not create a graphics pipeline\n");
		return false;
	}

	return true;
}

bool VulkanGraphicsPipeline::destroy()
{
	if (mGraphicsPipeline != VK_NULL_HANDLE)
	{
		vkDestroyPipeline(mDevice.getHandle(), mGraphicsPipeline, nullptr);
		mGraphicsPipeline = VK_NULL_HANDLE;
		return true;
	}
	return false;
}

bool VulkanGraphicsPipeline::isCreated() const
{
	return mGraphicsPipeline != VK_NULL_HANDLE;
}

const VkPipeline& VulkanGraphicsPipeline::getHandle() const
{
	return mGraphicsPipeline;
}

VulkanGraphicsPipeline::VulkanGraphicsPipeline(VulkanDevice& device, VulkanPipelineLayout& layout, VulkanRenderPass& renderPass, VulkanPipelineCache* cache)
	: mDevice(device)
	, mLayout(layout)
	, mRenderPass(renderPass)
	, mCache(cache)
	, mGraphicsPipeline(VK_NULL_HANDLE)
	, mSubpass(0)
	, mShaderStages()
	, mVertexBindingDescriptions()
	, mVertexAttributeDescriptions()
	, mInputAssemblyState(nullptr)
	, mTessellationState(nullptr)
	, mViewport()
	, mScissor()
	, mRasterizationState(nullptr)
	, mMultisampleState(nullptr)
	, mDepthStencilState(nullptr)
	, mBlendAttachments()
	, mBlendState()
	, mDynamicStates()
{
	VULKAN_OBJECTTRACKER_REGISTER();

	mBlendState = {
		VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO,   // VkStructureType                              sType
		nullptr,                                                    // const void                                 * pNext
		0,                                                          // VkPipelineColorBlendStateCreateFlags         flags
		false,                                                      // Vkbool32                                     logicOpEnable
		VK_LOGIC_OP_COPY,                                           // VkLogicOp                                    logicOp
		0,                                                          // VulkanU32                                     attachmentCount
		nullptr,                                                    // const VkPipelineColorBlendAttachmentState  * pAttachments
		{                                                           // float                                        blendConstants[4]
			1.0f,
			1.0f,
			1.0f,
			1.0f
		}
	};
}

void VulkanGraphicsPipeline::ensuresInputAssemblyStateInitialized()
{
	if (mInputAssemblyState == nullptr)
	{
		mInputAssemblyState = std::unique_ptr<VkPipelineInputAssemblyStateCreateInfo>(new VkPipelineInputAssemblyStateCreateInfo());

		mInputAssemblyState->sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO; // VkStructureType                           sType
		mInputAssemblyState->pNext = nullptr;                                                     // const void                              * pNext
		mInputAssemblyState->flags = 0;                                                           // VkPipelineInputAssemblyStateCreateFlags   flags
		mInputAssemblyState->topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;    // VkPrimitiveTopology                       topology
		mInputAssemblyState->primitiveRestartEnable = false;                    // Vkbool32                                  primitiveRestartEnable
	}
}

void VulkanGraphicsPipeline::ensuresTessellationStateInitialized()
{
	if (mTessellationState == nullptr)
	{
		mTessellationState = std::unique_ptr<VkPipelineTessellationStateCreateInfo>(new VkPipelineTessellationStateCreateInfo());
		
		*mTessellationState = {
			VK_STRUCTURE_TYPE_PIPELINE_TESSELLATION_STATE_CREATE_INFO, // VkStructureType                            sType
			nullptr,                                                   // const void                               * pNext
			0,                                                         // VkPipelineTessellationStateCreateFlags     flags
			0                                                          // VulkanU32                                   patchControlPoints
		};
	}
}

void VulkanGraphicsPipeline::ensuresRasterizationStateInitialized()
{
	if (mRasterizationState == nullptr)
	{
		mRasterizationState = std::unique_ptr<VkPipelineRasterizationStateCreateInfo>(new VkPipelineRasterizationStateCreateInfo());

		*mRasterizationState = sDefaultRasterizationState;
	}
}

void VulkanGraphicsPipeline::ensuresMultisampleStateInitialized()
{
	if (mMultisampleState == nullptr)
	{
		mMultisampleState = std::unique_ptr<VkPipelineMultisampleStateCreateInfo>(new VkPipelineMultisampleStateCreateInfo());

		*mMultisampleState = sDefaultMultisampleState;
	}
}

void VulkanGraphicsPipeline::ensuresDepthStencilStateInitialized()
{
	if (mDepthStencilState == nullptr)
	{
		mDepthStencilState = std::unique_ptr<VkPipelineDepthStencilStateCreateInfo>(new VkPipelineDepthStencilStateCreateInfo());
		
		*mDepthStencilState = sDefaultDepthStencilState;
	}
}

const VkPipelineInputAssemblyStateCreateInfo VulkanGraphicsPipeline::sDefaultInputAssemblyState = {
	VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO,  // VkStructureType                            sType
	nullptr,                                                      // const void                               * pNext
	0,                                                            // VkPipelineTessellationStateCreateFlags     flags
	VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST,                          // VkPrimitiveTopology                       topology
	false                                                         // Vkbool32                                  primitiveRestartEnable
};

const VkPipelineRasterizationStateCreateInfo VulkanGraphicsPipeline::sDefaultRasterizationState = {
	VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO, // VkStructureType                            sType
	nullptr,                                                    // const void                               * pNext
	0,                                                          // VkPipelineRasterizationStateCreateFlags    flags
	false,                                                      // Vkbool32                                   depthClampEnable
	false,                                                      // Vkbool32                                   rasterizerDiscardEnable
	VK_POLYGON_MODE_FILL,                                       // VkPolygonMode                              polygonMode
	VK_CULL_MODE_BACK_BIT,                                      // VkCullModeFlags                            cullMode
	VK_FRONT_FACE_COUNTER_CLOCKWISE,                            // VkFrontFace                                frontFace
	false,                                                      // Vkbool32                                   depthBiasEnable
	0.0f,                                                       // float                                      depthBiasConstantFactor
	0.0f,                                                       // float                                      depthBiasClamp
	0.0f,                                                       // float                                      depthBiasSlopeFactor
	1.0f                                                        // float                                      lineWidth
};

const VkPipelineMultisampleStateCreateInfo VulkanGraphicsPipeline::sDefaultMultisampleState = {
	VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO, // VkStructureType                          sType
	nullptr,                                                  // const void                             * pNext
	0,                                                        // VkPipelineMultisampleStateCreateFlags    flags
	VK_SAMPLE_COUNT_1_BIT,                                    // VkSampleCountFlagBits                    rasterizationSamples
	false,                                                    // Vkbool32                                 sampleShadingEnable
	0.0f,                                                     // float                                    minSampleShading
	nullptr,                                                  // const VkSampleMask                     * pSampleMask
	false,                                                    // Vkbool32                                 alphaToCoverageEnable
	false                                                     // Vkbool32                                 alphaToOneEnable
};

const VkPipelineDepthStencilStateCreateInfo VulkanGraphicsPipeline::sDefaultDepthStencilState = {
	VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO,   // VkStructureType                            sType
	nullptr,                                                      // const void                               * pNext
	0,                                                            // VkPipelineDepthStencilStateCreateFlags     flags
	true,                                                         // Vkbool32                                   depthTestEnable
	true,                                                         // Vkbool32                                   depthWriteEnable
	VK_COMPARE_OP_LESS_OR_EQUAL,                                  // VkCompareOp                                depthCompareOp
	false,                                                        // Vkbool32                                   depthBoundsTestEnable
	false,                                                        // Vkbool32                                   stencilTestEnable
	{},                                                           // VkStencilOpState                           front
	{},                                                           // VkStencilOpState                           back
	0.0f,                                                         // float                                      minDepthBounds
	1.0f                                                          // float                                      maxDepthBounds
};

VULKAN_NAMESPACE_END