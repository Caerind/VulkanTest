#ifndef REFLECTIVE_AND_REFRACTIVE_GEOMETRY_USING_CUBEMAPS_HPP
#define REFLECTIVE_AND_REFRACTIVE_GEOMETRY_USING_CUBEMAPS_HPP

#include "../../CookBook/SampleBase.hpp"

#include "../../Math/Matrix4.hpp"

#include "../../Mesh.hpp"
#include "../../Window.hpp"

class ReflectiveAndRefractiveGeometryUsingCubemaps : public SampleBase
{
	public:
		nu::Mesh mSkybox;
		nu::Vulkan::Buffer::Ptr mSkyboxVertexBuffer;
		nu::Vulkan::MemoryBlock::Ptr mSkyboxVertexBufferMemory;

		nu::Mesh mMesh;
		nu::Vulkan::Buffer::Ptr mMeshVertexBuffer;
		nu::Vulkan::MemoryBlock::Ptr mMeshVertexBufferMemory;

		nu::Vulkan::ImageHelper::Ptr mSkyboxTexture;

		nu::Vulkan::DescriptorSetLayout::Ptr mDescriptorSetLayout;
		nu::Vulkan::DescriptorPool::Ptr mDescriptorPool;
		std::vector<nu::Vulkan::DescriptorSet::Ptr> mDescriptorSets;

		nu::Vulkan::RenderPass::Ptr mRenderPass;
		nu::Vulkan::PipelineLayout::Ptr mPipelineLayout; 
		std::vector<nu::Vulkan::GraphicsPipeline::Ptr> mPipelines;
		enum PipelineNames
		{
			MeshPipeline = 0,
			SkyboxPipeline = 1,
			Count
		};

		nu::Vulkan::Buffer::Ptr mStagingBuffer;
		nu::Vulkan::MemoryBlock::Ptr mStagingBufferMemory;
		bool mUpdateUniformBuffer;
		nu::Vulkan::Buffer::Ptr mUniformBuffer;
		nu::Vulkan::MemoryBlock::Ptr mUniformBufferMemory;

		uint32_t mFrameIndex = 0;

		virtual bool initialize(nu::Vulkan::WindowParameters windowParameters) override 
		{
			if (!initializeVulkan(windowParameters, nullptr)) 
			{
				return false;
			}

			// TODO : Camera

			// Vertex data - mesh
			if (!mMesh.loadFromFile("../Data/Models/teapot.obj", true, false, false, true))
			{
				return false;
			}
			mMeshVertexBuffer = mLogicalDevice->createBuffer(mMesh.size(), VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT);
			if (mMeshVertexBuffer == nullptr || !mMeshVertexBuffer->isInitialized())
			{
				return false;
			}
			mMeshVertexBufferMemory = mMeshVertexBuffer->allocateAndBindMemoryBlock(VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);
			if (mMeshVertexBufferMemory == nullptr || !mMeshVertexBufferMemory->isInitialized())
			{
				return false;
			}
			if (!mGraphicsQueue->useStagingBufferToUpdateBufferAndWait(mMesh.size(), &mMesh.data[0], mMeshVertexBuffer.get(),
				0, 0, VK_ACCESS_VERTEX_ATTRIBUTE_READ_BIT, VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT, VK_PIPELINE_STAGE_VERTEX_INPUT_BIT,
				mFramesResources.front().mCommandBuffer.get(), {}, 50000000)) 
			{
				return false;
			}

			// Vertex data - skybox
			if (!mSkybox.loadFromFile("../Data/Models/cube.obj", false, false, false, false))
			{
				return false;
			}
			mSkyboxVertexBuffer = mLogicalDevice->createBuffer(mSkybox.size(), VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT);
			if (mSkyboxVertexBuffer == nullptr || !mSkyboxVertexBuffer->isInitialized())
			{
				return false;
			}
			mSkyboxVertexBufferMemory = mSkyboxVertexBuffer->allocateAndBindMemoryBlock(VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);
			if (mSkyboxVertexBufferMemory == nullptr || !mSkyboxVertexBufferMemory->isInitialized())
			{
				return false;
			}
			if (!mGraphicsQueue->useStagingBufferToUpdateBufferAndWait(mSkybox.size(), &mSkybox.data[0], mSkyboxVertexBuffer.get(),
				0, 0, VK_ACCESS_VERTEX_ATTRIBUTE_READ_BIT, VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT, VK_PIPELINE_STAGE_VERTEX_INPUT_BIT,
				mFramesResources.front().mCommandBuffer.get(), {}, 50000000))
			{
				return false;
			}

			// Staging buffer & Uniform buffer
			mStagingBuffer = mLogicalDevice->createBuffer(2 * 16 * sizeof(float), VK_BUFFER_USAGE_TRANSFER_SRC_BIT);
			if (mStagingBuffer == nullptr || !mStagingBuffer->isInitialized())
			{
				return false;
			}
			mStagingBufferMemory = mStagingBuffer->allocateAndBindMemoryBlock(VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT);
			if (mStagingBufferMemory == nullptr || !mStagingBufferMemory->isInitialized())
			{
				return false;
			}
			mUniformBuffer = mLogicalDevice->createBuffer(2 * 16 * sizeof(float), VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT);
			if (mUniformBuffer == nullptr || !mUniformBuffer->isInitialized())
			{
				return false;
			}
			mUniformBufferMemory = mUniformBuffer->allocateAndBindMemoryBlock(VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);
			if (mUniformBufferMemory == nullptr || !mUniformBufferMemory->isInitialized())
			{
				return false;
			}
			if (!updateStagingBuffer(true)) 
			{
				return false;
			}

			// Cubemap
			mSkyboxTexture = nu::Vulkan::ImageHelper::createCombinedImageSampler(*mLogicalDevice, VK_IMAGE_TYPE_2D, VK_FORMAT_R8G8B8A8_UNORM, { 1024, 1024, 1 }, 1, 6,
				VK_IMAGE_USAGE_SAMPLED_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT, true, VK_IMAGE_VIEW_TYPE_CUBE, VK_IMAGE_ASPECT_COLOR_BIT, VK_FILTER_LINEAR,
				VK_FILTER_LINEAR, VK_SAMPLER_MIPMAP_MODE_NEAREST, VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE, VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE,
				VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE, 0.0f, 0.0f, 1.0f, false, 1.0f, false, VK_COMPARE_OP_ALWAYS, VK_BORDER_COLOR_FLOAT_OPAQUE_BLACK, false);
			std::vector<std::string> cubemapImages = {
				"../Data/Textures/Skansen/posx.jpg",
				"../Data/Textures/Skansen/negx.jpg",
				"../Data/Textures/Skansen/posy.jpg",
				"../Data/Textures/Skansen/negy.jpg",
				"../Data/Textures/Skansen/posz.jpg",
				"../Data/Textures/Skansen/negz.jpg"
			};

			for (size_t i = 0; i < cubemapImages.size(); i++) 
			{
				std::vector<unsigned char> cubemapImageData;
				int imageDataSize;
				if (!loadTextureDataFromFile(cubemapImages[i].c_str(), 4, cubemapImageData, nullptr, nullptr, nullptr, &imageDataSize)) 
				{
					return false;
				}
				VkImageSubresourceLayers imageSubresource = {
					VK_IMAGE_ASPECT_COLOR_BIT,    // VkImageAspectFlags     aspectMask
					0,                            // uint32_t               mipLevel
					static_cast<uint32_t>(i),     // uint32_t               baseArrayLayer
					1                             // uint32_t               layerCount
				};
				mGraphicsQueue->useStagingBufferToUpdateImageAndWait(imageDataSize, &cubemapImageData[0],
					mSkyboxTexture->getImage(), imageSubresource, { 0, 0, 0 }, { 1024, 1024, 1 }, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL,
					0, VK_ACCESS_SHADER_READ_BIT, VK_IMAGE_ASPECT_COLOR_BIT, VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT, VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT,
					mFramesResources.front().mCommandBuffer.get(), {}, 50000000);
			}

			// Descriptor sets with uniform buffer
			std::vector<VkDescriptorSetLayoutBinding> descriptorSetLayoutBindings = {
				{
					0,                                          // uint32_t             binding
					VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,          // VkDescriptorType     descriptorType
					1,                                          // uint32_t             descriptorCount
					VK_SHADER_STAGE_VERTEX_BIT,                 // VkShaderStageFlags   stageFlags
					nullptr                                     // const VkSampler    * pImmutableSamplers
				},
				{
					1,                                          // uint32_t             binding
					VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,  // VkDescriptorType     descriptorType
					1,                                          // uint32_t             descriptorCount
					VK_SHADER_STAGE_FRAGMENT_BIT,               // VkShaderStageFlags   stageFlags
					nullptr                                     // const VkSampler    * pImmutableSamplers
				}
			};
			mDescriptorSetLayout = mLogicalDevice->createDescriptorSetLayout(descriptorSetLayoutBindings);
			if (mDescriptorSetLayout == nullptr || !mDescriptorSetLayout->isInitialized())
			{
				return false;
			}

			std::vector<VkDescriptorPoolSize> descriptorPoolSizes = {
				{
					VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,          // VkDescriptorType     type
					1                                           // uint32_t             descriptorCount
				},
				{
					VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,  // VkDescriptorType     type
					1                                           // uint32_t             descriptorCount
				}
			};
			mDescriptorPool = mLogicalDevice->createDescriptorPool(false, 1, descriptorPoolSizes);
			if (mDescriptorPool == nullptr || !mDescriptorPool->isInitialized())
			{
				return false;
			}

			// TODO : Allocate more than one at once
			mDescriptorSets.resize(1);
			mDescriptorSets[0] = mDescriptorPool->allocateDescriptorSet(mDescriptorSetLayout.get());
			if (mDescriptorSets[0] == nullptr || !mDescriptorSets[0]->isInitialized())
			{
				return false;
			}

			nu::Vulkan::BufferDescriptorInfo bufferDescriptorUpdate = {
				mDescriptorSets[0]->getHandle(),            // VkDescriptorSet                      TargetDescriptorSet
				0,                                          // uint32_t                             TargetDescriptorBinding
				0,                                          // uint32_t                             TargetArrayElement
				VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,          // VkDescriptorType                     TargetDescriptorType
				{                                           // std::vector<VkDescriptorBufferInfo>  BufferInfos
					{
						mUniformBuffer->getHandle(),              // VkBuffer                             buffer
						0,                                        // VkDeviceSize                         offset
						VK_WHOLE_SIZE                             // VkDeviceSize                         range
					}
				}
			};
			nu::Vulkan::ImageDescriptorInfo imageDescriptorUpdate = {
				mDescriptorSets[0]->getHandle(),            // VkDescriptorSet                      TargetDescriptorSet
				1,                                          // uint32_t                             TargetDescriptorBinding
				0,                                          // uint32_t                             TargetArrayElement
				VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,  // VkDescriptorType                     TargetDescriptorType
				{                                           // std::vector<VkDescriptorImageInfo>   ImageInfos
					{
						mSkyboxTexture->getSampler()->getHandle(),      // VkSampler                            sampler
						mSkyboxTexture->getImageView()->getHandle(),    // VkImageView                          imageView
						VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL  // VkImageLayout                        imageLayout
					}
				}
			};

			mLogicalDevice->updateDescriptorSets({ imageDescriptorUpdate }, { bufferDescriptorUpdate }, {}, {});

			// Render pass
			std::vector<VkAttachmentDescription> attachmentDescriptions = {
				{
					0,                                                // VkAttachmentDescriptionFlags     flags
					mSwapchain->getFormat(),                          // VkFormat                         format
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
					mSwapchain->getDepthFormat(),                     // VkFormat                         format
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

			std::vector<nu::Vulkan::SubpassParameters> subpassParameters = {
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

			mRenderPass = mLogicalDevice->createRenderPass(attachmentDescriptions, subpassParameters, subpassDependencies);
			if (mRenderPass == nullptr || !mRenderPass->isInitialized())
			{
				return false;
			}

			// Graphics pipeline

			// Common

			std::vector<VkPushConstantRange> pushConstantRanges = {
				{
					VK_SHADER_STAGE_FRAGMENT_BIT,     // VkShaderStageFlags   stageFlags
					0,								  // uint32_t			  offset
					sizeof(float) * 4				  // uint32_t             size
				}
			};

			mPipelineLayout = mLogicalDevice->createPipelineLayout({ mDescriptorSetLayout->getHandle() }, pushConstantRanges);
			if (mPipelineLayout == nullptr || !mPipelineLayout->isInitialized())
			{
				return false;
			}

			mPipelines.resize(PipelineNames::Count);
			mPipelines[PipelineNames::MeshPipeline] = mLogicalDevice->initGraphicsPipeline(*mPipelineLayout, *mRenderPass, nullptr);
			mPipelines[PipelineNames::SkyboxPipeline] = mLogicalDevice->initGraphicsPipeline(*mPipelineLayout, *mRenderPass, nullptr);

			nu::Vulkan::GraphicsPipeline* modelPipeline = mPipelines[PipelineNames::MeshPipeline].get();
			nu::Vulkan::GraphicsPipeline* skyboxPipeline = mPipelines[PipelineNames::SkyboxPipeline].get();

			modelPipeline->setSubpass(0);
			skyboxPipeline->setSubpass(0);

			// Model

			nu::Vulkan::ShaderModule::Ptr modelVertexShaderModule = mLogicalDevice->initShaderModule();
			if (modelVertexShaderModule == nullptr || !modelVertexShaderModule->loadFromFile("../Examples/4 - Reflective and Refractive Geometry Using Cubemaps/modelShader.vert.spv"))
			{
				return false;
			}
			modelVertexShaderModule->setVertexEntrypointName("main");

			nu::Vulkan::ShaderModule::Ptr modelFragmentShaderModule = mLogicalDevice->initShaderModule();
			if (modelFragmentShaderModule == nullptr || !modelFragmentShaderModule->loadFromFile("../Examples/4 - Reflective and Refractive Geometry Using Cubemaps/modelShader.frag.spv"))
			{
				return false;
			}
			modelFragmentShaderModule->setFragmentEntrypointName("main");

			if (!modelVertexShaderModule->create() || !modelFragmentShaderModule->create())
			{
				return false;
			}

			modelPipeline->addShaderModule(modelVertexShaderModule.get());
			modelPipeline->addShaderModule(modelFragmentShaderModule.get());

			modelPipeline->addVertexBinding(0, 6 * sizeof(float), VK_VERTEX_INPUT_RATE_VERTEX);
			modelPipeline->addVertexAttribute(0, 0, VK_FORMAT_R32G32B32_SFLOAT, 0);
			modelPipeline->addVertexAttribute(1, 0, VK_FORMAT_R32G32B32_SFLOAT, 3 * sizeof(float));

			// Skybox

			nu::Vulkan::ShaderModule::Ptr skyboxVertexShaderModule = mLogicalDevice->initShaderModule();
			if (skyboxVertexShaderModule == nullptr || !skyboxVertexShaderModule->loadFromFile("../Examples/4 - Reflective and Refractive Geometry Using Cubemaps/skyboxShader.vert.spv"))
			{
				return false;
			}
			skyboxVertexShaderModule->setVertexEntrypointName("main");

			nu::Vulkan::ShaderModule::Ptr skyboxFragmentShaderModule = mLogicalDevice->initShaderModule();
			if (skyboxFragmentShaderModule == nullptr || !skyboxFragmentShaderModule->loadFromFile("../Examples/4 - Reflective and Refractive Geometry Using Cubemaps/skyboxShader.frag.spv"))
			{
				return false;
			}
			skyboxFragmentShaderModule->setFragmentEntrypointName("main");

			if (!skyboxVertexShaderModule->create() || !skyboxFragmentShaderModule->create())
			{
				return false;
			}

			skyboxPipeline->addShaderModule(skyboxVertexShaderModule.get());
			skyboxPipeline->addShaderModule(skyboxFragmentShaderModule.get());

			skyboxPipeline->addVertexBinding(0, 3 * sizeof(float), VK_VERTEX_INPUT_RATE_VERTEX);
			skyboxPipeline->addVertexAttribute(0, 0, VK_FORMAT_R32G32B32_SFLOAT, 0);

			// Common

			modelPipeline->setViewport(0.0f, 0.0f, 500.0f, 500.0f, 0.0f, 1.0f);
			modelPipeline->setScissor(0, 0, 500, 500);
			skyboxPipeline->setViewport(0.0f, 0.0f, 500.0f, 500.0f, 0.0f, 1.0f);
			skyboxPipeline->setScissor(0, 0, 500, 500);

			// Model

			//modelPipeline->setRasterizationState(false, false, VK_POLYGON_MODE_FILL, VK_CULL_MODE_BACK_BIT, VK_FRONT_FACE_COUNTER_CLOCKWISE, false, 0.0f, 0.0f, 0.0f, 1.0f);

			// Skybox

			skyboxPipeline->setRasterizationState(false, false, VK_POLYGON_MODE_FILL, VK_CULL_MODE_FRONT_BIT, VK_FRONT_FACE_COUNTER_CLOCKWISE, false, 0.0f, 0.0f, 0.0f, 1.0f);

			// Common

			modelPipeline->addDynamicState(VK_DYNAMIC_STATE_VIEWPORT);
			modelPipeline->addDynamicState(VK_DYNAMIC_STATE_SCISSOR);
			skyboxPipeline->addDynamicState(VK_DYNAMIC_STATE_VIEWPORT);
			skyboxPipeline->addDynamicState(VK_DYNAMIC_STATE_SCISSOR);

			// TODO : Create at the same time
			if (!modelPipeline->create() || !skyboxPipeline->create())
			{
				return false;
			}

			return true;
		}

		virtual bool draw() override 
		{
			auto prepareFrame = [&](nu::Vulkan::CommandBuffer* commandBuffer, uint32_t swapchainImageIndex, nu::Vulkan::Framebuffer* framebuffer) 
			{
				if (!commandBuffer->beginRecording(VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT, nullptr))
				{
					return false;
				}

				if (mUpdateUniformBuffer)
				{
					mUpdateUniformBuffer = false;

					nu::Vulkan::BufferTransition preTransferTransition = {
						mUniformBuffer.get(),         // Buffer*          buffer
						VK_ACCESS_UNIFORM_READ_BIT,   // VkAccessFlags    currentAccess
						VK_ACCESS_TRANSFER_WRITE_BIT, // VkAccessFlags    newAccess
						VK_QUEUE_FAMILY_IGNORED,      // uint32_t         currentQueueFamily
						VK_QUEUE_FAMILY_IGNORED       // uint32_t         newQueueFamily
					};
					commandBuffer->setBufferMemoryBarrier(VK_PIPELINE_STAGE_VERTEX_SHADER_BIT, VK_PIPELINE_STAGE_TRANSFER_BIT, { preTransferTransition });
			
					std::vector<VkBufferCopy> regions = {
						{
							0,                        // VkDeviceSize     srcOffset
							0,                        // VkDeviceSize     dstOffset
							2 * 16 * sizeof(float)    // VkDeviceSize     size
						}
					};
					commandBuffer->copyDataBetweenBuffers(mStagingBuffer.get(), mUniformBuffer.get(), regions);

					nu::Vulkan::BufferTransition postTransferTransition = {
						mUniformBuffer.get(),         // Buffer*          buffer
						VK_ACCESS_TRANSFER_WRITE_BIT, // VkAccessFlags    currentAccess
						VK_ACCESS_UNIFORM_READ_BIT,   // VkAccessFlags    newAccess
						VK_QUEUE_FAMILY_IGNORED,      // uint32_t         currentQueueFamily
						VK_QUEUE_FAMILY_IGNORED       // uint32_t         newQueueFamily
					};
					commandBuffer->setBufferMemoryBarrier(VK_PIPELINE_STAGE_TRANSFER_BIT, VK_PIPELINE_STAGE_VERTEX_SHADER_BIT, { postTransferTransition });
				}

				if (mPresentQueue->getFamilyIndex() != mGraphicsQueue->getFamilyIndex()) 
				{
					nu::Vulkan::ImageTransition imageTransitionBeforeDrawing = {
						mSwapchain->getImage(swapchainImageIndex), // VkImage             image
						VK_ACCESS_MEMORY_READ_BIT,                // VkAccessFlags        currentAccess
						VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT,     // VkAccessFlags        newAccess
						VK_IMAGE_LAYOUT_UNDEFINED,                // VkImageLayout        currentLayout
						VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL, // VkImageLayout        newLayout
						mPresentQueue->getFamilyIndex(),          // uint32_t             currentQueueFamily
						mGraphicsQueue->getFamilyIndex(),         // uint32_t             newQueueFamily
						VK_IMAGE_ASPECT_COLOR_BIT                 // VkImageAspectFlags   aspect
					};
					commandBuffer->setImageMemoryBarrier(VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT, VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT, { imageTransitionBeforeDrawing } );
				}

				// Drawing
				commandBuffer->beginRenderPass(mRenderPass->getHandle(), framebuffer->getHandle(), { { 0, 0 }, mSwapchain->getSize() }, { { 0.1f, 0.2f, 0.3f, 1.0f },{ 1.0f, 0 } }, VK_SUBPASS_CONTENTS_INLINE);

				uint32_t width = mSwapchain->getSize().width;
				uint32_t height = mSwapchain->getSize().height;

				VkViewport viewport = {
					0.0f,                                       // float    x
					0.0f,                                       // float    y
					static_cast<float>(width),                  // float    width
					static_cast<float>(height),                 // float    height
					0.0f,                                       // float    minDepth
					1.0f,                                       // float    maxDepth
				};
				commandBuffer->setViewportStateDynamically(0, { viewport });

				VkRect2D scissor = {
					{                                           // VkOffset2D     offset
						0,                                            // int32_t        x
						0                                             // int32_t        y
					},
					{                                           // VkExtent2D     extent
						width,                                      // uint32_t       width
						height                                      // uint32_t       height
					}
				};
				commandBuffer->setScissorStateDynamically(0, { scissor });

				commandBuffer->bindDescriptorSets(VK_PIPELINE_BIND_POINT_GRAPHICS, mPipelineLayout->getHandle(), 0, { mDescriptorSets[0].get() }, {});

				// Draw model

				commandBuffer->bindPipeline(mPipelines[PipelineNames::MeshPipeline].get());
				commandBuffer->bindVertexBuffers(0, { { mMeshVertexBuffer.get(), 0 } });
				std::array<float, 4> position = { 0.0f, 0.0f, -4.0f, 0.0f };
				commandBuffer->provideDataToShadersThroughPushConstants(mPipelineLayout->getHandle(), VK_SHADER_STAGE_FRAGMENT_BIT, 0, sizeof(float) * 4, &position[0]); // TODO : Camera position
				for (size_t i = 0; i < mMesh.parts.size(); i++) 
				{
					commandBuffer->drawGeometry(mMesh.parts[i].vertexCount, 1, mMesh.parts[i].vertexOffset, 0);
				}

				// Draw skybox

				commandBuffer->bindPipeline(mPipelines[PipelineNames::SkyboxPipeline].get());
				commandBuffer->bindVertexBuffers(0, { { mSkyboxVertexBuffer.get(), 0 } });
				for (size_t i = 0; i < mSkybox.parts.size(); i++)
				{
					commandBuffer->drawGeometry(mSkybox.parts[i].vertexCount, 1, mSkybox.parts[i].vertexOffset, 0);
				}

				commandBuffer->endRenderPass();

				if (mPresentQueue->getFamilyIndex() != mGraphicsQueue->getFamilyIndex())
				{
					nu::Vulkan::ImageTransition imageTransitionBeforePresent = {
						mSwapchain->getImage(swapchainImageIndex),  // VkImage            image
						VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT,     // VkAccessFlags        currentAccess
						VK_ACCESS_MEMORY_READ_BIT,                // VkAccessFlags        newAccess
						VK_IMAGE_LAYOUT_PRESENT_SRC_KHR,          // VkImageLayout        currentLayout
						VK_IMAGE_LAYOUT_PRESENT_SRC_KHR,          // VkImageLayout        newLayout
						mGraphicsQueue->getFamilyIndex(),         // uint32_t             currentQueueFamily
						mPresentQueue->getFamilyIndex(),          // uint32_t             newQueueFamily
						VK_IMAGE_ASPECT_COLOR_BIT                 // VkImageAspectFlags   aspect
					};
					commandBuffer->setImageMemoryBarrier(VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT, VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT, { imageTransitionBeforePresent } );
				}

				return commandBuffer->endRecording();
			};

			FrameResources& currentFrame = mFramesResources[mFrameIndex];

			// TODO : Fence warning might come from here : "vkWaitForFences called for fence 0x... which has not been submitted on a Queue or during acquire next image."
			if (!currentFrame.mDrawingFinishedFence->wait(2000000000))
			{
				return false;
			}

			if (!currentFrame.mDrawingFinishedFence->reset())
			{
				return false;
			}

			uint32_t imageIndex;
			if (!mSwapchain->acquireImageIndex(2000000000, currentFrame.mImageAcquiredSemaphore->getHandle(), VK_NULL_HANDLE, imageIndex))
			{
				return false;
			}

			std::vector<VkImageView> attachments = { mSwapchain->getImageView(imageIndex) };
			if (currentFrame.mDepthAttachment != nullptr)
			{
				attachments.push_back(currentFrame.mDepthAttachment->getHandle());
			}

			// TODO : Only create once
			currentFrame.mFramebuffer = mRenderPass->createFramebuffer(attachments, mSwapchain->getSize().width, mSwapchain->getSize().height, 1);
			if (currentFrame.mFramebuffer == nullptr || !currentFrame.mFramebuffer->isInitialized())
			{
				return false;
			}

			if (!prepareFrame(currentFrame.mCommandBuffer.get(), imageIndex, currentFrame.mFramebuffer.get()))
			{
				return false;
			}

			std::vector<WaitSemaphoreInfo> waitSemaphoreInfos = {};
			waitSemaphoreInfos.push_back({
				currentFrame.mImageAcquiredSemaphore->getHandle(),  // VkSemaphore            Semaphore
				VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT       // VkPipelineStageFlags   WaitingStage
			});
			if (!mGraphicsQueue->submitCommandBuffers({ currentFrame.mCommandBuffer.get() }, waitSemaphoreInfos, { currentFrame.mReadyToPresentSemaphore->getHandle() }, currentFrame.mDrawingFinishedFence.get()))
			{
				return false;
			}

			PresentInfo presentInfo = {
				mSwapchain->getHandle(), // VkSwapchainKHR         Swapchain
				imageIndex               // uint32_t               ImageIndex
			};
			if (!mPresentQueue->presentImage({ currentFrame.mReadyToPresentSemaphore->getHandle() }, { presentInfo }))
			{
				return false;
			}

			mFrameIndex = (mFrameIndex + 1) % mFramesResources.size();
			return true;
		}

		void onMouseEvent() 
		{
			updateStagingBuffer(false);
		}

		bool updateStagingBuffer(bool force)
		{
			mUpdateUniformBuffer = true;
			static float horizontalAngle = 0.0f;
			static float verticalAngle = 0.0f;

			if (mMouseState.buttons[0].isPressed || force) 
			{
				horizontalAngle += 0.5f * mMouseState.position.delta.x;
				verticalAngle = nu::clamp(verticalAngle - 0.5f * mMouseState.position.delta.y, -90.0f, 90.0f);

				nu::Matrix4f mr1 = nu::Quaternionf(verticalAngle, { -1.0f, 0.0f, 0.0f }).toMatrix4();
				nu::Matrix4f mr2 = nu::Quaternionf(horizontalAngle, { 0.0f, 1.0f, 0.0f }).toMatrix4();
				nu::Matrix4f rotation = mr1 * mr2;
				nu::Matrix4f translation = nu::Matrix4f::translation({ 0.0f, 0.0f, -4.0f });
				nu::Matrix4f modelMatrix = translation * rotation;

				nu::Matrix4f viewMatrix = nu::Matrix4f::lookAt(nu::Vector3f::zero, { 0.0f, 0.0f, -4.0f }, nu::Vector3f::up);

				nu::Matrix4f modelViewMatrix = viewMatrix * modelMatrix;

				if (!mStagingBufferMemory->mapUpdateAndUnmapHostVisibleMemory(0, sizeof(float) * 16, &modelViewMatrix[0], true, nullptr))
				{
					return false;
				}

				nu::Matrix4f perspectiveMatrix = nu::Matrix4f::perspective(50.0f, static_cast<float>(mSwapchain->getSize().width) / static_cast<float>(mSwapchain->getSize().height), 0.5f, 10.0f);

				if (!mStagingBufferMemory->mapUpdateAndUnmapHostVisibleMemory(sizeof(float) * 16, sizeof(float) * 16, &perspectiveMatrix[0], true, nullptr))
				{
					return false;
				}
			}
			return true;
		}

		virtual bool resize() override
		{
			if (!mSwapchain->recreate(mFramesResources))
			{
				return false;
			}

			if (isReady()) 
			{
				if (!updateStagingBuffer(true)) 
				{
					return false;
				}
			}
			return true;
		}
};

#endif // REFLECTIVE_AND_REFRACTIVE_GEOMETRY_USING_CUBEMAPS_HPP