#ifndef VERTEX_DIFFUSE_LIGHTNING_HPP
#define VERTEX_DIFFUSE_LIGHTNING_HPP

#include "../../CookBook/SampleBase.hpp"

#include "../../Math/Matrix4.hpp"

#include "../../Mesh.hpp"
#include "../../Window.hpp"

class VertexDiffuseLightning : public SampleBase
{
	public:
		nu::Mesh mMesh;
		nu::Vulkan::Buffer::Ptr mVertexBuffer;
		nu::Vulkan::MemoryBlock::Ptr mVertexBufferMemory;

		nu::Vulkan::DescriptorSetLayout::Ptr mDescriptorSetLayout;
		nu::Vulkan::DescriptorPool::Ptr mDescriptorPool;
		std::vector<nu::Vulkan::DescriptorSet::Ptr> mDescriptorSets;

		nu::Vulkan::RenderPass::Ptr mRenderPass;
		nu::Vulkan::PipelineLayout::Ptr mPipelineLayout;
		std::vector<nu::Vulkan::GraphicsPipeline::Ptr> mPipelines;
		enum PipelineNames
		{
			MeshPipeline = 0,
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

			// Vertex data
			if (!mMesh.loadFromFile("../Data/Models/knot.obj", true, false, false, true))
			{
				return false;
			}
			mVertexBuffer = mLogicalDevice->createBuffer(mMesh.size(), VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT);
			if (mVertexBuffer == nullptr || !mVertexBuffer->isInitialized())
			{
				return false;
			}
			mVertexBufferMemory = mVertexBuffer->allocateAndBindMemoryBlock(VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);
			if (mVertexBufferMemory == nullptr || !mVertexBufferMemory->isInitialized())
			{
				return false;
			}

			if (!mGraphicsQueue->useStagingBufferToUpdateBufferAndWait(mMesh.size(), &mMesh.data[0], mVertexBuffer.get(),
				0, 0, VK_ACCESS_VERTEX_ATTRIBUTE_READ_BIT, VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT, VK_PIPELINE_STAGE_VERTEX_INPUT_BIT,
				mFramesResources.front().mCommandBuffer.get(), {}, 50000000)) 
			{
				return false;
			}

			// Staging buffer
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

			// Uniform buffer
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

			// Descriptor set with uniform buffer
			VkDescriptorSetLayoutBinding descriptorSetLayoutBinding = {
				0,                                          // uint32_t             binding
				VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,          // VkDescriptorType     descriptorType
				1,                                          // uint32_t             descriptorCount
				VK_SHADER_STAGE_VERTEX_BIT,                 // VkShaderStageFlags   stageFlags
				nullptr                                     // const VkSampler    * pImmutableSamplers
			};
			mDescriptorSetLayout = mLogicalDevice->createDescriptorSetLayout({ descriptorSetLayoutBinding });
			if (mDescriptorSetLayout == nullptr || !mDescriptorSetLayout->isInitialized())
			{
				return false;
			}

			VkDescriptorPoolSize descriptorPoolSize = {
				VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,          // VkDescriptorType     type
				1                                           // uint32_t             descriptorCount
			};
			mDescriptorPool = mLogicalDevice->createDescriptorPool(false, 1, { descriptorPoolSize });
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

			mLogicalDevice->updateDescriptorSets({}, { bufferDescriptorUpdate }, {}, {});

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

			mPipelineLayout = mLogicalDevice->createPipelineLayout({ mDescriptorSetLayout->getHandle() }, {});
			if (mPipelineLayout == nullptr || !mPipelineLayout->isInitialized())
			{
				return false;
			}

			nu::Vulkan::ShaderModule::Ptr vertexShaderModule = mLogicalDevice->initShaderModule();
			if (vertexShaderModule == nullptr || !vertexShaderModule->loadFromFile("../Examples/1 - Vertex Diffuse Lightning/shader.vert.spv"))
			{
				return false;
			}
			vertexShaderModule->setVertexEntrypointName("main");

			nu::Vulkan::ShaderModule::Ptr fragmentShaderModule = mLogicalDevice->initShaderModule();
			if (fragmentShaderModule == nullptr || !fragmentShaderModule->loadFromFile("../Examples/1 - Vertex Diffuse Lightning/shader.frag.spv"))
			{
				return false;
			}
			fragmentShaderModule->setFragmentEntrypointName("main");

			if (!vertexShaderModule->create() || !fragmentShaderModule->create())
			{
				return false;
			}

			mPipelines.resize(PipelineNames::Count);
			mPipelines[PipelineNames::MeshPipeline] = mLogicalDevice->initGraphicsPipeline(*mPipelineLayout, *mRenderPass, nullptr);

			nu::Vulkan::GraphicsPipeline* modelPipeline = mPipelines[PipelineNames::MeshPipeline].get();

			modelPipeline->setSubpass(0);

			modelPipeline->addShaderModule(vertexShaderModule.get());
			modelPipeline->addShaderModule(fragmentShaderModule.get());

			modelPipeline->addVertexBinding(0, 6 * sizeof(float), VK_VERTEX_INPUT_RATE_VERTEX);
			modelPipeline->addVertexAttribute(0, 0, VK_FORMAT_R32G32B32_SFLOAT, 0);
			modelPipeline->addVertexAttribute(1, 0, VK_FORMAT_R32G32B32_SFLOAT, 3 * sizeof(float));

			//modelPipeline->setInputAssemblyState(VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST, false);

			modelPipeline->setViewport(0.0f, 0.0f, 500.0f, 500.0f, 0.0f, 1.0f);
			modelPipeline->setScissor(0, 0, 500, 500);

			//modelPipeline->setRasterizationState(false, false, VK_POLYGON_MODE_FILL, VK_CULL_MODE_BACK_BIT, VK_FRONT_FACE_COUNTER_CLOCKWISE, false, 0.0f, 0.0f, 0.0f, 1.0f);

			//modelPipeline->setMultisampleState(VK_SAMPLE_COUNT_1_BIT, false, 0.0f, nullptr, false, false);

			//modelPipeline->setDepthStencilState(true, true, VK_COMPARE_OP_LESS_OR_EQUAL, false, false, {}, {}, 0.0f, 1.0f);

			//modelPipeline->addBlend(false);
			//modelPipeline->setBlendState(false, VK_LOGIC_OP_COPY, 1.0f, 1.0f, 1.0f, 1.0f);

			modelPipeline->addDynamicState(VK_DYNAMIC_STATE_VIEWPORT);
			modelPipeline->addDynamicState(VK_DYNAMIC_STATE_SCISSOR);

			if (!modelPipeline->create())
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

				commandBuffer->bindVertexBuffers(0, { { mVertexBuffer.get(), 0 } } );

				commandBuffer->bindDescriptorSets(VK_PIPELINE_BIND_POINT_GRAPHICS, mPipelineLayout->getHandle(), 0, { mDescriptorSets[0].get() }, {});

				commandBuffer->bindPipeline(mPipelines[PipelineNames::MeshPipeline].get());

				for (size_t i = 0; i < mMesh.parts.size(); i++) 
				{
					commandBuffer->drawGeometry(mMesh.parts[i].vertexCount, 1, mMesh.parts[i].vertexOffset, 0);
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

#endif // VERTEX_DIFFUSE_LIGHTNING_HPP