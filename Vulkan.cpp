#include "Vulkan.hpp"

namespace nu
{

Vulkan::Vulkan()
	: mDynamicLibrary()
	, mReady(false)
{
}

Vulkan::~Vulkan()
{
	mReady = false;
	mDynamicLibrary.unloadLibrary();
}

bool Vulkan::initialize(const WindowParameters& windowParameters)
{
	// Initialize
	if (!initializeVulkan(windowParameters))
	{
		return false;
	}

	// Mesh
	if (!VulkanLoader::loadModelFromObjFile("../Models/knot.obj", true, false, false, true, mModel))
	{
		return false;
	}

	// Vertex buffer
	mVertexBuffer.getDeviceHandle() = mLogicalDevice.getHandle();
	if (!VulkanLoader::createBuffer(mLogicalDevice.getHandle(), sizeof(mModel.data[0]) * mModel.data.size(), VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT, mVertexBuffer.getHandle()))
	{
		return false;
	}
	mVertexBufferMemory.getDeviceHandle() = mLogicalDevice.getHandle();
	if (!VulkanLoader::allocateAndBindMemoryObjectToBuffer(mPhysicalDevice, mLogicalDevice.getHandle(), mVertexBuffer.getHandle(), VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, mVertexBufferMemory.getHandle()))
	{
		return false;
	}

	if (!VulkanLoader::useStagingBufferToUpdateBufferWithDeviceLocalMemoryBound(mPhysicalDevice, mLogicalDevice.getHandle(), sizeof(mModel.data[0]) * mModel.data.size(),
		&mModel.data[0], mVertexBuffer.getHandle(), 0, 0, VK_ACCESS_VERTEX_ATTRIBUTE_READ_BIT, VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT, VK_PIPELINE_STAGE_VERTEX_INPUT_BIT,
		mGraphicsQueue.handle, mFramesResources.front().commandBuffer, {})) 
	{
		return false;
	}

	// Staging buffer
	mStagingBuffer.getDeviceHandle() = mLogicalDevice.getHandle();
	if (!VulkanLoader::createBuffer(mLogicalDevice.getHandle(), sizeof(float) * 16 * 2, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, mStagingBuffer.getHandle()))
	{
		return false;
	}
	mStagingBufferMemory.getDeviceHandle() = mLogicalDevice.getHandle();
	if (!VulkanLoader::allocateAndBindMemoryObjectToBuffer(mPhysicalDevice, mLogicalDevice.getHandle(), mStagingBuffer.getHandle(), VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT, mStagingBufferMemory.getHandle()))
	{
		return false;
	}

	// Uniform buffer
	mUniformBuffer.getDeviceHandle() = mLogicalDevice.getHandle();
	mUniformBufferMemory.getDeviceHandle() = mLogicalDevice.getHandle();
	if (!VulkanLoader::createUniformBuffer(mPhysicalDevice, mLogicalDevice.getHandle(), 2 * 16 * sizeof(float), VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, mUniformBuffer.getHandle(), mUniformBufferMemory.getHandle()))
	{
		return false;
	}

	if (!updateStagingBuffer(true))
	{
		return false;
	}

	// Descriptor set for UniformBuffer
	VkDescriptorSetLayoutBinding descriptorSetLayoutBinding = {
		0,                                          // uint32_t             binding
		VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,          // VkDescriptorType     descriptorType
		1,                                          // uint32_t             descriptorCount
		VK_SHADER_STAGE_VERTEX_BIT,                 // VkShaderStageFlags   stageFlags
		nullptr                                     // const VkSampler    * pImmutableSamplers
	};
	mDescriptorSetLayout.getDeviceHandle() = mLogicalDevice.getHandle();
	if (!VulkanLoader::createDescriptorSetLayout(mLogicalDevice.getHandle(), { descriptorSetLayoutBinding }, mDescriptorSetLayout.getHandle()))
	{
		return false;
	}
	VkDescriptorPoolSize descriptorPoolSize = {
		VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,          // VkDescriptorType     type
		1                                           // uint32_t             descriptorCount
	};
	mDescriptorPool.getDeviceHandle() = mLogicalDevice.getHandle();
	if (!VulkanLoader::createDescriptorPool(mLogicalDevice.getHandle(), false, 1, { descriptorPoolSize }, mDescriptorPool.getHandle()))
	{
		return false;
	}
	if (!VulkanLoader::allocateDescriptorSets(mLogicalDevice.getHandle(), mDescriptorPool.getHandle(), { mDescriptorSetLayout.getHandle() }, mDescriptorSets))
	{
		return false;
	}
	VulkanLoader::BufferDescriptorInfo bufferDescriptorUpdate = {
		mDescriptorSets[0],                         // VkDescriptorSet                      TargetDescriptorSet
		0,                                          // uint32_t                             TargetDescriptorBinding
		0,                                          // uint32_t                             TargetArrayElement
		VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,          // VkDescriptorType                     TargetDescriptorType
		{                                           // std::vector<VkDescriptorBufferInfo>  BufferInfos
			{
				mUniformBuffer.getHandle(),               // VkBuffer                             buffer
				0,                                        // VkDeviceSize                         offset
				VK_WHOLE_SIZE                             // VkDeviceSize                         range
			}
		}
	};
	VulkanLoader::updateDescriptorSets(mLogicalDevice.getHandle(), {}, { bufferDescriptorUpdate }, {}, {});

	// Render pass
	std::vector<VkAttachmentDescription> attachmentDescriptions = {
		{
			0,                                                // VkAttachmentDescriptionFlags     flags
			mSwapchain.format,                                // VkFormat                         format
			VK_SAMPLE_COUNT_1_BIT,                            // VkSampleCountFlagBits            samples
			VK_ATTACHMENT_LOAD_OP_CLEAR,                      // VkAttachmentLoadOp               loadOp
			VK_ATTACHMENT_STORE_OP_STORE,                     // VkAttachmentStoreOp              storeOp
			VK_ATTACHMENT_LOAD_OP_DONT_CARE,                  // VkAttachmentLoadOp               stencilLoadOp
			VK_ATTACHMENT_STORE_OP_DONT_CARE,                 // VkAttachmentStoreOp              stencilStoreOp
			VK_IMAGE_LAYOUT_UNDEFINED,                        // VkImageLayout                    initialLayout
			VK_IMAGE_LAYOUT_PRESENT_SRC_KHR                   // VkImageLayout                    finalLayout
		},
		{
			0,                                                // VkAttachmentDescriptionFlags     flags
			depthFormat,                                      // VkFormat                         format
			VK_SAMPLE_COUNT_1_BIT,                            // VkSampleCountFlagBits            samples
			VK_ATTACHMENT_LOAD_OP_CLEAR,                      // VkAttachmentLoadOp               loadOp
			VK_ATTACHMENT_STORE_OP_DONT_CARE,                 // VkAttachmentStoreOp              storeOp
			VK_ATTACHMENT_LOAD_OP_DONT_CARE,                  // VkAttachmentLoadOp               stencilLoadOp
			VK_ATTACHMENT_STORE_OP_DONT_CARE,                 // VkAttachmentStoreOp              stencilStoreOp
			VK_IMAGE_LAYOUT_UNDEFINED,                        // VkImageLayout                    initialLayout
			VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL  // VkImageLayout                    finalLayout
		}
	};
	VkAttachmentReference depthAttachment = {
		1,                                                // uint32_t                             attachment
		VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL  // VkImageLayout                        layout
	};
	std::vector<VulkanLoader::SubpassParameters> subpassParameters = {
		{
			VK_PIPELINE_BIND_POINT_GRAPHICS,              // VkPipelineBindPoint                  PipelineType
			{},                                           // std::vector<VkAttachmentReference>   InputAttachments
			{                                             // std::vector<VkAttachmentReference>   ColorAttachments
				{
					0,                                          // uint32_t                             attachment
					VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL,   // VkImageLayout                        layout
				}
			},
			{},                                           // std::vector<VkAttachmentReference>   ResolveAttachments
			&depthAttachment,                             // VkAttachmentReference const        * DepthStencilAttachment
			{}                                            // std::vector<uint32_t>                PreserveAttachments
		}
	};
	std::vector<VkSubpassDependency> subpassDependencies = {
		{
			VK_SUBPASS_EXTERNAL,                            // uint32_t                   srcSubpass
			0,                                              // uint32_t                   dstSubpass
			VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT,              // VkPipelineStageFlags       srcStageMask
			VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,  // VkPipelineStageFlags       dstStageMask
			VK_ACCESS_MEMORY_READ_BIT,                      // VkAccessFlags              srcAccessMask
			VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT,           // VkAccessFlags              dstAccessMask
			VK_DEPENDENCY_BY_REGION_BIT                     // VkDependencyFlags          dependencyFlags
		},
		{
			0,                                              // uint32_t                   srcSubpass
			VK_SUBPASS_EXTERNAL,                            // uint32_t                   dstSubpass
			VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,  // VkPipelineStageFlags       srcStageMask
			VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT,              // VkPipelineStageFlags       dstStageMask
			VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT,           // VkAccessFlags              srcAccessMask
			VK_ACCESS_MEMORY_READ_BIT,                      // VkAccessFlags              dstAccessMask
			VK_DEPENDENCY_BY_REGION_BIT                     // VkDependencyFlags          dependencyFlags
		}
	};
	mRenderPass.getDeviceHandle() = mLogicalDevice.getHandle();
	if (!VulkanLoader::createRenderPass(mLogicalDevice.getHandle(), attachmentDescriptions, subpassParameters, subpassDependencies, mRenderPass.getHandle())) 
	{
		return false;
	}

	// Graphics pipeline
	mPipelineLayout.getDeviceHandle() = mLogicalDevice.getHandle();
	if (!VulkanLoader::createPipelineLayout(mLogicalDevice.getHandle(), { mDescriptorSetLayout.getHandle() }, {}, mPipelineLayout.getHandle()))
	{
		return false;
	}
	std::vector<unsigned char> vertexShaderSpirv;
	if (!VulkanLoader::loadBinaryFile("../Shaders/vertexShader.vert.spv", vertexShaderSpirv)) 
	{
		return false;
	}
	VkSmartShaderModule vertexShaderModule(mLogicalDevice.getHandle());
	if (!VulkanLoader::createShaderModule(mLogicalDevice.getHandle(), vertexShaderSpirv, vertexShaderModule.getHandle())) 
	{
		return false;
	}
	std::vector<unsigned char> fragmentShaderSpirv;
	if (!VulkanLoader::loadBinaryFile("../Shaders/fragmentShader.frag.spv", fragmentShaderSpirv))
	{
		return false;
	}
	VkSmartShaderModule fragmentShaderModule(mLogicalDevice.getHandle());
	if (!VulkanLoader::createShaderModule(mLogicalDevice.getHandle(), fragmentShaderSpirv, fragmentShaderModule.getHandle()))
	{
		return false;
	}
	std::vector<VulkanLoader::ShaderStageParameters> shaderStageParams = {
		{
			VK_SHADER_STAGE_VERTEX_BIT,       // VkShaderStageFlagBits        ShaderStage
			vertexShaderModule.getHandle(),   // VkShaderModule               ShaderModule
			"main",                           // char const                 * EntryPointName
			nullptr                           // VkSpecializationInfo const * SpecializationInfo
		},
		{
			VK_SHADER_STAGE_FRAGMENT_BIT,     // VkShaderStageFlagBits        ShaderStage
			fragmentShaderModule.getHandle(), // VkShaderModule               ShaderModule
			"main",                           // char const                 * EntryPointName
			nullptr                           // VkSpecializationInfo const * SpecializationInfo
		}
	};
	std::vector<VkPipelineShaderStageCreateInfo> shaderStageCreateInfos;
	VulkanLoader::specifyPipelineShaderStages(shaderStageParams, shaderStageCreateInfos);
	std::vector<VkVertexInputBindingDescription> vertexInputBindingDescriptions = {
		{
			0,                            // uint32_t                     binding
			6 * sizeof(float),            // uint32_t                     stride
			VK_VERTEX_INPUT_RATE_VERTEX   // VkVertexInputRate            inputRate
		}
	};
	std::vector<VkVertexInputAttributeDescription> vertexAttributeDescriptions = {
		{
			0,                                                                        // uint32_t   location
			0,                                                                        // uint32_t   binding
			VK_FORMAT_R32G32B32_SFLOAT,                                               // VkFormat   format
			0                                                                         // uint32_t   offset
		},
		{
			1,                                                                        // uint32_t   location
			0,                                                                        // uint32_t   binding
			VK_FORMAT_R32G32B32_SFLOAT,                                               // VkFormat   format
			3 * sizeof(float)                                                         // uint32_t   offset
		}
	};
	VkPipelineVertexInputStateCreateInfo vertexInputStateCreateInfo;
	VulkanLoader::specifyPipelineVertexInputState(vertexInputBindingDescriptions, vertexAttributeDescriptions, vertexInputStateCreateInfo);
	VkPipelineInputAssemblyStateCreateInfo inputAssemblyStateCreateInfo;
	VulkanLoader::specifyPipelineInputAssemblyState(VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST, false, inputAssemblyStateCreateInfo);
	VulkanLoader::ViewportInfo viewportInfos = {
		{                     // std::vector<VkViewport>   Viewports
			{
				0.0f,               // float          x
				0.0f,               // float          y
				500.0f,             // float          width
				500.0f,             // float          height
				0.0f,               // float          minDepth
				1.0f                // float          maxDepth
			}
		},
		{                     // std::vector<VkRect2D>     Scissors
			{
				{                   // VkOffset2D     offset
					0,                  // int32_t        x
					0                   // int32_t        y
				},
			{                   // VkExtent2D     extent
				500,                // uint32_t       width
				500                 // uint32_t       height
			}
			}
		}
	};
	VkPipelineViewportStateCreateInfo viewportStateCreateInfo;
	VulkanLoader::specifyPipelineViewportAndScissorTestState(viewportInfos, viewportStateCreateInfo);
	VkPipelineRasterizationStateCreateInfo rasterizationStateCreateInfo;
	VulkanLoader::specifyPipelineRasterizationState(false, false, VK_POLYGON_MODE_FILL, VK_CULL_MODE_BACK_BIT, VK_FRONT_FACE_COUNTER_CLOCKWISE, false, 0.0f, 0.0f, 0.0f, 1.0f, rasterizationStateCreateInfo);
	VkPipelineMultisampleStateCreateInfo multisampleStateCreateInfo;
	VulkanLoader::specifyPipelineMultisampleState(VK_SAMPLE_COUNT_1_BIT, false, 0.0f, nullptr, false, false, multisampleStateCreateInfo);
	VkPipelineDepthStencilStateCreateInfo depthStencilStateCreateInfo;
	VulkanLoader::specifyPipelineDepthAndStencilState(true, true, VK_COMPARE_OP_LESS_OR_EQUAL, false, 0.0f, 1.0f, false, {}, {}, depthStencilStateCreateInfo);
	std::vector<VkPipelineColorBlendAttachmentState> attachmentBlendStates = {
		{
			false,                          // VkBool32                 blendEnable
			VK_BLEND_FACTOR_ONE,            // VkBlendFactor            srcColorBlendFactor
			VK_BLEND_FACTOR_ONE,            // VkBlendFactor            dstColorBlendFactor
			VK_BLEND_OP_ADD,                // VkBlendOp                colorBlendOp
			VK_BLEND_FACTOR_ONE,            // VkBlendFactor            srcAlphaBlendFactor
			VK_BLEND_FACTOR_ONE,            // VkBlendFactor            dstAlphaBlendFactor
			VK_BLEND_OP_ADD,                // VkBlendOp                alphaBlendOp
			VK_COLOR_COMPONENT_R_BIT |      // VkColorComponentFlags    colorWriteMask
			VK_COLOR_COMPONENT_G_BIT |
			VK_COLOR_COMPONENT_B_BIT |
			VK_COLOR_COMPONENT_A_BIT
		}
	};
	VkPipelineColorBlendStateCreateInfo blendStateCreateInfo;
	VulkanLoader::specifyPipelineBlendState(false, VK_LOGIC_OP_COPY, attachmentBlendStates, { 1.0f, 1.0f, 1.0f, 1.0f }, blendStateCreateInfo);
	std::vector<VkDynamicState> dynamicStates = {
		VK_DYNAMIC_STATE_VIEWPORT,
		VK_DYNAMIC_STATE_SCISSOR
	};
	VkPipelineDynamicStateCreateInfo dynamicStateCreateInfo;
	VulkanLoader::specifyPipelineDynamicStates(dynamicStates, dynamicStateCreateInfo);
	VkGraphicsPipelineCreateInfo pipelineCreateInfo;
	VulkanLoader::specifyGraphicsPipelineCreationParameters(0, shaderStageCreateInfos, vertexInputStateCreateInfo, inputAssemblyStateCreateInfo,
		nullptr, &viewportStateCreateInfo, rasterizationStateCreateInfo, &multisampleStateCreateInfo, &depthStencilStateCreateInfo, &blendStateCreateInfo,
		&dynamicStateCreateInfo, mPipelineLayout.getHandle(), mRenderPass.getHandle(), 0, VK_NULL_HANDLE, -1, pipelineCreateInfo);
	std::vector<VkPipeline> pipeline;
	if (!VulkanLoader::createGraphicsPipelines(mLogicalDevice.getHandle(), { pipelineCreateInfo }, VK_NULL_HANDLE, pipeline)) 
	{
		return false;
	}
	mPipeline.getDeviceHandle() = mLogicalDevice.getHandle();
	mPipeline.getHandle() = pipeline[0];

	return true;
}

bool Vulkan::draw()
{
	auto prepareFrame = [&](VkCommandBuffer commandBuffer, uint32_t swapchainImageIndex, VkFramebuffer framebuffer) 
	{
		if (!VulkanLoader::beginCommandBufferRecordingOperation(commandBuffer, VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT, nullptr)) 
		{
			return false;
		}

		if (mUpdateUniformBuffer) 
		{
			mUpdateUniformBuffer = false;

			VulkanLoader::BufferTransition preTransferTransition = {
				mUniformBuffer.getHandle(),   // VkBuffer         Buffer
				VK_ACCESS_UNIFORM_READ_BIT,   // VkAccessFlags    CurrentAccess
				VK_ACCESS_TRANSFER_WRITE_BIT, // VkAccessFlags    NewAccess
				VK_QUEUE_FAMILY_IGNORED,      // uint32_t         CurrentQueueFamily
				VK_QUEUE_FAMILY_IGNORED       // uint32_t         NewQueueFamily
			};
			VulkanLoader::setBufferMemoryBarrier(commandBuffer, VK_PIPELINE_STAGE_VERTEX_SHADER_BIT, VK_PIPELINE_STAGE_TRANSFER_BIT, { preTransferTransition });

			std::vector<VkBufferCopy> regions = {
				{
					0,                        // VkDeviceSize     srcOffset
					0,                        // VkDeviceSize     dstOffset
					2 * 16 * sizeof(float)    // VkDeviceSize     size
				}
			};
			VulkanLoader::copyDataBetweenBuffers(commandBuffer, mStagingBuffer.getHandle(), mUniformBuffer.getHandle(), regions);

			VulkanLoader::BufferTransition postTransferTransition = {
				mUniformBuffer.getHandle(),   // VkBuffer         Buffer
				VK_ACCESS_TRANSFER_WRITE_BIT, // VkAccessFlags    CurrentAccess
				VK_ACCESS_UNIFORM_READ_BIT,   // VkAccessFlags    NewAccess
				VK_QUEUE_FAMILY_IGNORED,      // uint32_t         CurrentQueueFamily
				VK_QUEUE_FAMILY_IGNORED       // uint32_t         NewQueueFamily
			};
			VulkanLoader::setBufferMemoryBarrier(commandBuffer, VK_PIPELINE_STAGE_TRANSFER_BIT, VK_PIPELINE_STAGE_VERTEX_SHADER_BIT, { postTransferTransition });
		}

		if (mPresentQueue.familyIndex != mGraphicsQueue.familyIndex) 
		{
			VulkanLoader::ImageTransition imageTransitionBeforeDrawing = {
				mSwapchain.images[swapchainImageIndex],   // VkImage              Image
				VK_ACCESS_MEMORY_READ_BIT,                // VkAccessFlags        CurrentAccess
				VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT,     // VkAccessFlags        NewAccess
				VK_IMAGE_LAYOUT_UNDEFINED,                // VkImageLayout        CurrentLayout
				VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL, // VkImageLayout        NewLayout
				mPresentQueue.familyIndex,                // uint32_t             CurrentQueueFamily
				mGraphicsQueue.familyIndex,               // uint32_t             NewQueueFamily
				VK_IMAGE_ASPECT_COLOR_BIT                 // VkImageAspectFlags   Aspect
			};
			VulkanLoader::setImageMemoryBarrier(commandBuffer, VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT, VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT, { imageTransitionBeforeDrawing });
		}

		// Drawing
		VulkanLoader::beginRenderPass(commandBuffer, mRenderPass.getHandle(), framebuffer, { { 0, 0 }, mSwapchain.size }, { { 0.1f, 0.2f, 0.3f, 1.0f },{ 1.0f, 0 } }, VK_SUBPASS_CONTENTS_INLINE);

		VkViewport viewport = {
			0.0f,                                       // float    x
			0.0f,                                       // float    y
			static_cast<float>(mSwapchain.size.width),   // float    width
			static_cast<float>(mSwapchain.size.height),  // float    height
			0.0f,                                       // float    minDepth
			1.0f,                                       // float    maxDepth
		};
		VulkanLoader::setViewportStateDynamically(commandBuffer, 0, { viewport });

		VkRect2D scissor = {
			{                                           // VkOffset2D     offset
				0,											// int32_t        x
				0                                           // int32_t        y
			},
			{                                           // VkExtent2D     extent
				mSwapchain.size.width,                      // uint32_t       width
				mSwapchain.size.height                      // uint32_t       height
			}
		};
		VulkanLoader::setScissorStateDynamically(commandBuffer, 0, { scissor });

		VulkanLoader::bindVertexBuffers(commandBuffer, 0, { { mVertexBuffer.getHandle(), 0 } });

		VulkanLoader::bindDescriptorSets(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, mPipelineLayout.getHandle(), 0, mDescriptorSets, {});

		VulkanLoader::bindPipelineObject(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, mPipeline.getHandle());

		for (size_t i = 0; i < mModel.parts.size(); i++) 
		{
			VulkanLoader::drawGeometry(commandBuffer, mModel.parts[i].vertexCount, 1, mModel.parts[i].vertexOffset, 0);
		}

		VulkanLoader::endRenderPass(commandBuffer);

		if (mPresentQueue.familyIndex != mGraphicsQueue.familyIndex) 
		{
			VulkanLoader::ImageTransition imageTransitionBeforePresent = {
				mSwapchain.images[swapchainImageIndex],   // VkImage              Image
				VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT,     // VkAccessFlags        CurrentAccess
				VK_ACCESS_MEMORY_READ_BIT,                // VkAccessFlags        NewAccess
				VK_IMAGE_LAYOUT_PRESENT_SRC_KHR,          // VkImageLayout        CurrentLayout
				VK_IMAGE_LAYOUT_PRESENT_SRC_KHR,          // VkImageLayout        NewLayout
				mGraphicsQueue.familyIndex,               // uint32_t             CurrentQueueFamily
				mPresentQueue.familyIndex,                // uint32_t             NewQueueFamily
				VK_IMAGE_ASPECT_COLOR_BIT                 // VkImageAspectFlags   Aspect
			};
			VulkanLoader::setImageMemoryBarrier(commandBuffer, VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT, VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT, { imageTransitionBeforePresent });
		}

		if (!VulkanLoader::endCommandBufferRecordingOperation(commandBuffer)) 
		{
			return false;
		}
		return true;
	};

	return VulkanLoader::increasePerformanceThroughIncreasingTheNumberOfSeparatelyRenderedFrames(mLogicalDevice.getHandle(), mGraphicsQueue.handle, mPresentQueue.handle, mSwapchain.handle.getHandle(), mSwapchain.size, mSwapchain.imageViewsRaw, mRenderPass.getHandle(), {}, prepareFrame, mFramesResources);
}

bool Vulkan::resize()
{
	if (!createSwapchain())
	{
		return false;
	}
	return true;
}

void Vulkan::deinitialize()
{
	if (mLogicalDevice) 
	{
		VulkanLoader::waitForAllSubmittedCommandsToBeFinished(mLogicalDevice.getHandle());
	}
	vkDestroyDebugReportCallbackEXT(mInstance.getHandle(), mDebug, nullptr);
}

void Vulkan::updateTime()
{
}

bool Vulkan::isReady() const
{
	return mReady;
}

bool Vulkan::updateMatrices(const Matrix4f& m, const Matrix4f& v, const Matrix4f& p)
{
	mUpdateUniformBuffer = true;

	Matrix4f modelView = v * m;
	if (!VulkanLoader::mapUpdateAndUnmapHostVisibleMemory(mLogicalDevice.getHandle(), mStagingBufferMemory.getHandle(), 0, sizeof(modelView[0]) * 16, &modelView.data[0], true, nullptr))
	{
		return false;
	}

	Matrix4f perspective = p;
	if (!VulkanLoader::mapUpdateAndUnmapHostVisibleMemory(mLogicalDevice.getHandle(), mStagingBufferMemory.getHandle(), 0, sizeof(perspective[0]) * 16, &perspective.data[0], true, nullptr))
	{
		return false;
	}

	return true;
}

bool Vulkan::initializeVulkan(const WindowParameters& windowParameters, bool debugReport, VkPhysicalDeviceFeatures * desiredDeviceFeatures, VkImageUsageFlags swapchainImageUsage, bool useDepth, VkImageUsageFlags depthAttachmentUsage)
{
	if (!VulkanLoader::loadDynamicLibrary(mDynamicLibrary))
	{
		return false;
	}

	if (!VulkanLoader::loadFunctionExportedFromDynamicLibrary(mDynamicLibrary))
	{
		return false;
	}

	if (!VulkanLoader::loadGlobalLevelFunctions())
	{
		return false;
	}

	std::vector<const char*> instanceExtensions;
	if (debugReport)
	{
		instanceExtensions.push_back(VK_EXT_DEBUG_REPORT_EXTENSION_NAME);
	}
	if (!VulkanLoader::createInstanceWithWsiExtensionsEnabled(instanceExtensions, "Numea", mInstance.getHandle()))
	{
		return false;
	}

	if (!VulkanLoader::loadInstanceLevelFunctions(mInstance.getHandle(), instanceExtensions))
	{
		return false;
	}

	mDebug = VK_NULL_HANDLE;
	if (debugReport)
	{
		VkDebugReportCallbackCreateInfoEXT callback = {
			VK_STRUCTURE_TYPE_DEBUG_REPORT_CALLBACK_CREATE_INFO_EXT,    // sType
			NULL,                                                       // pNext
			10,                                                         // flags
			&Vulkan::debugCallback,                                     // pfnCallback
			NULL                                                        // pUserData
		};
		if (vkCreateDebugReportCallbackEXT(mInstance.getHandle(), &callback, nullptr, &mDebug) != VK_SUCCESS)
		{
			printf("DebugReport can't be created\n");
			return false;
		}
	}

	mPresentationSurface.initialize(mInstance.getHandle());
	if (!VulkanLoader::createPresentationSurface(mInstance.getHandle(), windowParameters, mPresentationSurface.getHandle()))
	{
		return false;
	}

	std::vector<VkPhysicalDevice> physicalDevices;
	if (!VulkanLoader::queryAvailablePhysicalDevices(mInstance.getHandle(), physicalDevices))
	{
		return false;
	}

	for (auto& physicalDevice : physicalDevices)
	{
		std::vector<VkQueueFamilyProperties> queueFamilies;
		if (!VulkanLoader::queryAvailableQueueFamiliesAndTheirProperties(physicalDevice, queueFamilies))
		{
			continue;
		}

		if (!VulkanLoader::selectQueueFamilyWithDesiredCapabilities(physicalDevice, queueFamilies, VK_QUEUE_GRAPHICS_BIT, mGraphicsQueue.familyIndex))
		{
			continue;
		}

		if (!VulkanLoader::selectQueueFamilyWithDesiredCapabilities(physicalDevice, queueFamilies, VK_QUEUE_COMPUTE_BIT, mComputeQueue.familyIndex))
		{
			continue;
		}

		if (!VulkanLoader::selectQueueFamilyThatSupportsPresentationToGivenSurface(physicalDevice, mPresentationSurface.getHandle(), mPresentQueue.familyIndex))
		{
			continue;
		}

		std::vector<VulkanLoader::QueueInfo> requestedQueues = { { mGraphicsQueue.familyIndex,{ 1.0f } } };
		if (mGraphicsQueue.familyIndex != mComputeQueue.familyIndex) 
		{
			requestedQueues.push_back({ mComputeQueue.familyIndex,{ 1.0f } });
		}
		if ((mGraphicsQueue.familyIndex != mPresentQueue.familyIndex) && (mComputeQueue.familyIndex != mPresentQueue.familyIndex)) 
		{
			requestedQueues.push_back({ mPresentQueue.familyIndex,{ 1.0f } });
		}

		std::vector<const char*> deviceExtensions;
		if (!VulkanLoader::createLogicalDeviceWithWsiExtensionsEnabled(physicalDevice, requestedQueues, deviceExtensions, desiredDeviceFeatures, mLogicalDevice.getHandle()))
		{
			continue;
		}
		else
		{
			if (!VulkanLoader::loadDeviceLevelFunctions(mLogicalDevice.getHandle(), deviceExtensions))
			{
				continue;
			}
			mPhysicalDevice = physicalDevice;
			VulkanLoader::getDeviceQueue(mLogicalDevice.getHandle(), mGraphicsQueue.familyIndex, 0, mGraphicsQueue.handle);
			VulkanLoader::getDeviceQueue(mLogicalDevice.getHandle(), mComputeQueue.familyIndex, 0, mComputeQueue.handle);
			VulkanLoader::getDeviceQueue(mLogicalDevice.getHandle(), mPresentQueue.familyIndex, 0, mPresentQueue.handle);
			break;
		}
	}
	if (!mLogicalDevice)
	{
		return false;
	}

	mCommandPool.getDeviceHandle() = mLogicalDevice.getHandle();
	if (!VulkanLoader::createCommandPool(mLogicalDevice.getHandle(), VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT, mGraphicsQueue.familyIndex, mCommandPool.getHandle()))
	{
		return false;
	}

	for (uint32_t i = 0; i < framesCount; i++) 
	{
		std::vector<VkCommandBuffer> commandBuffer;
		VkSmartSemaphore imageAcquiredSemaphore(mLogicalDevice.getHandle());
		VkSmartSemaphore readyToPresentSemaphore(mLogicalDevice.getHandle());
		VkSmartFence drawingFinishedFence(mLogicalDevice.getHandle());
		VkSmartImageView depthAttachment(mLogicalDevice.getHandle());
		VkSmartFramebuffer fb(mLogicalDevice.getHandle()); // TODO : Do this should get the logical device ???

		if (!VulkanLoader::allocateCommandBuffers(mLogicalDevice.getHandle(), mCommandPool.getHandle(), VK_COMMAND_BUFFER_LEVEL_PRIMARY, 1, commandBuffer)) 
		{
			return false;
		}
		if (!VulkanLoader::createSemaphore(mLogicalDevice.getHandle(), imageAcquiredSemaphore.getHandle())) 
		{
			return false;
		}
		if (!VulkanLoader::createSemaphore(mLogicalDevice.getHandle(), readyToPresentSemaphore.getHandle())) 
		{
			return false;
		}
		if (!VulkanLoader::createFence(mLogicalDevice.getHandle(), true, drawingFinishedFence.getHandle())) 
		{
			return false;
		}

		mFramesResources.emplace_back(
			commandBuffer[0],
			imageAcquiredSemaphore,
			readyToPresentSemaphore,
			drawingFinishedFence,
			depthAttachment,
			fb
		);
	}

	if (!createSwapchain(swapchainImageUsage, useDepth, depthAttachmentUsage))
	{
		return false;
	}

	return true;
}

bool Vulkan::createSwapchain(VkImageUsageFlags swapchainImageUsage, bool useDepth, VkImageUsageFlags depthAttachmentUsage)
{
	VulkanLoader::waitForAllSubmittedCommandsToBeFinished(mLogicalDevice.getHandle()); // used for resize

	mReady = false; // If recreate

	mSwapchain.imageViewsRaw.clear();
	mSwapchain.imageViews.clear();
	mSwapchain.images.clear();

	if (!mSwapchain.handle)
	{
		mSwapchain.handle.getDeviceHandle() = mLogicalDevice.getHandle();
	}
	VkSmartSwapchainKHR oldSwapchain = std::move(mSwapchain.handle);
	mSwapchain.handle.getDeviceHandle() = mLogicalDevice.getHandle();
	if (!VulkanLoader::createSwapchainWithR8G8B8A8FormatAndMailboxPresentMode(mPhysicalDevice, mPresentationSurface.getHandle(), mLogicalDevice.getHandle(), swapchainImageUsage, mSwapchain.size, mSwapchain.format, oldSwapchain.getHandle(), mSwapchain.handle.getHandle(), mSwapchain.images))
	{
		return false;
	}
	if (!mSwapchain.handle)
	{
		return true; // TODO : Do this should really be true ???
	}

	for (size_t i = 0; i < mSwapchain.images.size(); i++)
	{
		mSwapchain.imageViews.emplace_back(VkSmartImageView(mLogicalDevice.getHandle()));
		if (!VulkanLoader::createImageView(mLogicalDevice.getHandle(), mSwapchain.images[i], VK_IMAGE_VIEW_TYPE_2D, mSwapchain.format, VK_IMAGE_ASPECT_COLOR_BIT, mSwapchain.imageViews.back().getHandle()))
		{
			return false;
		}
		mSwapchain.imageViewsRaw.push_back(mSwapchain.imageViews.back().getHandle());
	}


	// When we want to use depth buffering, we need to use a depth attachment
	// It must have the same size as the swapchain, so we need to recreate it along with the swapchain
	mDepthImages.clear();
	mDepthImagesMemory.clear();

	if (useDepth)
	{
		for (uint32_t i = 0; i < framesCount; i++)
		{
			mDepthImages.emplace_back(VkSmartImage(mLogicalDevice.getHandle()));
			mDepthImagesMemory.emplace_back(VkSmartDeviceMemory(mLogicalDevice.getHandle()));
			mFramesResources[i].depthAttachment.getDeviceHandle() = mLogicalDevice.getHandle();

			if (!VulkanLoader::create2DImageAndView(mPhysicalDevice, mLogicalDevice.getHandle(), depthFormat, mSwapchain.size, 1, 1, VK_SAMPLE_COUNT_1_BIT, VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT, VK_IMAGE_ASPECT_DEPTH_BIT, mDepthImages.back().getHandle(), mDepthImagesMemory.back().getHandle(), mFramesResources[i].depthAttachment.getHandle())) 
			{
				return false;
			}
		}
	}

	mReady = true;

	return true;
}

bool Vulkan::updateStagingBuffer(bool force)
{
	mUpdateUniformBuffer = true;
	static float horizontalAngle = 0.0f;
	static float verticalAngle = 0.0f;

	if (force)
	{
		horizontalAngle += 0.5f;
		verticalAngle -= 0.2f;

		Matrix4f rotation = Matrix4f(Quaternionf(verticalAngle, { 1.0f, 0.0f, 0.0f }).toMatrix3()) * Matrix4f(Quaternionf(horizontalAngle, { 0.0, 1.0f, 0.0f }).toMatrix3());
		Matrix4f translation = Matrix4f::translation({ 0.0f, 0.0f, -4.0f });
		Matrix4f modelView = translation * rotation;
		if (!VulkanLoader::mapUpdateAndUnmapHostVisibleMemory(mLogicalDevice.getHandle(), mStagingBufferMemory.getHandle(), 0, sizeof(modelView[0]) * 16, &modelView.data[0], true, nullptr)) 
		{
			return false;
		}

		float ratio = static_cast<float>(mSwapchain.size.width) / static_cast<float>(mSwapchain.size.height);
		Matrix4f perspective = Matrix4f::perspective(50.0f, ratio, 0.5f, 10.0f);
		if (!VulkanLoader::mapUpdateAndUnmapHostVisibleMemory(mLogicalDevice.getHandle(), mStagingBufferMemory.getHandle(), 0, sizeof(perspective[0]) * 16, &perspective.data[0], true, nullptr)) 
		{
			return false;
		}
	}

	return true;
}

VkBool32 Vulkan::debugCallback(VkDebugReportFlagsEXT flags, VkDebugReportObjectTypeEXT objectType, uint64_t object, size_t location, int32_t messageCode, const char * pLayerPrefix, const char * pMessage, void * pUserData)
{
	printf("%s\n", pMessage);
	return (VkBool32)true;
}

} // namespace nu
