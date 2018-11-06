#ifndef DRAWING_PARTICLES_USING_COMPTE_AND_GRAPHICS_PIPELINES_HPP
#define DRAWING_PARTICLES_USING_COMPTE_AND_GRAPHICS_PIPELINES_HPP

#include "../../CookBook/SampleBase.hpp"

#include "../../Math/Matrix4.hpp"

#include "../../Mesh.hpp"
#include "../../Window.hpp"

#include "../../CookBook/OrbitingCamera.hpp"

#include "../../VertexBuffer.hpp"
#include "../../StagingBuffer.hpp"
#include "../../UniformBuffer.hpp"

class DrawingParticlesUsingComputeAndGraphicsPipelines : public SampleBase
{
	public:
		nu::Vulkan::CommandPool::Ptr mComputeCommandPool;
		nu::Vulkan::CommandBuffer::Ptr mComputeCommandBuffer;
		nu::Vulkan::Semaphore::Ptr mComputeSemaphore;
		nu::Vulkan::Fence::Ptr mComputeFence;

		const uint32_t PARTICLES_COUNT = 2000;

		nu::VertexBuffer::Ptr mParticlesVertexBuffer;
		nu::Vulkan::BufferView::Ptr mParticlesVertexBufferView;

		nu::UniformBuffer::Ptr mUniformBuffer;
		nu::StagingBuffer::Ptr mStagingBuffer;

		std::vector<nu::Vulkan::DescriptorSetLayout::Ptr> mDescriptorSetLayouts;
		nu::Vulkan::DescriptorPool::Ptr mDescriptorPool;
		std::vector<nu::Vulkan::DescriptorSet::Ptr> mDescriptorSets;

		nu::Vulkan::RenderPass::Ptr mRenderPass;
		nu::Vulkan::PipelineLayout::Ptr mGraphicsPipelineLayout; 
		std::vector<nu::Vulkan::GraphicsPipeline::Ptr> mGraphicsPipelines;
		enum GraphicsPipelineNames
		{
			ParticlesPipeline = 0,
			Count
		};

		nu::Vulkan::PipelineLayout::Ptr mComputePipelineLayout;
		nu::Vulkan::ComputePipeline::Ptr mComputePipeline;

		uint32_t mFrameIndex = 0;

		virtual bool initialize(nu::Vulkan::WindowParameters windowParameters) override 
		{
			VkPhysicalDeviceFeatures deviceFeatures = {};
			deviceFeatures.geometryShader = true;

			if (!initializeVulkan(windowParameters, &deviceFeatures)) 
			{
				return false;
			}

			// Compute command buffer creation
			mComputeCommandPool = mLogicalDevice->createCommandPool(VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT, mComputeQueue->getFamilyIndex());
			if (mComputeCommandPool == nullptr)
			{
				return false;
			}

			mComputeCommandBuffer = mComputeCommandPool->allocatePrimaryCommandBuffer();

			// Vertex buffer / storage texel buffer
			{
				std::vector<float> particles;
				particles.reserve(PARTICLES_COUNT * sizeof(float) * 8);

				for (uint32_t i = 0; i < PARTICLES_COUNT; i++) 
				{
					OrbitingCamera particle({ 0.0f, 0.0f, 0.0f }, 1.5f, static_cast<float>((std::rand() % 181) - 90), static_cast<float>((std::rand() % 51) - 25));
					const nu::Vector3f& position = particle.getPosition();
					nu::Vector3f color = 0.0075f * nu::Vector3f{
						250.0f - std::abs(particle.getVerticalAngle() * 10.0f),
						static_cast<float>(std::rand() % 61 + 40),
						static_cast<float>(std::rand() % 61)
					};
					float speed = 0.5f + 0.01f * static_cast<float>(std::rand() % 101) + color[0] * 0.5f;
					particles.push_back(position.x);
					particles.push_back(position.y);
					particles.push_back(position.z);
					particles.push_back(1.0f);
					particles.push_back(color.x);
					particles.push_back(color.y);
					particles.push_back(color.z);
					particles.push_back(speed);
				}

				const VkFormatProperties& formatProperties = mLogicalDevice->getPhysicalDevice().getFormatProperties(VK_FORMAT_R32G32B32A32_SFLOAT);
				if (!(formatProperties.bufferFeatures & VK_FORMAT_FEATURE_STORAGE_TEXEL_BUFFER_BIT))
				{
					// TODO : Use Numea System Log
					printf("Provided format is not supported for a storage texel buffer\n");
					return false;
				}

				mParticlesVertexBuffer = nu::VertexBuffer::createVertexBuffer(*mLogicalDevice, (uint32_t)sizeof(particles[0]) * (uint32_t)particles.size());
				if (!mParticlesVertexBuffer || !mParticlesVertexBuffer->updateAndWait((uint32_t)sizeof(particles[0]) * (uint32_t)particles.size(), &particles[0], 0, 0, VK_ACCESS_VERTEX_ATTRIBUTE_READ_BIT, VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT, VK_PIPELINE_STAGE_VERTEX_INPUT_BIT, mFramesResources.front().mCommandBuffer.get(), mGraphicsQueue.get(), {}, 50000000))
				{
					return false;
				}
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

			// Descriptor sets with uniform buffer
			std::vector<VkDescriptorSetLayoutBinding> descriptorSetLayoutBindings = {
				{
					0,                                          // uint32_t             binding
					VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,          // VkDescriptorType     descriptorType
					1,                                          // uint32_t             descriptorCount
					VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_GEOMETRY_BIT,     // VkShaderStageFlags   stageFlags           
					nullptr                                     // const VkSampler    * pImmutableSamplers
				},
				{
					0,											// uint32_t             binding
					VK_DESCRIPTOR_TYPE_STORAGE_TEXEL_BUFFER,    // VkDescriptorType     descriptorType
					1,                                          // uint32_t             descriptorCount
					VK_SHADER_STAGE_COMPUTE_BIT,                // VkShaderStageFlags   stageFlags
					nullptr                                     // const VkSampler    * pImmutableSamplers
				}
			};
			mDescriptorSetLayouts.resize(2);
			mDescriptorSetLayouts[0] = mLogicalDevice->createDescriptorSetLayout({ descriptorSetLayoutBindings[0] });
			if (mDescriptorSetLayouts[0] == nullptr || !mDescriptorSetLayouts[0]->isInitialized())
			{
				return false;
			}
			mDescriptorSetLayouts[1] = mLogicalDevice->createDescriptorSetLayout({ descriptorSetLayoutBindings[1] });
			if (mDescriptorSetLayouts[1] == nullptr || !mDescriptorSetLayouts[1]->isInitialized())
			{
				return false;
			}

			std::vector<VkDescriptorPoolSize> descriptorPoolSizes = {
				{
					VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,          // VkDescriptorType     type
					1                                           // uint32_t             descriptorCount
				},
				{
					VK_DESCRIPTOR_TYPE_STORAGE_TEXEL_BUFFER,    // VkDescriptorType     type
					1                                           // uint32_t             descriptorCount
				}
			};
			mDescriptorPool = mLogicalDevice->createDescriptorPool(false, 2, descriptorPoolSizes);
			if (mDescriptorPool == nullptr || !mDescriptorPool->isInitialized())
			{
				return false;
			}

			// TODO : Allocate more than one at once
			mDescriptorSets.resize(2);
			mDescriptorSets[0] = mDescriptorPool->allocateDescriptorSet(mDescriptorSetLayouts[0].get());
			mDescriptorSets[1] = mDescriptorPool->allocateDescriptorSet(mDescriptorSetLayouts[1].get());
			if (mDescriptorSets[0] == nullptr || !mDescriptorSets[0]->isInitialized())
			{
				return false;
			}
			if (mDescriptorSets[1] == nullptr || !mDescriptorSets[1]->isInitialized())
			{
				return false;
			}

			// Update descriptor
			// TODO : Update more than one at once
			mUniformBuffer->updateDescriptor(mDescriptorSets[0].get(), 0, 0);

			nu::Vulkan::TexelBufferDescriptorInfo storageTexelBufferDescriptorUpdate = {
				mDescriptorSets[1]->getHandle(),            // VkDescriptorSet                      TargetDescriptorSet
				0,                                          // uint32_t                             TargetDescriptorBinding
				0,                                          // uint32_t                             TargetArrayElement
				VK_DESCRIPTOR_TYPE_STORAGE_TEXEL_BUFFER,    // VkDescriptorType                     TargetDescriptorType
				{                                           // std::vector<VkDescriptorBufferInfo>  BufferInfos
					{
						mParticlesVertexBufferView->getHandle()
					}
				}
			};

			mLogicalDevice->updateDescriptorSets({}, {}, { storageTexelBufferDescriptorUpdate }, {});

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

			// Compute pipeline

			VkPushConstantRange pushConstantRange = {
				VK_SHADER_STAGE_COMPUTE_BIT,    // VkShaderStageFlags     stageFlags
				0,                              // uint32_t               offset
				sizeof(float)                   // uint32_t               size
			};
			mComputePipelineLayout = mLogicalDevice->createPipelineLayout({ mDescriptorSetLayouts[1]->getHandle() }, { pushConstantRange });
			if (mComputePipelineLayout == nullptr || !mComputePipelineLayout->isInitialized())
			{
				return false;
			}

			nu::Vulkan::ShaderModule::Ptr computeShaderModule = mLogicalDevice->initShaderModule();
			if (computeShaderModule == nullptr || !computeShaderModule->loadFromFile("../Examples/8 - Drawing Particles Using Compute And Graphics Pipelines/shader.comp.spv"))
			{
				return false;
			}
			computeShaderModule->setComputeEntrypointName("main");

			if (!computeShaderModule->create())
			{
				return false;
			}

			mComputePipeline = mLogicalDevice->createComputePipeline(computeShaderModule.get(), mComputePipelineLayout.get(), nullptr, VK_NULL_HANDLE);
			if (mComputePipeline == nullptr || !mComputePipeline->isInitialized())
			{
				return false;
			}

			mComputeSemaphore = mLogicalDevice->createSemaphore();
			if (mComputeSemaphore == nullptr || !mComputeSemaphore->isInitialized())
			{
				return false;
			}

			mComputeFence = mLogicalDevice->createFence(true);
			if (mComputeFence == nullptr || !mComputeFence->isInitialized())
			{
				return false;
			}

			// Graphics pipeline

			mGraphicsPipelineLayout = mLogicalDevice->createPipelineLayout({ mDescriptorSetLayouts[0]->getHandle() }, {});
			if (mGraphicsPipelineLayout == nullptr || !mGraphicsPipelineLayout->isInitialized())
			{
				return false;
			}

			mGraphicsPipelines.resize(GraphicsPipelineNames::Count);
			mGraphicsPipelines[GraphicsPipelineNames::ParticlesPipeline] = mLogicalDevice->initGraphicsPipeline(*mGraphicsPipelineLayout, *mRenderPass, nullptr);

			nu::Vulkan::GraphicsPipeline* particlesPipeline = mGraphicsPipelines[GraphicsPipelineNames::ParticlesPipeline].get();

			particlesPipeline->setSubpass(0);

			nu::Vulkan::ShaderModule::Ptr particlesVertexShaderModule = mLogicalDevice->initShaderModule();
			if (particlesVertexShaderModule == nullptr || !particlesVertexShaderModule->loadFromFile("../Examples/8 - Drawing Particles Using Compute And Graphics Pipelines/shader.vert.spv"))
			{
				return false;
			}
			particlesVertexShaderModule->setVertexEntrypointName("main");

			nu::Vulkan::ShaderModule::Ptr particlesGeometryShaderModule = mLogicalDevice->initShaderModule();
			if (particlesGeometryShaderModule == nullptr || !particlesGeometryShaderModule->loadFromFile("../Examples/8 - Drawing Particles Using Compute And Graphics Pipelines/shader.geom.spv"))
			{
				return false;
			}
			particlesGeometryShaderModule->setGeometryEntrypointName("main");

			nu::Vulkan::ShaderModule::Ptr particlesFragmentShaderModule = mLogicalDevice->initShaderModule();
			if (particlesFragmentShaderModule == nullptr || !particlesFragmentShaderModule->loadFromFile("../Examples/8 - Drawing Particles Using Compute And Graphics Pipelines/shader.frag.spv"))
			{
				return false;
			}
			particlesFragmentShaderModule->setFragmentEntrypointName("main");

			if (!particlesVertexShaderModule->create() || !particlesGeometryShaderModule->create() || !particlesFragmentShaderModule->create())
			{
				return false;
			}

			particlesPipeline->addShaderModule(particlesVertexShaderModule.get());
			particlesPipeline->addShaderModule(particlesGeometryShaderModule.get());
			particlesPipeline->addShaderModule(particlesFragmentShaderModule.get());

			particlesPipeline->addVertexBinding(0, 8 * sizeof(float), VK_VERTEX_INPUT_RATE_VERTEX);
			particlesPipeline->addVertexAttribute(0, 0, VK_FORMAT_R32G32B32A32_SFLOAT, 0);
			particlesPipeline->addVertexAttribute(1, 0, VK_FORMAT_R32G32B32A32_SFLOAT, 4 * sizeof(float));

			particlesPipeline->setInputAssemblyState(VK_PRIMITIVE_TOPOLOGY_POINT_LIST, false);

			particlesPipeline->setViewport(0.0f, 0.0f, 500.0f, 500.0f, 0.0f, 1.0f);
			particlesPipeline->setScissor(0, 0, 500, 500);

			particlesPipeline->setRasterizationState(false, false, VK_POLYGON_MODE_FILL, VK_CULL_MODE_BACK_BIT, VK_FRONT_FACE_COUNTER_CLOCKWISE, false, 0.0f, 0.0f, 0.0f, 1.0f);

			particlesPipeline->addBlend(
				true,                                 // VkBool32                 blendEnable
				VK_BLEND_FACTOR_SRC_ALPHA,            // VkBlendFactor            srcColorBlendFactor
				VK_BLEND_FACTOR_ONE,                  // VkBlendFactor            dstColorBlendFactor
				VK_BLEND_OP_ADD,                      // VkBlendOp                colorBlendOp
				VK_BLEND_FACTOR_ONE,                  // VkBlendFactor            srcAlphaBlendFactor
				VK_BLEND_FACTOR_ONE,                  // VkBlendFactor            dstAlphaBlendFactor
				VK_BLEND_OP_ADD,                      // VkBlendOp                alphaBlendOp
				VK_COLOR_COMPONENT_R_BIT |            // VkColorComponentFlags    colorWriteMask
				VK_COLOR_COMPONENT_G_BIT |
				VK_COLOR_COMPONENT_B_BIT |
				VK_COLOR_COMPONENT_A_BIT);
			particlesPipeline->setBlendState(false, VK_LOGIC_OP_COPY, 1.0f, 1.0f, 1.0f, 1.0f);

			particlesPipeline->addDynamicState(VK_DYNAMIC_STATE_VIEWPORT);
			particlesPipeline->addDynamicState(VK_DYNAMIC_STATE_SCISSOR);

			if (!particlesPipeline->create())
			{
				return false;
			}

			return true;
		}

		virtual bool draw() override 
		{
			WaitSemaphoreInfo waitSemaphoreInfo = {
				mComputeSemaphore->getHandle(),
				VK_PIPELINE_STAGE_VERTEX_INPUT_BIT
			};
			if (!mComputeFence->wait(2000000000))
			{
				return false;
			}
			if (!mComputeFence->reset())
			{
				return false;
			}

			if (!mComputeCommandBuffer->beginRecording(VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT, nullptr))
			{
				return false;
			}
			mComputeCommandBuffer->bindDescriptorSets(VK_PIPELINE_BIND_POINT_COMPUTE, mComputePipelineLayout->getHandle(), 0, { mDescriptorSets[1].get() }, {});
			mComputeCommandBuffer->bindPipeline(mComputePipeline.get());
			float time = mTimerState.getDeltaTime();
			mComputeCommandBuffer->provideDataToShadersThroughPushConstants(mComputePipelineLayout->getHandle(), VK_SHADER_STAGE_COMPUTE_BIT, 0, sizeof(float), &time);
			mComputeCommandBuffer->dispatchComputeWork(PARTICLES_COUNT / 32 + 1, 1, 1);
			if (!mComputeCommandBuffer->endRecording())
			{
				return false;
			}
			if (!mComputeQueue->submitCommandBuffers({ mComputeCommandBuffer.get() }, {}, { mComputeSemaphore->getHandle() }, mComputeFence.get()))
			{
				return false;
			}

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

				mParticlesVertexBuffer->bindTo(commandBuffer, 0, 0);
				commandBuffer->bindDescriptorSets(VK_PIPELINE_BIND_POINT_GRAPHICS, mGraphicsPipelineLayout->getHandle(), 0, { mDescriptorSets[0].get() }, {});
				commandBuffer->bindPipeline(mGraphicsPipelines[GraphicsPipelineNames::ParticlesPipeline].get());
				commandBuffer->drawGeometry(PARTICLES_COUNT, 1, 0, 0);

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

			std::vector<WaitSemaphoreInfo> waitSemaphoreInfos = { waitSemaphoreInfo }; // TODO : This is modified for Compute Pipeline Stage
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

				if (!mStagingBuffer->mapUpdateAndUnmapHostVisibleMemory(0, sizeof(float) * 16, &modelViewMatrix[0], true, nullptr))
				{
					return false;
				}

				nu::Matrix4f perspectiveMatrix = nu::Matrix4f::perspective(50.0f, static_cast<float>(mSwapchain->getSize().width) / static_cast<float>(mSwapchain->getSize().height), 0.5f, 10.0f);

				if (!mStagingBuffer->mapUpdateAndUnmapHostVisibleMemory(sizeof(float) * 16, sizeof(float) * 16, &perspectiveMatrix[0], true, nullptr))
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

#endif // DRAWING_PARTICLES_USING_COMPTE_AND_GRAPHICS_PIPELINES_HPP
