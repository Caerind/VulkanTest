#ifndef NU_VULKAN_LOADER_HPP
#define NU_VULKAN_LOADER_HPP

#include "System/DynamicLibrary.hpp"
#include "VulkanFunctions.hpp"
#include "VulkanSmartObjects.hpp"
#include "WindowParameters.hpp"

#include "System/String.hpp"
#include "Math/Vector3.hpp"

#include <cstdio>
#include <vector>
#include <array>
#include <fstream>
#include <thread>

namespace nu
{

class VulkanLoader
{
	public:
		VulkanLoader() = delete;
		~VulkanLoader() = delete;

		static bool load();
		static bool isLoaded();
		static void unload();

	public:
		struct QueueInfo
		{
			uint32_t familyIndex;
			std::vector<float> priorities;
		};

		struct PresentInfo 
		{
			VkSwapchainKHR  swapchain;
			uint32_t imageIndex;
		};

		struct WaitSemaphoreInfo
		{
			VkSemaphore semaphore;
			VkPipelineStageFlags waitingStage;
		};

		struct BufferTransition 
		{
			VkBuffer buffer;
			VkAccessFlags currentAccess;
			VkAccessFlags newAccess;
			uint32_t currentQueueFamily;
			uint32_t newQueueFamily;
		};

		struct ImageTransition
		{
			VkImage image;
			VkAccessFlags currentAccess;
			VkAccessFlags newAccess;
			VkImageLayout currentLayout;
			VkImageLayout newLayout;
			uint32_t currentQueueFamily;
			uint32_t newQueueFamily;
			VkImageAspectFlags aspect;
		};

		struct ImageDescriptorInfo 
		{
			VkDescriptorSet targetDescriptorSet;
			uint32_t targetDescriptorBinding;
			uint32_t targetArrayElement;
			VkDescriptorType targetDescriptorType;
			std::vector<VkDescriptorImageInfo> imageInfos;
		};

		struct BufferDescriptorInfo 
		{
			VkDescriptorSet targetDescriptorSet;
			uint32_t targetDescriptorBinding;
			uint32_t targetArrayElement;
			VkDescriptorType targetDescriptorType;
			std::vector<VkDescriptorBufferInfo> bufferInfos;
		};

		struct TexelBufferDescriptorInfo 
		{
			VkDescriptorSet targetDescriptorSet;
			uint32_t targetDescriptorBinding;
			uint32_t targetArrayElement;
			VkDescriptorType targetDescriptorType;
			std::vector<VkBufferView> texelBufferViews;
		};

		struct CopyDescriptorInfo 
		{
			VkDescriptorSet targetDescriptorSet;
			uint32_t targetDescriptorBinding;
			uint32_t targetArrayElement;
			VkDescriptorSet sourceDescriptorSet;
			uint32_t sourceDescriptorBinding;
			uint32_t sourceArrayElement;
			uint32_t descriptorCount;
		};

		struct SubpassParameters 
		{
			VkPipelineBindPoint pipelineType;
			std::vector<VkAttachmentReference> inputAttachments;
			std::vector<VkAttachmentReference> colorAttachments;
			std::vector<VkAttachmentReference> resolveAttachments;
			const VkAttachmentReference* depthStencilAttachment;
			std::vector<uint32_t> preserveAttachments;
		};

		struct ShaderStageParameters
		{
			VkShaderStageFlagBits shaderStage;
			VkShaderModule shaderModule;
			const char* entryPointName;
			const VkSpecializationInfo* specializationInfo;
		};

		struct ViewportInfo
		{
			std::vector<VkViewport> viewports;
			std::vector<VkRect2D> scissors;
		};

		struct VertexBufferParameters
		{
			VkBuffer buffer;
			VkDeviceSize memoryOffset;
		};

		struct CommandBufferRecordingThreadParameters 
		{
			VkCommandBuffer commandBuffer;
			std::function<bool(VkCommandBuffer)> recordingFunction;
		};

		struct Mesh
		{
			std::vector<float> data;
			struct Part 
			{
				uint32_t vertexOffset;
				uint32_t vertexCount;
			};
			std::vector<Part> parts;
		};

		struct FrameResources 
		{
			VkCommandBuffer commandBuffer;
			VkSmartSemaphore imageAcquiredSemaphore;
			VkSmartSemaphore readyToPresentSemaphore;
			VkSmartFence drawingFinishedFence;
			VkSmartImageView depthAttachment;
			VkSmartFramebuffer framebuffer;

			FrameResources(VkCommandBuffer& pCommandBuffer, VkSmartSemaphore& pImageAcquiredSemaphore, VkSmartSemaphore& pReadyToPresentSemaphore, VkSmartFence& pDrawingFinishedFence, VkSmartImageView& pDepthAttachment, VkSmartFramebuffer& pFramebuffer)
				: commandBuffer(pCommandBuffer)
				, imageAcquiredSemaphore(std::move(pImageAcquiredSemaphore))
				, readyToPresentSemaphore(std::move(pReadyToPresentSemaphore))
				, drawingFinishedFence(std::move(pDrawingFinishedFence))
				, depthAttachment(std::move(pDepthAttachment))
				, framebuffer(std::move(pFramebuffer))
			{
			}

			FrameResources(FrameResources&& other) 
			{
				*this = std::move(other);
			}

			FrameResources& operator=(FrameResources&& other) 
			{
				if (this != &other) 
				{
					VkCommandBuffer commandBuffer = this->commandBuffer;

					this->commandBuffer = other.commandBuffer;
					other.commandBuffer = commandBuffer;
					imageAcquiredSemaphore = std::move(other.imageAcquiredSemaphore);
					readyToPresentSemaphore = std::move(other.readyToPresentSemaphore);
					drawingFinishedFence = std::move(other.drawingFinishedFence);
					depthAttachment = std::move(other.depthAttachment);
					framebuffer = std::move(other.framebuffer);
				}
				return *this;
			}

			FrameResources(const FrameResources&) = delete;
			FrameResources& operator=(const FrameResources&) = delete;
		};

		// Instances & Devices
		static bool loadDynamicLibrary(DynamicLibrary& dynamicLibrary);
		static bool loadFunctionExportedFromDynamicLibrary(const DynamicLibrary& dynamicLibrary);
		static bool loadGlobalLevelFunctions();
		static bool queryAvailableInstanceExtensions(std::vector<VkExtensionProperties>& availableExtensions);
		static bool createInstance(const std::vector<const char*>& desiredExtensions, const std::string& applicationName, VkInstance& instance);
		static bool loadInstanceLevelFunctions(VkInstance instance, const std::vector<const char*>& enabledExtensions);
		static bool queryAvailablePhysicalDevices(VkInstance instance, std::vector<VkPhysicalDevice>& availablePhysicalDevices);
		static bool queryAvailableDeviceExtensions(VkPhysicalDevice physicalDevice, std::vector<VkExtensionProperties>& availableExtensions);
		static void queryFeaturesAndPropertiesOfPhysicalDevice(VkPhysicalDevice physicalDevice, VkPhysicalDeviceFeatures& deviceFeatures, VkPhysicalDeviceProperties& deviceProperties);
		static bool queryAvailableQueueFamiliesAndTheirProperties(VkPhysicalDevice physicalDevice, std::vector<VkQueueFamilyProperties>& queueFamilies);
		static bool selectQueueFamilyWithDesiredCapabilities(VkPhysicalDevice physicalDevice, const std::vector<VkQueueFamilyProperties>& queueFamilies, VkQueueFlags desiredCapabilities, uint32_t& queueFamilyIndex);
		static bool selectQueueFamilyThatSupportsPresentationToSurface(VkPhysicalDevice physicalDevice, const std::vector<VkQueueFamilyProperties>& queueFamilies, VkSurfaceKHR presentationSurface, uint32_t& queueFamilyIndex);
		static bool createLogicalDevice(VkPhysicalDevice physicalDevice, std::vector<QueueInfo> queueInfos, const std::vector<const char*>& desiredExtensions, VkPhysicalDeviceFeatures* desiredFeatures, VkDevice& logicalDevice);
		static bool loadDeviceLevelFunctions(VkDevice logicalDevice, const std::vector<const char*>& enabledExtensions);
		static void getDeviceQueue(VkDevice logicalDevice, uint32_t queueFamilyIndex, uint32_t queueIndex, VkQueue& queue);
		static bool createLogicalDeviceWithGeometryShadersAndGraphicsAndComputeQueues(VkInstance instance, VkDevice& logicalDevice, VkQueue& graphicsQueue, VkQueue& computeQueue);
		static void destroyLogicalDevice(VkDevice& logicalDevice);
		static void destroyInstance(VkInstance& instance);
		static void releaseDynamicLibrary(DynamicLibrary& dynamicLibrary);

		// Image Presentation
		static bool createInstanceWithWsiExtensionsEnabled(std::vector<const char*>& desiredExtensions, const std::string& applicationName, VkInstance& instance);
		static bool createPresentationSurface(VkInstance instance, const WindowParameters& windowParameters, VkSurfaceKHR& presentationSurface);
		static bool selectQueueFamilyThatSupportsPresentationToGivenSurface(VkPhysicalDevice physicalDevice, VkSurfaceKHR presentationSurface, uint32_t& queueFamilyIndex);
		static bool createLogicalDeviceWithWsiExtensionsEnabled(VkPhysicalDevice physicalDevice, std::vector<QueueInfo> queueInfos, std::vector<const char*>& desiredExtensions, VkPhysicalDeviceFeatures* desiredFeatures, VkDevice& logicalDevice);
		static bool selectDesiredPresentationMode(VkPhysicalDevice physicalDevice, VkSurfaceKHR presentationSurface, VkPresentModeKHR desiredPresentMode, VkPresentModeKHR& presentMode);
		static bool queryCapabilitiesOfPresentationSurface(VkPhysicalDevice physicalDevice, VkSurfaceKHR presentationSurface, VkSurfaceCapabilitiesKHR& surfaceCapabilities);
		static bool selectNumberOfSwapchainImages(const VkSurfaceCapabilitiesKHR& surfaceCapabilities, uint32_t& numberOfImages);
		static bool chooseSizeOfSwapchainImages(const VkSurfaceCapabilitiesKHR& surfaceCapabilities, VkExtent2D& sizeOfImages);
		static bool selectDesiredUsageScenariosOfSwapchainImages(const VkSurfaceCapabilitiesKHR& surfaceCapabilities, VkImageUsageFlags desiredUsages, VkImageUsageFlags& imageUsage);
		static bool selectTransformationOfSwapchainImages(const VkSurfaceCapabilitiesKHR& surfaceCapabilities, VkSurfaceTransformFlagBitsKHR desiredTransform, VkSurfaceTransformFlagBitsKHR& surfaceTransform);
		static bool selectFormatOfSwapchainImages(VkPhysicalDevice physicalDevice, VkSurfaceKHR presentationSurface, VkSurfaceFormatKHR desiredSurfaceFormat, VkFormat& imageFormat, VkColorSpaceKHR& imageColorSpace);
		static bool createSwapchain(VkDevice logicalDevice, VkSurfaceKHR presentationSurface, uint32_t imageCount, VkSurfaceFormatKHR surfaceFormat, VkExtent2D imageSize, VkImageUsageFlags imageUsage, VkSurfaceTransformFlagBitsKHR surfaceTransform, VkPresentModeKHR presentMode, VkSwapchainKHR& oldSwapchain, VkSwapchainKHR& swapchain);
		static bool queryHandlesOfSwapchainImages(VkDevice logicalDevice, VkSwapchainKHR swapchain, std::vector<VkImage>& swapchainImages);
		static bool createSwapchainWithR8G8B8A8FormatAndMailboxPresentMode(VkPhysicalDevice physicalDevice, VkSurfaceKHR presentationSurface, VkDevice logicalDevice, VkImageUsageFlags swapchainImageUsage, VkExtent2D& imageSize, VkFormat& imageFormat, VkSwapchainKHR& oldSwapchain, VkSwapchainKHR& swapchain, std::vector<VkImage>& swapchainImages);
		static bool acquireSwapchainImage(VkDevice logicalDevice, VkSwapchainKHR swapchain, VkSemaphore semaphore, VkFence fence, uint32_t& imageIndex);
		static bool presentImage(VkQueue queue, std::vector<VkSemaphore> renderingSemaphores, std::vector<PresentInfo> imagesToPresent);
		static void destroySwapchain(VkDevice logicalDevice, VkSwapchainKHR& swapchain);
		static void destroyPresentationSurface(VkInstance instance, VkSurfaceKHR& presentationSurface);

		// Command Buffers & Synchronization
		static bool createCommandPool(VkDevice logicalDevice, VkCommandPoolCreateFlags parameters, uint32_t queueFamily, VkCommandPool& commandPool);
		static bool allocateCommandBuffers(VkDevice logicalDevice, VkCommandPool commandPool, VkCommandBufferLevel level, uint32_t count, std::vector<VkCommandBuffer>& commandBuffers);
		static bool beginCommandBufferRecordingOperation(VkCommandBuffer commandBuffer, VkCommandBufferUsageFlags usage, VkCommandBufferInheritanceInfo* secondaryCommandBufferInfo);
		static bool endCommandBufferRecordingOperation(VkCommandBuffer commandBuffer);
		static bool resetCommandBuffer(VkCommandBuffer commandBuffer, bool releaseResources);
		static bool resetCommandPool(VkDevice logicalDevice, VkCommandPool commandPool, bool releaseResources);
		static bool createSemaphore(VkDevice logicalDevice, VkSemaphore& semaphore);
		static bool createFence(VkDevice logicalDevice, bool signaled, VkFence& fence);
		static bool waitForFences(VkDevice logicalDevice, const std::vector<VkFence>& fences, VkBool32 waitForAll, uint64_t timeout);
		static bool resetFences(VkDevice logicalDevice, const std::vector<VkFence>& fences);
		static bool submitCommandBuffersToQueue(VkQueue queue, std::vector<WaitSemaphoreInfo> waitSemaphoreInfos, std::vector<VkCommandBuffer> commandBuffers, std::vector<VkSemaphore> signalSemaphores, VkFence fence);
		static bool synchronizeTwoCommandBuffers(VkQueue firstQueue, std::vector<WaitSemaphoreInfo> firstWaitSemaphoreInfos, std::vector<VkCommandBuffer> firstCommandBuffers, std::vector<WaitSemaphoreInfo> synchronizingSemaphores, VkQueue secondQueue, std::vector<VkCommandBuffer> secondCommandBuffers, std::vector<VkSemaphore> secondSignalSemaphores, VkFence secondFence);
		static bool checkIfProcessingOfSubmittedCommandBufferHasFinished(VkDevice logicalDevice, VkQueue queue, std::vector<WaitSemaphoreInfo> waitSemaphoreInfos, std::vector<VkCommandBuffer> commandBuffers, std::vector<VkSemaphore> signalSemaphores, VkFence fence, uint64_t timeout, VkResult& waitStatus);
		static bool waitUntilAllCommandsSubmittedToQueueAreFinished(VkQueue queue);
		static bool waitForAllSubmittedCommandsToBeFinished(VkDevice logicalDevice);
		static void destroyFence(VkDevice logicalDevice, VkFence& fence);
		static void destroySemaphore(VkDevice logicalDevice, VkSemaphore& semaphore);
		static void freeCommandBuffers(VkDevice logicalDevice, VkCommandPool commandPool, std::vector<VkCommandBuffer>& commandBuffers);
		static void destroyCommandPool(VkDevice logicalDevice, VkCommandPool& commandPool);

		// Resources & Memory
		static bool createBuffer(VkDevice logicalDevice, VkDeviceSize size, VkBufferUsageFlags usage, VkBuffer& buffer);
		static bool allocateAndBindMemoryObjectToBuffer(VkPhysicalDevice physicalDevice, VkDevice logicalDevice, VkBuffer buffer, VkMemoryPropertyFlagBits memoryProperties, VkDeviceMemory& memoryObject);
		static void setBufferMemoryBarrier(VkCommandBuffer commandBuffer, VkPipelineStageFlags generatingStages, VkPipelineStageFlags consumingStages, std::vector<BufferTransition> bufferTransitions);
		static bool createBufferView(VkDevice logicalDevice, VkBuffer buffer, VkFormat format, VkDeviceSize memoryOffset, VkDeviceSize memoryRange, VkBufferView& bufferView);
		static bool createImage(VkDevice logicalDevice, VkImageType type, VkFormat format, VkExtent3D size, uint32_t numMipmaps, uint32_t numLayers, VkSampleCountFlagBits samples, VkImageUsageFlags usageScenarios, bool cubemap, VkImage& image);
		static bool allocateAndBindMemoryObjectToImage(VkPhysicalDevice physicalDevice, VkDevice logicalDevice, VkImage image, VkMemoryPropertyFlagBits memoryProperties, VkDeviceMemory& memoryObject);
		static void setImageMemoryBarrier(VkCommandBuffer commandBuffer, VkPipelineStageFlags generatingStages, VkPipelineStageFlags consumingStages, std::vector<ImageTransition> imageTransitions);
		static bool createImageView(VkDevice logicalDevice, VkImage image, VkImageViewType viewType, VkFormat format, VkImageAspectFlags aspect, VkImageView& imageView);
		static bool create2DImageAndView(VkPhysicalDevice physicalDevice, VkDevice logicalDevice, VkFormat format, VkExtent2D size, uint32_t numMipmaps, uint32_t numLayers, VkSampleCountFlagBits samples, VkImageUsageFlags usage, VkImageAspectFlags aspect, VkImage& image, VkDeviceMemory& memoryObject, VkImageView& imageView);
		static bool createLayered2DImageWithCubemapView(VkPhysicalDevice physicalDevice, VkDevice logicalDevice, uint32_t size, uint32_t numMipmaps, VkImageUsageFlags usage, VkImageAspectFlags aspect, VkImage& image, VkDeviceMemory& memoryObject, VkImageView& imageView);
		static bool mapUpdateAndUnmapHostVisibleMemory(VkDevice logicalDevice, VkDeviceMemory memoryObject, VkDeviceSize offset, VkDeviceSize dataSize, void* data, bool unmap, void** pointer);
		static void copyDataBetweenBuffers(VkCommandBuffer commandBuffer, VkBuffer sourceBuffer, VkBuffer destinationBuffer, std::vector<VkBufferCopy> regions);
		static void copyDataFromBufferToImage(VkCommandBuffer commandBuffer, VkBuffer sourceBuffer, VkImage destinationImage, VkImageLayout imageLayout, std::vector<VkBufferImageCopy> regions);
		static void copyDataFromImageToBuffer(VkCommandBuffer commandBuffer, VkImage sourceImage, VkImageLayout imageLayout, VkBuffer destinationBuffer, std::vector<VkBufferImageCopy> regions);
		static bool useStagingBufferToUpdateBufferWithDeviceLocalMemoryBound(VkPhysicalDevice physicalDevice, VkDevice logicalDevice, VkDeviceSize dataSize, void* data, VkBuffer destinationBuffer, VkDeviceSize destinationOffset, VkAccessFlags destinationBufferCurrentAccess, VkAccessFlags destinationBufferNewAccess, VkPipelineStageFlags destinationBufferGeneratingStages, VkPipelineStageFlags destinationBufferConsumingStages, VkQueue queue, VkCommandBuffer commandBuffer, std::vector<VkSemaphore> signalSemaphores);
		static bool useStagingBufferToUpdateImageWithDeviceLocalMemoryBound(VkPhysicalDevice physicalDevice, VkDevice logicalDevice, VkDeviceSize dataSize, void* data, VkImage destinationImage, VkImageSubresourceLayers destinationImageSubresource, VkOffset3D destinationImageOffset, VkExtent3D destinationImageSize, VkImageLayout destinationImageCurrentLayout, VkImageLayout destinationImageNewLayout, VkAccessFlags destinationImageCurrentAccess, VkAccessFlags destinationImageNewAccess, VkImageAspectFlags destinationImageAspect, VkPipelineStageFlags destinationImageGeneratingStages, VkPipelineStageFlags destinationImageConsumingStages, VkQueue queue, VkCommandBuffer commandBuffer, std::vector<VkSemaphore> signalSemaphores);
		static void destroyImageView(VkDevice logicalDevice, VkImageView& imageView);
		static void destroyImage(VkDevice logicalDevice, VkImage& image);
		static void destroyBufferView(VkDevice logicalDevice, VkBufferView& bufferView);
		static void freeMemoryObject(VkDevice logicalDevice, VkDeviceMemory& memoryObject);
		static void destroyBuffer(VkDevice logicalDevice, VkBuffer& buffer);

		// Descriptor Sets
		static bool createSampler(VkDevice logicalDevice, VkFilter magFilter, VkFilter minFilter, VkSamplerMipmapMode mipmapMode, VkSamplerAddressMode uAddressMode, VkSamplerAddressMode vAddressMode, VkSamplerAddressMode wAddressMode, float lodBias, bool anisotropyEnable, float maxAnisotropy, bool compareEnable, VkCompareOp compareOperator, float minLod, float maxLod, VkBorderColor borderColor, bool unnormalizedCoords, VkSampler& sampler);
		static bool createSampledImage(VkPhysicalDevice physicalDevice, VkDevice logicalDevice, VkImageType type, VkFormat format, VkExtent3D size, uint32_t numMipmaps, uint32_t numLayers, VkImageUsageFlags usage, bool cubemap, VkImageViewType viewType, VkImageAspectFlags aspect, bool linearFiltering, VkImage& sampledImage, VkDeviceMemory& memoryObject, VkImageView& sampledImageView);
		static bool createCombinedImageSampler(VkPhysicalDevice physicalDevice, VkDevice logicalDevice, VkImageType type, VkFormat format, VkExtent3D size, uint32_t numMipmaps, uint32_t numLayers, VkImageUsageFlags usage, bool cubemap, VkImageViewType viewType, VkImageAspectFlags aspect, VkFilter magFilter, VkFilter minFilter, VkSamplerMipmapMode mipmapMode, VkSamplerAddressMode uAddressMode, VkSamplerAddressMode vAddressMode, VkSamplerAddressMode wAddressMode, float lodBias, bool anisotropyEnable, float maxAnisotropy, bool compareEnable, VkCompareOp compareOperator, float minLod, float maxLod, VkBorderColor borderColor, bool unnormalizedCoords, VkSampler& sampler, VkImage& sampledImage, VkDeviceMemory& memoryObject, VkImageView& sampledImageView);
		static bool createStorageImage(VkPhysicalDevice physicalDevice, VkDevice logicalDevice, VkImageType type, VkFormat format, VkExtent3D size, uint32_t numMipmaps, uint32_t numLayers, VkImageUsageFlags usage, VkImageViewType viewType, VkImageAspectFlags aspect, bool atomicOperations, VkImage& storageImage, VkDeviceMemory& memoryObject, VkImageView& storageImageView);
		static bool createUniformTexelBuffer(VkPhysicalDevice physicalDevice, VkDevice logicalDevice, VkFormat format, VkDeviceSize size, VkImageUsageFlags usage, VkBuffer& uniformTexelBuffer, VkDeviceMemory& memoryObject, VkBufferView& uniformTexelBufferView);
		static bool createStorageTexelBuffer(VkPhysicalDevice physicalDevice, VkDevice logicalDevice, VkFormat format, VkDeviceSize size, VkBufferUsageFlags usage, bool atomicOperations, VkBuffer& storageTexelBuffer, VkDeviceMemory& memoryObject, VkBufferView& storageTexelBufferView);
		static bool createUniformBuffer(VkPhysicalDevice physicalDevice, VkDevice logicalDevice, VkDeviceSize size, VkBufferUsageFlags usage, VkBuffer& uniformBuffer, VkDeviceMemory& memoryObject);
		static bool createStorageBuffer(VkPhysicalDevice physicalDevice, VkDevice logicalDevice, VkDeviceSize size, VkBufferUsageFlags usage, VkBuffer& storageBuffer, VkDeviceMemory& memoryObject);
		static bool createInputAttachment(VkPhysicalDevice physicalDevice, VkDevice logicalDevice, VkImageType type, VkFormat format, VkExtent3D size, VkImageUsageFlags usage, VkImageViewType viewType, VkImageAspectFlags aspect, VkImage& inputAttachment, VkDeviceMemory& memoryObject, VkImageView& inputAttachmentImageView);
		static bool createDescriptorSetLayout(VkDevice logicalDevice, const std::vector<VkDescriptorSetLayoutBinding>& bindings, VkDescriptorSetLayout& descriptorSetLayout);
		static bool createDescriptorPool(VkDevice logicalDevice, bool freeIndividualSets, uint32_t maxSetsCount, const std::vector<VkDescriptorPoolSize>& descriptorTypes, VkDescriptorPool& descriptorPool);
		static bool allocateDescriptorSets(VkDevice logicalDevice, VkDescriptorPool descriptorPool, const std::vector<VkDescriptorSetLayout>& descriptorSetLayouts, std::vector<VkDescriptorSet>& descriptorSets);
		static void updateDescriptorSets(VkDevice logicalDevice, const std::vector<ImageDescriptorInfo>& imageDescriptorInfos, const std::vector<BufferDescriptorInfo>& bufferDescriptorInfos, const std::vector<TexelBufferDescriptorInfo>& texelBufferDescriptorInfos, const std::vector<CopyDescriptorInfo>& copyDescriptorInfos);
		static void bindDescriptorSets(VkCommandBuffer commandBuffer, VkPipelineBindPoint pipelineType, VkPipelineLayout pipelineLayout, uint32_t indexForFirstSet, const std::vector<VkDescriptorSet>& descriptorSets, const std::vector<uint32_t>& dynamicOffsets);
		static bool createDescriptorsWithTextureAndUniformBuffer(VkPhysicalDevice physicalDevice, VkDevice logicalDevice, VkExtent3D sampledImageSize, uint32_t uniformBufferSize, VkSampler& sampler, VkImage& sampledImage, VkDeviceMemory& sampledImageMemoryObject, VkImageView& sampledImageView, VkBuffer& uniformBuffer, VkDeviceMemory& uniformBufferMemoryObject, VkDescriptorSetLayout& descriptorSetLayout, VkDescriptorPool& descriptorPool, std::vector<VkDescriptorSet>& descriptorSets);
		static bool freeDescriptorSets(VkDevice logicalDevice, VkDescriptorPool descriptorPool, std::vector<VkDescriptorSet>& descriptorSets);
		static bool resetDescriptorPool(VkDevice logicalDevice, VkDescriptorPool descriptorPool);
		static void destroyDescriptorPool(VkDevice logicalDevice, VkDescriptorPool& descriptorPool);
		static void destroyDescriptorSetLayout(VkDevice logicalDevice, VkDescriptorSetLayout& descriptorSetLayout);
		static void destroySampler(VkDevice logicalDevice, VkSampler& sampler);

		// Render Passes & Framebuffers
		static void specifyAttachmentsDescriptions(const std::vector<VkAttachmentDescription>& attachmentsDescriptions);
		static void specifySubpassDescriptions(const std::vector<SubpassParameters>& subpassParameters, std::vector<VkSubpassDescription>& subpassDescriptions);
		static void specifyDependenciesBetweenSubpasses(const std::vector<VkSubpassDependency>& subpassesDependencies);
		static bool createRenderPass(VkDevice logicalDevice, const std::vector<VkAttachmentDescription>& attachmentsDescriptions, const std::vector<SubpassParameters>& subpassParameters, const std::vector<VkSubpassDependency>& subpassDependencies, VkRenderPass& renderPass);
		static bool createFramebuffer(VkDevice logicalDevice, VkRenderPass renderPass, const std::vector<VkImageView>& attachments, uint32_t width, uint32_t height, uint32_t layers, VkFramebuffer& framebuffer);
		static bool prepareRenderPassForGeometryRenderingAndPostprocessSubpasses(VkDevice logicalDevice, VkRenderPass renderPass);
		static bool prepareRenderPassAndFramebufferWithColorAndDepthAttachments(VkPhysicalDevice physicalDevice, VkDevice logicalDevice, uint32_t width, uint32_t height, VkImage& colorImage, VkDeviceMemory& colorImageMemoryObject, VkImageView& colorImageView, VkImage& depthImage, VkDeviceMemory& depthImageMemoryObject, VkImageView& depthImageView, VkRenderPass& renderPass, VkFramebuffer& framebuffer);
		static void beginRenderPass(VkCommandBuffer commandBuffer, VkRenderPass renderPass, VkFramebuffer framebuffer, VkRect2D renderArea, const std::vector<VkClearValue>& clearValues, VkSubpassContents subpassContents);
		static void progressToTheNextSubpass(VkCommandBuffer commandBuffer, VkSubpassContents subpassContents);
		static void endRenderPass(VkCommandBuffer commandBuffer);
		static void destroyFramebuffer(VkDevice logicalDevice, VkFramebuffer& framebuffer);
		static void destroyRenderPass(VkDevice logicalDevice, VkRenderPass& renderPass);

		// Graphics & Compute Pipelines
		static bool createShaderModule(VkDevice logicalDevice, const std::vector<unsigned char>& sourceCode, VkShaderModule& shaderModule);
		static void specifyPipelineShaderStages(const std::vector<ShaderStageParameters>& shaderStageParams, std::vector<VkPipelineShaderStageCreateInfo>& shaderStageCreateInfos);
		static void specifyPipelineVertexInputState(const std::vector<VkVertexInputBindingDescription>& bindingDescriptions, const std::vector<VkVertexInputAttributeDescription>& attributeDescriptions, VkPipelineVertexInputStateCreateInfo& vertexInputStateCreateInfo);
		static void specifyPipelineInputAssemblyState(VkPrimitiveTopology topology, bool primitiveRestartEnable, VkPipelineInputAssemblyStateCreateInfo& inputAssemblyStateCreateInfo);
		static void specifyPipelineTessellationState(uint32_t patchControlPointsCount, VkPipelineTessellationStateCreateInfo& tessellationStateCreateInfo);
		static void specifyPipelineViewportAndScissorTestState(const ViewportInfo& viewportInfos, VkPipelineViewportStateCreateInfo& viewportStateCreateInfo);
		static void specifyPipelineRasterizationState(bool depthClampEnable, bool rasterizerDiscardEnable, VkPolygonMode polygonMode, VkCullModeFlags cullingMode, VkFrontFace frontFace, bool depthBiasEnable, float depthBiasConstantFactor, float depthBiasClamp, float depthBiasSlopeFactor, float lineWidth, VkPipelineRasterizationStateCreateInfo& rasterizationStateCreateInfo);
		static void specifyPipelineMultisampleState(VkSampleCountFlagBits sampleCount, bool perSampleShadingEnable, float minSampleShading, const VkSampleMask* sampleMasks, bool alphaToCoverageEnable, bool alphaToOneEnable, VkPipelineMultisampleStateCreateInfo& multisampleStateCreateInfo);
		static void specifyPipelineDepthAndStencilState(bool depthTestEnable, bool depthWriteEnable, VkCompareOp depthCompareOp, bool depthBoundsTestEnable, float minDepthBounds, float maxDepthBounds, bool stencilTestEnable, VkStencilOpState frontStencilTestParameters, VkStencilOpState backStencilTestParameters, VkPipelineDepthStencilStateCreateInfo& depthAndStencilStateCreateInfo);
		static void specifyPipelineBlendState(bool logicOpEnable, VkLogicOp logicOp, const std::vector<VkPipelineColorBlendAttachmentState>& attachmentBlendStates, const std::array<float, 4>& blendConstants, VkPipelineColorBlendStateCreateInfo& blendStateCreateInfo);
		static void specifyPipelineDynamicStates(const std::vector<VkDynamicState>& dynamicStates, VkPipelineDynamicStateCreateInfo& dynamicStateCreateInfo);
		static bool createPipelineLayout(VkDevice logicalDevice, const std::vector<VkDescriptorSetLayout>& descriptorSetLayouts, const std::vector<VkPushConstantRange>& pushConstantRanges, VkPipelineLayout& pipelineLayout);
		static void specifyGraphicsPipelineCreationParameters(VkPipelineCreateFlags additionalOptions, const std::vector<VkPipelineShaderStageCreateInfo>& shaderStageCreateInfos, const VkPipelineVertexInputStateCreateInfo& vertexInputState_createInfo, const VkPipelineInputAssemblyStateCreateInfo& inputAssemblyStateCreateInfo, const VkPipelineTessellationStateCreateInfo* tessellationStateCreateInfo, const VkPipelineViewportStateCreateInfo* viewportStateCreateInfo, const VkPipelineRasterizationStateCreateInfo& rasterizationStateCreateInfo, const VkPipelineMultisampleStateCreateInfo* multisampleStateCreateInfo, const VkPipelineDepthStencilStateCreateInfo* depthAndStencilStateCreateInfo, const VkPipelineColorBlendStateCreateInfo* blendStateCreateInfo, const VkPipelineDynamicStateCreateInfo* dynamicStateCreateInfo, VkPipelineLayout pipelineLayout, VkRenderPass renderPass, uint32_t subpass, VkPipeline basePipelineHandle, int32_t basePipelineIndex, VkGraphicsPipelineCreateInfo& graphicsPipelineCreateInfo);
		static bool createPipelineCacheObject(VkDevice logicalDevice, const std::vector<unsigned char>& cacheData, VkPipelineCache& pipelineCache);
		static bool retrieveDataFromPipelineCache(VkDevice logicalDevice, VkPipelineCache pipelineCache, std::vector<unsigned char>& pipelineCacheData);
		static bool mergeMultiplePipelineCacheObjects(VkDevice logicalDevice, VkPipelineCache targetPipelineCache, const std::vector<VkPipelineCache>& sourcePipelineCaches);
		static bool createGraphicsPipelines(VkDevice logicalDevice, const std::vector<VkGraphicsPipelineCreateInfo>& graphicsPipelineCreateInfos, VkPipelineCache pipelineCache, std::vector<VkPipeline>& graphicsPipelines);
		static bool createComputePipeline(VkDevice logicalDevice, VkPipelineCreateFlags additionalOptions, const VkPipelineShaderStageCreateInfo& computeShaderStage, VkPipelineLayout pipelineLayout, VkPipeline basePipelineHandle, VkPipelineCache pipelineCache, VkPipeline& computePipeline);
		static void bindPipelineObject(VkCommandBuffer commandBuffer, VkPipelineBindPoint pipelineType, VkPipeline pipeline);
		static bool createPipelineLayoutWithCombinedImageSamplerBufferAndPushConstantRanges(VkDevice logicalDevice, const std::vector<VkPushConstantRange>& pushConstantRanges, VkDescriptorSetLayout& descriptorSetLayout, VkPipelineLayout& pipelineLayout);
		static bool createGraphicsPipelineWithVertexAndFragmentShadersDepthTestEnabledAndWithDynamicViewportAndScissorTests(VkDevice logicalDevice, VkPipelineCreateFlags additionalOptions, const std::string& vertexShaderFilename, const std::string& fragmentShaderFilename, const std::vector<VkVertexInputBindingDescription>& vertexInputBindingDescriptions, const std::vector<VkVertexInputAttributeDescription>& vertexAttributeDescriptions, VkPrimitiveTopology primitiveTopology, bool primitiveRestartEnable, VkPolygonMode polygonMode, VkCullModeFlags cullingMode, VkFrontFace frontFace, bool logicOpEnable, VkLogicOp logicOp, const std::vector<VkPipelineColorBlendAttachmentState>& attachmentBlendStates, const std::array<float, 4>& blendConstants, VkPipelineLayout pipelineLayout, VkRenderPass renderPass, uint32_t subpass, VkPipeline basePipelineHandle, VkPipelineCache pipelineCache, std::vector<VkPipeline>& graphicsPipeline);
		static bool createMultipleGraphicsPipelineOnMultipleThreads(VkDevice logicalDevice, const std::string& pipelineCacheFilename, const std::vector<std::vector<VkGraphicsPipelineCreateInfo>>& graphicsPipelinesCreateInfos, std::vector<std::vector<VkPipeline>>& graphicsPipelines);
		static void destroyPipeline(VkDevice logicalDevice, VkPipeline& pipeline);
		static void destroyPipelineCache(VkDevice logicalDevice, VkPipelineCache& pipelineCache);
		static void destroyPipelineLayout(VkDevice logicalDevice, VkPipelineLayout& pipelineLayout);
		static void destroyShaderModule(VkDevice logicalDevice, VkShaderModule& shaderModule);

		// Command Recording & Drawing
		static void clearColorImage(VkCommandBuffer commandBuffer, VkImage image, VkImageLayout imageLayout, const std::vector<VkImageSubresourceRange>& imageSubresourceRanges, VkClearColorValue& clearColor);
		static void clearDepthStencilImage(VkCommandBuffer commandBuffer, VkImage image, VkImageLayout imageLayout, const std::vector<VkImageSubresourceRange>& imageSubresourceRanges, VkClearDepthStencilValue& clearValue);
		static void clearRenderPassAttachments(VkCommandBuffer commandBuffer, const std::vector<VkClearAttachment>& attachments, const std::vector<VkClearRect>& rects);
		static void bindVertexBuffers(VkCommandBuffer commandBuffer, uint32_t firstBinding, const std::vector<VertexBufferParameters>& buffersParameters);
		static void bindIndexBuffer(VkCommandBuffer commandBuffer, VkBuffer buffer, VkDeviceSize memoryOffset, VkIndexType indexType);
		static void provideDataToShadersThroughPushConstants(VkCommandBuffer commandBuffer, VkPipelineLayout pipelineLayout, VkShaderStageFlags pipelineStages, uint32_t offset, uint32_t size, void* data);
		static void setViewportStateDynamically(VkCommandBuffer commandBuffer, uint32_t firstViewport, const std::vector<VkViewport>& viewports);
		static void setScissorStateDynamically(VkCommandBuffer commandBuffer, uint32_t firstScissor, const std::vector<VkRect2D>& scissors);
		static void setLineWidthStateDynamically(VkCommandBuffer commandBuffer, float lineWidth);
		static void setDepthBiasStateDynamically(VkCommandBuffer commandBuffer, float constantFactor, float clampValue, float slopeFactor);
		static void setBlendConstantsStateDynamically(VkCommandBuffer commandBuffer, const std::array<float, 4>& blendConstants);
		static void drawGeometry(VkCommandBuffer commandBuffer, uint32_t vertexCount, uint32_t instanceCount, uint32_t firstVertex, uint32_t firstInstance);
		static void drawIndexedGeometry(VkCommandBuffer commandBuffer, uint32_t indexCount, uint32_t instanceCount, uint32_t firstIndex, uint32_t vertexOffset, uint32_t firstInstance);
		static void dispatchComputeWork(VkCommandBuffer commandBuffer, uint32_t xSize, uint32_t ySize, uint32_t zSize);
		static void executeSecondaryCommandBufferInsidePrimaryCommandBuffer(VkCommandBuffer primaryCommandBuffer, const std::vector<VkCommandBuffer>& secondaryCommandBuffers);
		static bool recordCommandBufferThatDrawGeometryWithDynamicViewportAndScissorStates(VkCommandBuffer commandBuffer, VkImage swapchainImage, uint32_t presentQueueFamilyIndex, uint32_t graphicsQueueFamilyIndex, VkRenderPass renderPass, VkFramebuffer framebuffer, VkExtent2D framebufferSize, const std::vector<VkClearValue>& clearValues, VkPipeline graphicsPipeline, uint32_t firstVertexBufferBinding, const std::vector<VertexBufferParameters>& vertexBuffersParameters, VkPipelineLayout pipelineLayout, const std::vector<VkDescriptorSet>& descriptorSets, uint32_t indexForFirstDescriptorSet, const Mesh& geometry, uint32_t instanceCount, uint32_t firstInstance);
		static bool recordCommandBuffersOnMultipleThreads(const std::vector<CommandBufferRecordingThreadParameters>& threadsParameters, VkQueue queue, std::vector<WaitSemaphoreInfo> waitSemaphoreInfos, std::vector<VkSemaphore> signalSemaphores, VkFence fence);
		static bool prepareSingleFrameOfAnimation(VkDevice logicalDevice, VkQueue graphicsQueue, VkQueue presentQueue, VkSwapchainKHR swapchain, VkExtent2D swapchainSize, const std::vector<VkImageView>& swapchainImageViews, VkImageView depthAttachment, const std::vector<WaitSemaphoreInfo>& waitInfos, VkSemaphore imageAcquiredSemaphore, VkSemaphore readyToPresentSemaphore, VkFence finishedDrawingFence, std::function<bool(VkCommandBuffer, uint32_t, VkFramebuffer)> recordCommandBuffer, VkCommandBuffer commandBuffer, VkRenderPass renderPass, VkSmartFramebuffer& framebuffer);
		static bool increasePerformanceThroughIncreasingTheNumberOfSeparatelyRenderedFrames(VkDevice logicalDevice, VkQueue graphicsQueue, VkQueue presentQueue, VkSwapchainKHR swapchain, VkExtent2D swapchainSize, const std::vector<VkImageView>& swapchainImageViews, VkRenderPass renderPass, const std::vector<WaitSemaphoreInfo>& waitInfos, std::function<bool(VkCommandBuffer, uint32_t, VkFramebuffer)> recordCommandBuffer, std::vector<FrameResources>& frameResources);

		static bool isExtensionSupported(const std::vector<VkExtensionProperties>& availableExtensions, char const * const extension);
		static const char* toErrorString(VkResult result);
		static bool loadBinaryFile(const std::string& filename, std::vector<unsigned char>& contents);
		static bool loadTextureDataFromFile(const std::string& filename, int numRequestedComponents, std::vector<unsigned char>& imageData, int* imageWidth = nullptr, int* imageHeight = nullptr, int* imageNumComponents = nullptr, int* imageDataSize = nullptr);
		static bool loadModelFromObjFile(const std::string& filename, bool loadNormals, bool loadTexCoords, bool generateTangentSpaceVectors, bool unify, Mesh& mesh, uint32_t* vertexStride = nullptr);
		static void generateTangentSpaceVectors(Mesh& mesh);
		static void calculateTangentAndBitangent(const float* normalData, const Vector3<float>& faceTangent, const Vector3<float>& faceBitangent, float* tangentData, float* bitangentData);

		static void describePhysicalDevice(VkPhysicalDevice physicalDevice);
		static bool areFeaturesSupported(VkPhysicalDeviceFeatures* deviceFeatures, VkPhysicalDeviceFeatures* desiredFeatures);
		static bool createDevices(const std::vector<const char*>& desiredExtensions, VkPhysicalDeviceFeatures* desiredFeatures, VkPhysicalDevice& physicalDevice, VkDevice& deivce);
};

} // namespace nu

#endif // NU_VULKAN_LOADER_HPP