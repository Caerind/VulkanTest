#ifndef RENDERING_TESSELATED_TERRAIN_HPP
#define RENDERING_TESSELATED_TERRAIN_HPP

#include "../../CookBook/SampleBase.hpp"

#include "../../Math/Matrix4.hpp"

#include "../../Mesh.hpp"
#include "../../Window.hpp"

#include "../../VertexBuffer.hpp"
#include "../../StagingBuffer.hpp"
#include "../../UniformBuffer.hpp"

class RenderingTesselatedTerrain : public SampleBase
{
	public:
		nu::Mesh mModel;
		nu::VertexBuffer::Ptr mModelVertexBuffer;

		nu::Vulkan::ImageHelper::Ptr mHeightMap;

		nu::UniformBuffer::Ptr mUniformBuffer;
		nu::StagingBuffer::Ptr mStagingBuffer;

		nu::Vulkan::DescriptorSetLayout::Ptr mDescriptorSetLayout;
		nu::Vulkan::DescriptorPool::Ptr mDescriptorPool;
		std::vector<nu::Vulkan::DescriptorSet::Ptr> mDescriptorSets;

		nu::Vulkan::RenderPass::Ptr mRenderPass;
		nu::Vulkan::PipelineLayout::Ptr mPipelineLayout; 
		std::vector<nu::Vulkan::GraphicsPipeline::Ptr> mPipelines;
		enum PipelineNames
		{
			SolidTerrainPipeline = 0,
			LineTerrainPipeline = 1,
			Count
		};
		uint32_t mCurrentPipeline;

		uint32_t mFrameIndex = 0;

		virtual bool initialize(nu::Vulkan::WindowParameters windowParameters) override 
		{
			mCurrentPipeline = 0;

			VkPhysicalDeviceFeatures deviceFeatures = {};
			deviceFeatures.tessellationShader = true;
			deviceFeatures.geometryShader = true;
			deviceFeatures.fillModeNonSolid = true;

			if (!initializeVulkan(windowParameters, &deviceFeatures)) 
			{
				return false;
			}

			// Vertex data
			if (!mModel.loadFromFile("../Data/Models/plane.obj", false, true, false, false))
			{
				return false;
			}
			mModelVertexBuffer = nu::VertexBuffer::createVertexBuffer(*mLogicalDevice, mModel.size());
			if (!mModelVertexBuffer || !mModelVertexBuffer->updateAndWait(mModel.size(), &mModel.data[0], 0, 0, VK_ACCESS_VERTEX_ATTRIBUTE_READ_BIT, VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT, VK_PIPELINE_STAGE_VERTEX_INPUT_BIT, mFramesResources.front().mCommandBuffer.get(), mGraphicsQueue.get(), {}, 50000000))
			{
				return false;
			}

			// Staging buffer & Uniform buffer
			mUniformBuffer = nu::UniformBuffer::createUniformBuffer(*mLogicalDevice, 2 * 16 * sizeof(float));
			if (mUniformBuffer == nullptr)
			{
				return false;
			}
			mStagingBuffer = mUniformBuffer->createStagingBuffer();
			if (mStagingBuffer == nullptr)
			{
				return false;
			}
			if (!updateStagingBuffer(true)) 
			{
				return false;
			}

			// Combined Image sampler
			int width = 1;
			int height = 1;
			std::vector<unsigned char> imageData;
			if (!loadTextureDataFromFile("../Data/Textures/heightmap.png", 4, imageData, &width, &height))
			{
				return false;
			}

			mHeightMap = nu::Vulkan::ImageHelper::createCombinedImageSampler(*mLogicalDevice, VK_IMAGE_TYPE_2D, VK_FORMAT_R8G8B8A8_UNORM, { (uint32_t)width, (uint32_t)height, 1 }, 1, 1,
				VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT, false, VK_IMAGE_VIEW_TYPE_2D, VK_IMAGE_ASPECT_COLOR_BIT, VK_FILTER_LINEAR,
				VK_FILTER_LINEAR, VK_SAMPLER_MIPMAP_MODE_NEAREST, VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE, VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE,
				VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE, 0.0f, 0.0f, 1.0f, false, 1.0f, false, VK_COMPARE_OP_ALWAYS, VK_BORDER_COLOR_FLOAT_OPAQUE_BLACK,
				false);
			if (mHeightMap == nullptr || !mHeightMap->hasSampler() || mHeightMap->getImage() == nullptr || mHeightMap->getMemoryBlock() == nullptr || mHeightMap->getImageView() == nullptr)
			{
				return false;
			}
			VkImageSubresourceLayers imageSubresourceLayer = {
				VK_IMAGE_ASPECT_COLOR_BIT,    // VkImageAspectFlags     aspectMask
				0,                            // uint32_t               mipLevel
				0,                            // uint32_t               baseArrayLayer
				1                             // uint32_t               layerCount
			};
			if (!mGraphicsQueue->useStagingBufferToUpdateImageAndWait(imageData.size(), &imageData[0], mHeightMap->getImage(),
				imageSubresourceLayer, { 0, 0, 0 }, { (uint32_t)width, (uint32_t)height, 1 }, VK_IMAGE_LAYOUT_UNDEFINED,
				VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL, 0, VK_ACCESS_SHADER_READ_BIT, VK_IMAGE_ASPECT_COLOR_BIT, VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT,
				VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT, mFramesResources.front().mCommandBuffer.get(), {}, 50000000))
			{
				return false;
			}

			// Descriptor sets with uniform buffer
			std::vector<VkDescriptorSetLayoutBinding> descriptorSetLayoutBindings = {
				{
					0,                                          // uint32_t             binding
					VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,          // VkDescriptorType     descriptorType
					1,                                          // uint32_t             descriptorCount
					VK_SHADER_STAGE_TESSELLATION_CONTROL_BIT |  // VkShaderStageFlags   stageFlags
					VK_SHADER_STAGE_GEOMETRY_BIT,
					nullptr                                     // const VkSampler    * pImmutableSamplers
				},
				{
					1,                                          // uint32_t             binding
					VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,  // VkDescriptorType     descriptorType
					1,                                          // uint32_t             descriptorCount
					VK_SHADER_STAGE_TESSELLATION_CONTROL_BIT |  // VkShaderStageFlags   stageFlags
					VK_SHADER_STAGE_TESSELLATION_EVALUATION_BIT,
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

			// Update descriptor
			// TODO : Update more than one at once
			mUniformBuffer->updateDescriptor(mDescriptorSets[0].get(), 0, 0);

			nu::Vulkan::ImageDescriptorInfo imageDescriptorUpdate = {
				mDescriptorSets[0]->getHandle(),            // VkDescriptorSet                      TargetDescriptorSet
				1,                                          // uint32_t                             TargetDescriptorBinding
				0,                                          // uint32_t                             TargetArrayElement
				VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,  // VkDescriptorType                     TargetDescriptorType
				{                                           // std::vector<VkDescriptorImageInfo>   ImageInfos
					{
						mHeightMap->getSampler()->getHandle(),    // VkSampler                            sampler
						mHeightMap->getImageView()->getHandle(),  // VkImageView                          imageView
						VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL  // VkImageLayout                        imageLayout
					}
				}
			};

			mLogicalDevice->updateDescriptorSets({ imageDescriptorUpdate }, {}, {}, {});

			// Render pass
			mRenderPass = mLogicalDevice->initRenderPass();

			mRenderPass->addAttachment(mSwapchain->getFormat());
			mRenderPass->setAttachmentLoadOp(VK_ATTACHMENT_LOAD_OP_CLEAR);
			mRenderPass->setAttachmentStoreOp(VK_ATTACHMENT_STORE_OP_STORE);
			mRenderPass->setAttachmentFinalLayout(VK_IMAGE_LAYOUT_PRESENT_SRC_KHR);

			mRenderPass->addAttachment(mSwapchain->getDepthFormat());
			mRenderPass->setAttachmentLoadOp(VK_ATTACHMENT_LOAD_OP_CLEAR);
			mRenderPass->setAttachmentFinalLayout(VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL);

			mRenderPass->addSubpass(VK_PIPELINE_BIND_POINT_GRAPHICS);
			mRenderPass->addColorAttachmentToSubpass(0, VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL);
			mRenderPass->addDepthStencilAttachmentToSubpass(1, VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL);

			mRenderPass->addDependency(
				VK_SUBPASS_EXTERNAL,                            // uint32_t                   srcSubpass
				0,                                              // uint32_t                   dstSubpass
				VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT,              // VkPipelineStageFlags       srcStageMask
				VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,  // VkPipelineStageFlags       dstStageMask
				VK_ACCESS_MEMORY_READ_BIT,                      // VkAccessFlags              srcAccessMask
				VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT,           // VkAccessFlags              dstAccessMask
				VK_DEPENDENCY_BY_REGION_BIT                     // VkDependencyFlags          dependencyFlags
			);
			mRenderPass->addDependency(
				0,                                              // uint32_t                   srcSubpass
				VK_SUBPASS_EXTERNAL,                            // uint32_t                   dstSubpass
				VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,  // VkPipelineStageFlags       srcStageMask
				VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT,              // VkPipelineStageFlags       dstStageMask
				VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT,           // VkAccessFlags              srcAccessMask
				VK_ACCESS_MEMORY_READ_BIT,                      // VkAccessFlags              dstAccessMask
				VK_DEPENDENCY_BY_REGION_BIT                     // VkDependencyFlags          dependencyFlags
			);

			if (!mRenderPass->create())
			{
				return false;
			}

			// Graphics pipeline

			mPipelineLayout = mLogicalDevice->createPipelineLayout({ mDescriptorSetLayout->getHandle() }, {});
			if (mPipelineLayout == nullptr || !mPipelineLayout->isInitialized())
			{
				return false;
			}

			mPipelines.resize(PipelineNames::Count);
			mPipelines[PipelineNames::SolidTerrainPipeline] = mLogicalDevice->initGraphicsPipeline(*mPipelineLayout, *mRenderPass, nullptr);
			mPipelines[PipelineNames::LineTerrainPipeline] = mLogicalDevice->initGraphicsPipeline(*mPipelineLayout, *mRenderPass, nullptr);

			nu::Vulkan::GraphicsPipeline* solidTerrainPipeline = mPipelines[PipelineNames::SolidTerrainPipeline].get();
			nu::Vulkan::GraphicsPipeline* lineTerrainPipeline = mPipelines[PipelineNames::LineTerrainPipeline].get();

			solidTerrainPipeline->setSubpass(0);
			lineTerrainPipeline->setSubpass(0);

			nu::Vulkan::ShaderModule::Ptr terrainVertexShaderModule = mLogicalDevice->initShaderModule();
			if (terrainVertexShaderModule == nullptr || !terrainVertexShaderModule->loadFromFile("../Examples/9 - Rendering Tesselated Terrain/shader.vert.spv"))
			{
				return false;
			}
			terrainVertexShaderModule->setVertexEntrypointName("main");

			nu::Vulkan::ShaderModule::Ptr terrainTessellationControlShaderModule = mLogicalDevice->initShaderModule();
			if (terrainTessellationControlShaderModule == nullptr || !terrainTessellationControlShaderModule->loadFromFile("../Examples/9 - Rendering Tesselated Terrain/shader.tesc.spv"))
			{
				return false;
			}
			terrainTessellationControlShaderModule->setTessellationControlEntrypointName("main");

			nu::Vulkan::ShaderModule::Ptr terrainTessellationEvaluationShaderModule = mLogicalDevice->initShaderModule();
			if (terrainTessellationEvaluationShaderModule == nullptr || !terrainTessellationEvaluationShaderModule->loadFromFile("../Examples/9 - Rendering Tesselated Terrain/shader.tese.spv"))
			{
				return false;
			}
			terrainTessellationEvaluationShaderModule->setTessellationEvaluationEntrypointName("main");

			nu::Vulkan::ShaderModule::Ptr terrainGeometryShaderModule = mLogicalDevice->initShaderModule();
			if (terrainGeometryShaderModule == nullptr || !terrainGeometryShaderModule->loadFromFile("../Examples/9 - Rendering Tesselated Terrain/shader.geom.spv"))
			{
				return false;
			}
			terrainGeometryShaderModule->setGeometryEntrypointName("main");

			nu::Vulkan::ShaderModule::Ptr terrainFragmentShaderModule = mLogicalDevice->initShaderModule();
			if (terrainFragmentShaderModule == nullptr || !terrainFragmentShaderModule->loadFromFile("../Examples/9 - Rendering Tesselated Terrain/shader.frag.spv"))
			{
				return false;
			}
			terrainFragmentShaderModule->setFragmentEntrypointName("main");

			if (!terrainVertexShaderModule->create() || !terrainTessellationControlShaderModule->create() || !terrainTessellationEvaluationShaderModule->create() || !terrainGeometryShaderModule->create() || !terrainFragmentShaderModule->create())
			{
				return false;
			}

			solidTerrainPipeline->addShaderModule(terrainVertexShaderModule.get());
			solidTerrainPipeline->addShaderModule(terrainTessellationControlShaderModule.get());
			solidTerrainPipeline->addShaderModule(terrainTessellationEvaluationShaderModule.get());
			solidTerrainPipeline->addShaderModule(terrainGeometryShaderModule.get());
			solidTerrainPipeline->addShaderModule(terrainFragmentShaderModule.get());
			lineTerrainPipeline->addShaderModule(terrainVertexShaderModule.get());
			lineTerrainPipeline->addShaderModule(terrainTessellationControlShaderModule.get());
			lineTerrainPipeline->addShaderModule(terrainTessellationEvaluationShaderModule.get());
			lineTerrainPipeline->addShaderModule(terrainGeometryShaderModule.get());
			lineTerrainPipeline->addShaderModule(terrainFragmentShaderModule.get());

			solidTerrainPipeline->addVertexBinding(0, 5 * sizeof(float), VK_VERTEX_INPUT_RATE_VERTEX);
			solidTerrainPipeline->addVertexAttribute(0, 0, VK_FORMAT_R32G32B32_SFLOAT, 0);
			solidTerrainPipeline->addVertexAttribute(1, 0, VK_FORMAT_R32G32_SFLOAT, 3 * sizeof(float));
			lineTerrainPipeline->addVertexBinding(0, 5 * sizeof(float), VK_VERTEX_INPUT_RATE_VERTEX);
			lineTerrainPipeline->addVertexAttribute(0, 0, VK_FORMAT_R32G32B32_SFLOAT, 0);
			lineTerrainPipeline->addVertexAttribute(1, 0, VK_FORMAT_R32G32_SFLOAT, 3 * sizeof(float));

			solidTerrainPipeline->setRasterizationState(false, false, VK_POLYGON_MODE_FILL, VK_CULL_MODE_BACK_BIT, VK_FRONT_FACE_COUNTER_CLOCKWISE, false, 0.0f, 0.0f, 0.0f, 1.0f);
			lineTerrainPipeline->setRasterizationState(false, false, VK_POLYGON_MODE_LINE, VK_CULL_MODE_BACK_BIT, VK_FRONT_FACE_COUNTER_CLOCKWISE, false, 0.0f, 0.0f, 0.0f, 1.0f);

			solidTerrainPipeline->setTopology(VK_PRIMITIVE_TOPOLOGY_PATCH_LIST);
			solidTerrainPipeline->setPatchControlPoints(3);
			lineTerrainPipeline->setTopology(VK_PRIMITIVE_TOPOLOGY_PATCH_LIST);
			lineTerrainPipeline->setPatchControlPoints(3);

			solidTerrainPipeline->addDynamicState(VK_DYNAMIC_STATE_VIEWPORT);
			solidTerrainPipeline->addDynamicState(VK_DYNAMIC_STATE_SCISSOR);
			lineTerrainPipeline->addDynamicState(VK_DYNAMIC_STATE_VIEWPORT);
			lineTerrainPipeline->addDynamicState(VK_DYNAMIC_STATE_SCISSOR);

			if (!solidTerrainPipeline->create() || !lineTerrainPipeline->create())
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

				if (mStagingBuffer->needToSend())
				{
					mStagingBuffer->send(commandBuffer);
				}

				if (mPresentQueue->getFamilyIndex() != mGraphicsQueue->getFamilyIndex()) 
				{
					nu::Vulkan::ImageTransition imageTransitionBeforeDrawing = {
						mSwapchain->getImageHandle(swapchainImageIndex), // VkImage             image
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

				mModelVertexBuffer->bindTo(commandBuffer, 0, 0);
				commandBuffer->bindDescriptorSets(VK_PIPELINE_BIND_POINT_GRAPHICS, mPipelineLayout->getHandle(), 0, { mDescriptorSets[0].get() }, {});
				commandBuffer->bindPipeline(mPipelines[mCurrentPipeline].get());

				for (size_t i = 0; i < mModel.parts.size(); i++) 
				{
					commandBuffer->drawGeometry(mModel.parts[i].vertexCount, 1, mModel.parts[i].vertexOffset, 0);
				}

				commandBuffer->endRenderPass();

				if (mPresentQueue->getFamilyIndex() != mGraphicsQueue->getFamilyIndex())
				{
					nu::Vulkan::ImageTransition imageTransitionBeforePresent = {
						mSwapchain->getImageHandle(swapchainImageIndex),  // VkImage            image
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

			std::vector<VkImageView> attachments = { mSwapchain->getImageViewHandle(imageIndex) };
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
			if (mMouseState.buttons[1].wasClicked) 
			{
				mCurrentPipeline = 1 - mCurrentPipeline;
			}

			updateStagingBuffer(false);
		}

		bool updateStagingBuffer(bool force)
		{
			static float horizontalAngle = 0.0f;
			static float verticalAngle = 0.0f;

			if (mMouseState.buttons[0].isPressed || force) 
			{
				horizontalAngle += 0.5f * mMouseState.position.delta.x;
				verticalAngle = nu::clamp(verticalAngle - 0.5f * mMouseState.position.delta.y, -90.0f, 90.0f);

				nu::Matrix4f mr1 = nu::Quaternionf(verticalAngle, { -1.0f, 0.0f, 0.0f }).toMatrix4();
				nu::Matrix4f mr2 = nu::Quaternionf(horizontalAngle, { 0.0f, 1.0f, 0.0f }).toMatrix4();
				nu::Matrix4f rotation = mr1 * mr2;
				nu::Matrix4f translation = nu::Matrix4f::translation({ 0.0f, 1.0f, -4.0f });
				nu::Matrix4f modelMatrix = translation * rotation;

				nu::Matrix4f viewMatrix = nu::Matrix4f::lookAt(nu::Vector3f::zero, { 0.0f, 0.0f, -4.0f }, nu::Vector3f::up);

				nu::Matrix4f modelViewMatrix = viewMatrix * modelMatrix;

				if (!mStagingBuffer->mapWriteUnmap(0, sizeof(float) * 16, &modelViewMatrix[0], true, nullptr))
				{
					return false;
				}

				nu::Matrix4f perspectiveMatrix = nu::Matrix4f::perspective(50.0f, static_cast<float>(mSwapchain->getSize().width) / static_cast<float>(mSwapchain->getSize().height), 0.5f, 10.0f);

				if (!mStagingBuffer->mapWriteUnmap(sizeof(float) * 16, sizeof(float) * 16, &perspectiveMatrix[0], true, nullptr))
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

#endif // RENDERING_TESSELATED_TERRAIN_HPP
