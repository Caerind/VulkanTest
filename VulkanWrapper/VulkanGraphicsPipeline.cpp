#include "VulkanGraphicsPipeline.hpp"

#include "VulkanDevice.hpp"

namespace nu
{
namespace Vulkan
{

GraphicsPipeline::~GraphicsPipeline()
{
	destroy();

	ObjectTracker::unregisterObject(ObjectType_GraphicsPipeline);
}

void GraphicsPipeline::setSubpass(uint32_t subpass)
{
	mSubpass = subpass;
}

void GraphicsPipeline::addShaderModule(ShaderModule* shaderModule)
{
	if (shaderModule != nullptr)
	{
		shaderModule->addShaderStages(mShaderStages);
	}
}

void GraphicsPipeline::addVertexBinding(uint32_t binding, uint32_t stride, VkVertexInputRate inputRate)
{
	mVertexBindingDescriptions.push_back({ binding, stride, inputRate });
}

void GraphicsPipeline::addVertexBinding(const VkVertexInputBindingDescription& description)
{
	mVertexBindingDescriptions.push_back(description);
}

void GraphicsPipeline::addVertexAttribute(uint32_t location, uint32_t binding, VkFormat format, uint32_t offset)
{
	mVertexAttributeDescriptions.push_back({ location, binding, format, offset });
}

void GraphicsPipeline::addVertexAttribute(const VkVertexInputAttributeDescription& description)
{
	mVertexAttributeDescriptions.push_back(description);
}

void GraphicsPipeline::setInputAssemblyState(VkPrimitiveTopology topology, bool primitiveRestartEnable)
{
	ensuresInputAssemblyStateInitialized();

	mInputAssemblyState->topology = topology;
	mInputAssemblyState->primitiveRestartEnable = primitiveRestartEnable;
}

void GraphicsPipeline::setInputAssemblyState(const VkPipelineInputAssemblyStateCreateInfo& inputAssemblyState)
{
	ensuresInputAssemblyStateInitialized();

	*mInputAssemblyState = inputAssemblyState;
}

void GraphicsPipeline::setTopology(VkPrimitiveTopology topology)
{
	ensuresInputAssemblyStateInitialized();

	mInputAssemblyState->topology = topology;
}

void GraphicsPipeline::setPrimitiveRestart(bool primitiveRestartEnable)
{
	ensuresInputAssemblyStateInitialized();

	mInputAssemblyState->primitiveRestartEnable = primitiveRestartEnable;
}

void GraphicsPipeline::setTessellationState(const VkPipelineTessellationStateCreateInfo& tessellationState)
{
	ensuresTessellationStateInitialized();

	*mTessellationState = tessellationState;
}

void GraphicsPipeline::setPatchControlPoints(uint32_t patchControlPoints)
{
	ensuresTessellationStateInitialized();

	mTessellationState->patchControlPoints = patchControlPoints;
}

void GraphicsPipeline::setViewport(float x, float y, float width, float height, float minDepth, float maxDepth)
{
	mViewport = { x, y, width, height, minDepth, maxDepth };
}

void GraphicsPipeline::setViewport(const VkViewport& viewport)
{
	mViewport = viewport;
}

void GraphicsPipeline::setScissor(int32_t x, int32_t y, uint32_t width, uint32_t height)
{
	mScissor = { { x, y }, { width, height } };
}

void GraphicsPipeline::setScissor(const VkRect2D& scissor)
{
	mScissor = scissor;
}

void GraphicsPipeline::setRasterizationState(bool depthClampEnable, bool rasterizerDiscardEnable, VkPolygonMode polygonMode, VkCullModeFlags cullMode, VkFrontFace frontFace, bool depthBiasEnable, float depthBiasConstantFactor, float depthBiasClamp, float depthBiasSlopeFactor, float lineWidth)
{
	ensuresRasterizationStateInitialized();

	*mRasterizationState = {
		VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO, // VkStructureType                            sType
		nullptr,                                                    // const void                               * pNext
		0,                                                          // VkPipelineRasterizationStateCreateFlags    flags
		depthClampEnable,                                           // VkBool32                                   depthClampEnable
		rasterizerDiscardEnable,                                    // VkBool32                                   rasterizerDiscardEnable
		polygonMode,                                                // VkPolygonMode                              polygonMode
		cullMode,                                                   // VkCullModeFlags                            cullMode
		frontFace,                                                  // VkFrontFace                                frontFace
		depthBiasEnable,                                            // VkBool32                                   depthBiasEnable
		depthBiasConstantFactor,                                    // float                                      depthBiasConstantFactor
		depthBiasClamp,                                             // float                                      depthBiasClamp
		depthBiasSlopeFactor,                                       // float                                      depthBiasSlopeFactor
		lineWidth                                                   // float                                      lineWidth
	};
}

void GraphicsPipeline::setRasterizationState(const VkPipelineRasterizationStateCreateInfo& rasterizationState)
{
	ensuresRasterizationStateInitialized();

	*mRasterizationState = rasterizationState;
}

void GraphicsPipeline::setDepthClampEnable(bool depthClampEnable)
{
	ensuresRasterizationStateInitialized();

	mRasterizationState->depthClampEnable = depthClampEnable;
}

void GraphicsPipeline::setRasterizerDiscardEnable(bool rasterizerDiscardEnable)
{
	ensuresRasterizationStateInitialized();

	mRasterizationState->rasterizerDiscardEnable = rasterizerDiscardEnable;
}

void GraphicsPipeline::setPolygonMode(VkPolygonMode polygonMode)
{
	ensuresRasterizationStateInitialized();

	mRasterizationState->polygonMode = polygonMode;
}

void GraphicsPipeline::setCullMode(VkCullModeFlags cullMode)
{
	ensuresRasterizationStateInitialized();

	mRasterizationState->cullMode = cullMode;
}

void GraphicsPipeline::setFrontFace(VkFrontFace frontFace)
{
	ensuresRasterizationStateInitialized();

	mRasterizationState->frontFace = frontFace;
}

void GraphicsPipeline::setDepthBiasEnable(bool depthBiasEnable)
{
	ensuresRasterizationStateInitialized();

	mRasterizationState->depthBiasEnable = depthBiasEnable;
}

void GraphicsPipeline::setDepthBiasConstantFactor(float depthBiasConstantFactor)
{
	ensuresRasterizationStateInitialized();

	mRasterizationState->depthBiasConstantFactor = depthBiasConstantFactor;
}

void GraphicsPipeline::setDepthBiasClamp(float depthBiasClamp)
{
	ensuresRasterizationStateInitialized();

	mRasterizationState->depthBiasClamp = depthBiasClamp;
}

void GraphicsPipeline::setDepthBiasSlopeFactor(float depthBiasSlopeFactor)
{
	ensuresRasterizationStateInitialized();

	mRasterizationState->depthBiasSlopeFactor = depthBiasSlopeFactor;
}

void GraphicsPipeline::setLineWidth(float lineWidth)
{
	ensuresRasterizationStateInitialized();

	mRasterizationState->lineWidth = lineWidth;
}

void GraphicsPipeline::setMultisampleState(VkSampleCountFlagBits rasterizationSamples, bool sampleShadingEnable, float minSampleShading, const VkSampleMask* sampleMask, bool alphaToCoverageEnable, bool alphaToOneEnable)
{
	ensuresMultisampleStateInitialized();

	*mMultisampleState = {
		VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO, // VkStructureType                          sType
		nullptr,                                                  // const void                             * pNext
		0,                                                        // VkPipelineMultisampleStateCreateFlags    flags
		rasterizationSamples,                                     // VkSampleCountFlagBits                    rasterizationSamples
		sampleShadingEnable,                                      // VkBool32                                 sampleShadingEnable
		minSampleShading,                                         // float                                    minSampleShading
		sampleMask,                                               // const VkSampleMask                     * pSampleMask
		alphaToCoverageEnable,                                    // VkBool32                                 alphaToCoverageEnable
		alphaToOneEnable                                          // VkBool32                                 alphaToOneEnable
	};
}

void GraphicsPipeline::setMultisampleState(const VkPipelineMultisampleStateCreateInfo& multisampleState)
{
	ensuresMultisampleStateInitialized();

	*mMultisampleState = multisampleState;
}

void GraphicsPipeline::setRasterizationSamples(VkSampleCountFlagBits rasterizationSamples)
{
	ensuresMultisampleStateInitialized();

	mMultisampleState->rasterizationSamples = rasterizationSamples;
}

void GraphicsPipeline::setSampleShadingEnable(bool sampleShadingEnable)
{
	ensuresMultisampleStateInitialized();

	mMultisampleState->sampleShadingEnable = sampleShadingEnable;
}

void GraphicsPipeline::setMinSampleShading(float minSampleShading)
{
	ensuresMultisampleStateInitialized();

	mMultisampleState->minSampleShading = minSampleShading;
}

void GraphicsPipeline::setSampleMask(const VkSampleMask* sampleMask)
{
	ensuresMultisampleStateInitialized();

	mMultisampleState->pSampleMask = sampleMask;
}

void GraphicsPipeline::setAlphaToCoverageEnable(bool alphaToCoverageEnable)
{
	ensuresMultisampleStateInitialized();

	mMultisampleState->alphaToCoverageEnable = alphaToCoverageEnable;
}

void GraphicsPipeline::setAlphaToOneEnable(bool alphaToOneEnable)
{
	ensuresMultisampleStateInitialized();

	mMultisampleState->alphaToOneEnable = alphaToOneEnable;
}

void GraphicsPipeline::setDepthStencilState(bool depthTestEnable, bool depthWriteEnable, VkCompareOp depthCompareOp, bool depthBoundsTestEnable, bool stencilTestEnable, VkStencilOpState stencilFront, VkStencilOpState stencilBack, float minDepthBounds, float maxDepthBounds)
{
	ensuresDepthStencilStateInitialized();

	*mDepthStencilState = {
		VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO,   // VkStructureType                            sType
		nullptr,                                                      // const void                               * pNext
		0,                                                            // VkPipelineDepthStencilStateCreateFlags     flags
		depthTestEnable,                                              // VkBool32                                   depthTestEnable
		depthWriteEnable,                                             // VkBool32                                   depthWriteEnable
		depthCompareOp,                                               // VkCompareOp                                depthCompareOp
		depthBoundsTestEnable,                                        // VkBool32                                   depthBoundsTestEnable
		stencilTestEnable,                                            // VkBool32                                   stencilTestEnable
		stencilFront,                                                 // VkStencilOpState                           front
		stencilBack,                                                  // VkStencilOpState                           back
		minDepthBounds,                                               // float                                      minDepthBounds
		maxDepthBounds                                                // float                                      maxDepthBounds
	};
}

void GraphicsPipeline::setDepthStencilState(const VkPipelineDepthStencilStateCreateInfo& depthStencilState)
{
	ensuresDepthStencilStateInitialized();

	*mDepthStencilState = depthStencilState;
}

void GraphicsPipeline::setDepthTestEnable(bool depthTestEnable)
{
	ensuresDepthStencilStateInitialized();

	mDepthStencilState->depthTestEnable = depthTestEnable;
}

void GraphicsPipeline::setDepthWriteEnable(bool depthWriteEnable)
{
	ensuresDepthStencilStateInitialized();

	mDepthStencilState->depthWriteEnable = depthWriteEnable;
}

void GraphicsPipeline::setDepthCompareOp(VkCompareOp depthCompareOp)
{
	ensuresDepthStencilStateInitialized();

	mDepthStencilState->depthCompareOp = depthCompareOp;
}

void GraphicsPipeline::setDepthBoundsTestEnable(bool depthBoundsTestEnable)
{
	ensuresDepthStencilStateInitialized();

	mDepthStencilState->depthBoundsTestEnable = depthBoundsTestEnable;
}

void GraphicsPipeline::setStencilTestEnable(bool stencilTestEnable)
{
	ensuresDepthStencilStateInitialized();

	mDepthStencilState->stencilTestEnable = stencilTestEnable;
}

void GraphicsPipeline::setStencilFront(VkStencilOpState stencilFront)
{
	ensuresDepthStencilStateInitialized();

	mDepthStencilState->front = stencilFront;
}

void GraphicsPipeline::setStencilBack(VkStencilOpState stencilBack)
{
	ensuresDepthStencilStateInitialized();

	mDepthStencilState->back = stencilBack;
}

void GraphicsPipeline::setMinDepthBounds(float minDepthBounds)
{
	ensuresDepthStencilStateInitialized();

	mDepthStencilState->minDepthBounds = minDepthBounds;
}

void GraphicsPipeline::setMaxDepthBounds(float maxDepthBounds)
{
	ensuresDepthStencilStateInitialized();

	mDepthStencilState->maxDepthBounds = maxDepthBounds;
}

void GraphicsPipeline::addBlend(bool blendEnable, VkBlendFactor srcColorBlendFactor, VkBlendFactor dstColorBlendFactor, VkBlendOp colorBlendOp, VkBlendFactor srcAlphaBlendFactor, VkBlendFactor dstAlphaBlendFactor, VkBlendOp alphaBlendOp, VkColorComponentFlags colorWriteMask)
{
	VkPipelineColorBlendAttachmentState blend = {
		blendEnable,                    // VkBool32                 blendEnable
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

void GraphicsPipeline::addBlend(const VkPipelineColorBlendAttachmentState& blendAttachment)
{
	mBlendAttachments.push_back(blendAttachment);
}

void GraphicsPipeline::addBlend(bool blendEnable)
{
	VkPipelineColorBlendAttachmentState blend = {
		blendEnable,                    // VkBool32                 blendEnable
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

void GraphicsPipeline::setBlendEnable(bool blendEnable)
{
	mBlendAttachments.back().blendEnable = blendEnable;
}

void GraphicsPipeline::setBlendSrcColorBlendFactor(VkBlendFactor srcColorBlendFactor)
{
	mBlendAttachments.back().srcColorBlendFactor = srcColorBlendFactor;
}

void GraphicsPipeline::setBlendDstColorBlendFactor(VkBlendFactor dstColorBlendFactor)
{
	mBlendAttachments.back().dstColorBlendFactor = dstColorBlendFactor;
}

void GraphicsPipeline::setBlendColorBlendOp(VkBlendOp colorBlendOp)
{
	mBlendAttachments.back().colorBlendOp = colorBlendOp;
}

void GraphicsPipeline::setBlendSrcAlphaBlendFactor(VkBlendFactor srcAlphaBlendFactor)
{
	mBlendAttachments.back().srcAlphaBlendFactor = srcAlphaBlendFactor;
}

void GraphicsPipeline::setBlendDstAlphaBlendFactor(VkBlendFactor dstAlphaBlendFactor)
{
	mBlendAttachments.back().dstAlphaBlendFactor = dstAlphaBlendFactor;
}

void GraphicsPipeline::setBlendAlphaBlendOp(VkBlendOp alphaBlendOp)
{
	mBlendAttachments.back().alphaBlendOp = alphaBlendOp;
}

void GraphicsPipeline::setBlendColorWriteMask(VkColorComponentFlags colorWriteMask)
{
	mBlendAttachments.back().colorWriteMask = colorWriteMask;
}

void GraphicsPipeline::setBlendState(bool logicOpEnable, VkLogicOp logicOp, float r, float g, float b, float a)
{
	mBlendState = {
		VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO,   // VkStructureType                              sType
		nullptr,                                                    // const void                                 * pNext
		0,                                                          // VkPipelineColorBlendStateCreateFlags         flags
		logicOpEnable,                                              // VkBool32                                     logicOpEnable
		logicOp,                                                    // VkLogicOp                                    logicOp
		0,                                                          // uint32_t                                     attachmentCount
		nullptr,                                                    // const VkPipelineColorBlendAttachmentState  * pAttachments
		{                                                           // float                                        blendConstants[4]
			r,
			g,
			b,
			a
		}
	};
}

void GraphicsPipeline::setBlendState(const VkPipelineColorBlendStateCreateInfo& blendState)
{
	mBlendState = blendState;
}

void GraphicsPipeline::setBlendLogicOpEnable(bool logicOpEnable)
{
	mBlendState.logicOpEnable = logicOpEnable;
}

void GraphicsPipeline::setBlendLogicOp(VkLogicOp logicOp)
{
	mBlendState.logicOp = logicOp;
}

void GraphicsPipeline::setBlendConstants(float r, float g, float b, float a)
{
	mBlendState.blendConstants[0] = r;
	mBlendState.blendConstants[1] = g;
	mBlendState.blendConstants[2] = b;
	mBlendState.blendConstants[3] = a;
}

void GraphicsPipeline::addDynamicState(VkDynamicState dynamicState)
{
	mDynamicStates.push_back(dynamicState);
}

bool GraphicsPipeline::create()
{
	VkPipelineVertexInputStateCreateInfo vertexInputState = {
		VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO,    // VkStructureType                           sType
		nullptr,                                                      // const void                              * pNext
		0,                                                            // VkPipelineVertexInputStateCreateFlags     flags
		static_cast<uint32_t>(mVertexBindingDescriptions.size()),     // uint32_t                                  vertexBindingDescriptionCount
		mVertexBindingDescriptions.data(),                            // const VkVertexInputBindingDescription   * pVertexBindingDescriptions
		static_cast<uint32_t>(mVertexAttributeDescriptions.size()),   // uint32_t                                  vertexAttributeDescriptionCount
		mVertexAttributeDescriptions.data()                           // const VkVertexInputAttributeDescription * pVertexAttributeDescriptions
	};

	VkPipelineViewportStateCreateInfo viewportState = {
		VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO,    // VkStructureType                      sType
		nullptr,                                                  // const void                         * pNext
		0,                                                        // VkPipelineViewportStateCreateFlags   flags
		1,                                                        // uint32_t                             viewportCount
		&mViewport,                                               // const VkViewport                   * pViewports
		1,                                                        // uint32_t                             scissorCount
		&mScissor                                                 // const VkRect2D                     * pScissors
	};

	VkPipelineDynamicStateCreateInfo dynamicStates = {
		VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO,     // VkStructureType                      sType
		nullptr,                                                  // const void                         * pNext
		0,                                                        // VkPipelineDynamicStateCreateFlags    flags
		static_cast<uint32_t>(mDynamicStates.size()),             // uint32_t                             dynamicStateCount
		mDynamicStates.data()                                     // const VkDynamicState               * pDynamicStates
	};

	if (mBlendAttachments.empty())
	{
		VkPipelineColorBlendAttachmentState blend = {
			false,                          // VkBool32                 blendEnable
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
	auto count = (uint32_t)mBlendAttachments.size();
	mBlendState.attachmentCount = count;
	mBlendState.pAttachments = count ? mBlendAttachments.data() : nullptr;

	VkGraphicsPipelineCreateInfo pipelineCreateInfo = {
		VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO,            // VkStructureType                                sType
		nullptr,                                                    // const void                                   * pNext
		0,                                                          // VkPipelineCreateFlags                          flags
		static_cast<uint32_t>(mShaderStages.size()),                // uint32_t                                       stageCount
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
		mSubpass,                                                   // uint32_t                                       subpass
		VK_NULL_HANDLE,                                             // VkPipeline                                     basePipelineHandle
		-1                                                          // int32_t                                        basePipelineIndex
	};

	VkPipelineCache cacheHandle = (mCache != nullptr) ? mCache->getHandle() : VK_NULL_HANDLE;

	VkResult result = vkCreateGraphicsPipelines(mDevice.getHandle(), cacheHandle, 1, &pipelineCreateInfo, nullptr, &mGraphicsPipeline);
	if (result != VK_SUCCESS || mGraphicsPipeline == VK_NULL_HANDLE)
	{
		// TODO : Use Numea System Log
		printf("Could not create a graphics pipeline\n");
		return false;
	}

	return true;
}

bool GraphicsPipeline::destroy()
{
	if (mGraphicsPipeline != VK_NULL_HANDLE)
	{
		vkDestroyPipeline(mDevice.getHandle(), mGraphicsPipeline, nullptr);
		mGraphicsPipeline = VK_NULL_HANDLE;
		return true;
	}
	return false;
}

bool GraphicsPipeline::isCreated() const
{
	return mGraphicsPipeline != VK_NULL_HANDLE;
}

const VkPipeline& GraphicsPipeline::getHandle() const
{
	return mGraphicsPipeline;
}

GraphicsPipeline::GraphicsPipeline(Device& device, PipelineLayout& layout, RenderPass& renderPass, PipelineCache* cache)
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
	ObjectTracker::registerObject(ObjectType_GraphicsPipeline);

	mBlendState = {
		VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO,   // VkStructureType                              sType
		nullptr,                                                    // const void                                 * pNext
		0,                                                          // VkPipelineColorBlendStateCreateFlags         flags
		false,                                                      // VkBool32                                     logicOpEnable
		VK_LOGIC_OP_COPY,                                           // VkLogicOp                                    logicOp
		0,                                                          // uint32_t                                     attachmentCount
		nullptr,                                                    // const VkPipelineColorBlendAttachmentState  * pAttachments
		{                                                           // float                                        blendConstants[4]
			1.0f,
			1.0f,
			1.0f,
			1.0f
		}
	};
}

void GraphicsPipeline::ensuresInputAssemblyStateInitialized()
{
	if (mInputAssemblyState == nullptr)
	{
		mInputAssemblyState = std::unique_ptr<VkPipelineInputAssemblyStateCreateInfo>(new VkPipelineInputAssemblyStateCreateInfo());

		mInputAssemblyState->sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO; // VkStructureType                           sType
		mInputAssemblyState->pNext = nullptr;                                                     // const void                              * pNext
		mInputAssemblyState->flags = 0;                                                           // VkPipelineInputAssemblyStateCreateFlags   flags
		mInputAssemblyState->topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;    // VkPrimitiveTopology                       topology
		mInputAssemblyState->primitiveRestartEnable = false;                    // VkBool32                                  primitiveRestartEnable
	}
}

void GraphicsPipeline::ensuresTessellationStateInitialized()
{
	if (mTessellationState == nullptr)
	{
		mTessellationState = std::unique_ptr<VkPipelineTessellationStateCreateInfo>(new VkPipelineTessellationStateCreateInfo());
		
		*mTessellationState = {
			VK_STRUCTURE_TYPE_PIPELINE_TESSELLATION_STATE_CREATE_INFO, // VkStructureType                            sType
			nullptr,                                                   // const void                               * pNext
			0,                                                         // VkPipelineTessellationStateCreateFlags     flags
			0                                                          // uint32_t                                   patchControlPoints
		};
	}
}

void GraphicsPipeline::ensuresRasterizationStateInitialized()
{
	if (mRasterizationState == nullptr)
	{
		mRasterizationState = std::unique_ptr<VkPipelineRasterizationStateCreateInfo>(new VkPipelineRasterizationStateCreateInfo());

		*mRasterizationState = sDefaultRasterizationState;
	}
}

void GraphicsPipeline::ensuresMultisampleStateInitialized()
{
	if (mMultisampleState == nullptr)
	{
		mMultisampleState = std::unique_ptr<VkPipelineMultisampleStateCreateInfo>(new VkPipelineMultisampleStateCreateInfo());

		*mMultisampleState = sDefaultMultisampleState;
	}
}

void GraphicsPipeline::ensuresDepthStencilStateInitialized()
{
	if (mDepthStencilState == nullptr)
	{
		mDepthStencilState = std::unique_ptr<VkPipelineDepthStencilStateCreateInfo>(new VkPipelineDepthStencilStateCreateInfo());
		
		*mDepthStencilState = sDefaultDepthStencilState;
	}
}

const VkPipelineInputAssemblyStateCreateInfo GraphicsPipeline::sDefaultInputAssemblyState = {
	VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO,  // VkStructureType                            sType
	nullptr,                                                      // const void                               * pNext
	0,                                                            // VkPipelineTessellationStateCreateFlags     flags
	VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST,                          // VkPrimitiveTopology                       topology
	false                                                         // VkBool32                                  primitiveRestartEnable
};

const VkPipelineRasterizationStateCreateInfo GraphicsPipeline::sDefaultRasterizationState = {
	VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO, // VkStructureType                            sType
	nullptr,                                                    // const void                               * pNext
	0,                                                          // VkPipelineRasterizationStateCreateFlags    flags
	false,                                                      // VkBool32                                   depthClampEnable
	false,                                                      // VkBool32                                   rasterizerDiscardEnable
	VK_POLYGON_MODE_FILL,                                       // VkPolygonMode                              polygonMode
	VK_CULL_MODE_BACK_BIT,                                      // VkCullModeFlags                            cullMode
	VK_FRONT_FACE_COUNTER_CLOCKWISE,                            // VkFrontFace                                frontFace
	false,                                                      // VkBool32                                   depthBiasEnable
	0.0f,                                                       // float                                      depthBiasConstantFactor
	0.0f,                                                       // float                                      depthBiasClamp
	0.0f,                                                       // float                                      depthBiasSlopeFactor
	1.0f                                                        // float                                      lineWidth
};

const VkPipelineMultisampleStateCreateInfo GraphicsPipeline::sDefaultMultisampleState = {
	VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO, // VkStructureType                          sType
	nullptr,                                                  // const void                             * pNext
	0,                                                        // VkPipelineMultisampleStateCreateFlags    flags
	VK_SAMPLE_COUNT_1_BIT,                                    // VkSampleCountFlagBits                    rasterizationSamples
	false,                                                    // VkBool32                                 sampleShadingEnable
	0.0f,                                                     // float                                    minSampleShading
	nullptr,                                                  // const VkSampleMask                     * pSampleMask
	false,                                                    // VkBool32                                 alphaToCoverageEnable
	false                                                     // VkBool32                                 alphaToOneEnable
};

const VkPipelineDepthStencilStateCreateInfo GraphicsPipeline::sDefaultDepthStencilState = {
	VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO,   // VkStructureType                            sType
	nullptr,                                                      // const void                               * pNext
	0,                                                            // VkPipelineDepthStencilStateCreateFlags     flags
	true,                                                         // VkBool32                                   depthTestEnable
	true,                                                         // VkBool32                                   depthWriteEnable
	VK_COMPARE_OP_LESS_OR_EQUAL,                                  // VkCompareOp                                depthCompareOp
	false,                                                        // VkBool32                                   depthBoundsTestEnable
	false,                                                        // VkBool32                                   stencilTestEnable
	{},                                                           // VkStencilOpState                           front
	{},                                                           // VkStencilOpState                           back
	0.0f,                                                         // float                                      minDepthBounds
	1.0f                                                          // float                                      maxDepthBounds
};

} // namespace Vulkan
} // namespace nu