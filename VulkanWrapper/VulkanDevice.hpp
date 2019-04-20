#pragma once

#include <unordered_map>

#include "VulkanFunctions.hpp"

#include "../CookBook/Common.hpp" // TODO : Remove this

#include "VulkanQueueManager.hpp"

VULKAN_NAMESPACE_BEGIN

struct VulkanImageDescriptorInfo 
{
	VkDescriptorSet targetDescriptorSet; // TODO : Ptr ?
	VulkanU32 targetDescriptorBinding;
	VulkanU32 targetArrayElement;
	VkDescriptorType targetDescriptorType;
	std::vector<VkDescriptorImageInfo> imageInfos;
};

struct VulkanBufferDescriptorInfo 
{
	VkDescriptorSet targetDescriptorSet; // TODO : Ptr ?
	VulkanU32 targetDescriptorBinding;
	VulkanU32 targetArrayElement;
	VkDescriptorType targetDescriptorType;
	std::vector<VkDescriptorBufferInfo> bufferInfos;
};

struct VulkanTexelBufferDescriptorInfo 
{
	VkDescriptorSet targetDescriptorSet; // TODO : Ptr ?
	VulkanU32 targetDescriptorBinding;
	VulkanU32 targetArrayElement;
	VkDescriptorType targetDescriptorType;
	std::vector<VkBufferView> texelBufferViews; // TODO : Ptr ?
};

struct VulkanCopyDescriptorInfo 
{
	VkDescriptorSet targetDescriptorSet; // TODO : Ptr ?
	VulkanU32 targetDescriptorBinding;
	VulkanU32 targetArrayElement;
	VkDescriptorSet sourceDescriptorSet; // TODO : Ptr ?
	VulkanU32 sourceDescriptorBinding;
	VulkanU32 sourceArrayElement;
	VulkanU32 descriptorCount;
};

// TODO : Add DeviceObjectTracker ? Manage its objects ?
// TODO : Keep extensions ? features ? (as attributes)
// TODO : Add initialization parameters for layers/extensions/... ?

class VulkanDevice : VulkanObject<VulkanObjectType_Device>
{
	public:
		#if (defined VULKAN_MONO_DEVICE)
		static VulkanDevice& get();
		static bool initialize(VulkanPhysicalDevice& physicalDevice, VkPhysicalDeviceFeatures* desiredFeatures, const VulkanQueueParameters& queueParameters, VulkanSurface* surface = nullptr);
		static bool uninitialize();
		static bool initialized();
		#else
		static VulkanDevice& get(VulkanDeviceId id);
		static bool initialize(VulkanDeviceId id, VulkanPhysicalDevice& physicalDevice, VkPhysicalDeviceFeatures* desiredFeatures, const VulkanQueueParameters& queueParameters, VulkanSurface* surface = nullptr);
		static bool uninitialize(VulkanDeviceId id);
		static bool initialized(VulkanDeviceId id);
		#endif
	
	public:
		VulkanBufferPtr createBuffer(VkDeviceSize size, VkBufferUsageFlags usage);

		VulkanCommandPoolPtr createCommandPool(VkCommandPoolCreateFlags parameters, VulkanU32 queueFamily);

		VulkanDescriptorPoolPtr createDescriptorPool(bool freeIndividualSets, VulkanU32 maxSetsCount, const std::vector<VkDescriptorPoolSize>& descriptorTypes); 
		VulkanDescriptorSetLayoutPtr createDescriptorSetLayout(const std::vector<VkDescriptorSetLayoutBinding>& bindings);
		void updateDescriptorSets(const std::vector<VulkanImageDescriptorInfo>& imageDescriptorInfos, const std::vector<VulkanBufferDescriptorInfo>& bufferDescriptorInfos, const std::vector<VulkanTexelBufferDescriptorInfo>& texelBufferDescriptorInfos, const std::vector<VulkanCopyDescriptorInfo>& copyDescriptorInfos);

		VulkanFencePtr createFence(bool signaled = false);
		bool waitFences(const std::vector<VulkanFence*>& fences, bool waitForAll, VulkanU64 timeout); // TODO : Friendlier Time type
		bool resetFences(const std::vector<VulkanFence*>& fences);

		VulkanImagePtr createImage(VkImageType type, VkFormat format, VkExtent3D size, VulkanU32 numMipmaps, VulkanU32 numLayers, VkSampleCountFlagBits samples, VkImageUsageFlags usageScenarios, bool cubemap);
		
		VulkanMemoryBlockPtr createMemoryBlock(VkMemoryRequirements memoryRequirements, VkMemoryPropertyFlags memoryProperties);
   
   		// TODO : Endianness ?
		VulkanPipelineCachePtr createPipelineCache(const std::vector<VulkanU8>& cacheData = {});

		VulkanPipelineLayoutPtr createPipelineLayout(const std::vector<VkDescriptorSetLayout>& descriptorSetLayouts, const std::vector<VkPushConstantRange> pushConstantRanges);

		VulkanComputePipelinePtr createComputePipeline(VulkanShaderModule* computeShader, VulkanPipelineLayout* layout, VulkanPipelineCache* cache, VkPipelineCreateFlags additionalOptions);

		VulkanGraphicsPipelinePtr initGraphicsPipeline(VulkanPipelineLayout& layout, VulkanRenderPass& renderPass, VulkanPipelineCache* cache = nullptr);

		VulkanRenderPassPtr initRenderPass();

		VulkanSamplerPtr createSampler(VkFilter magFilter, VkFilter minFilter, VkSamplerMipmapMode mipmapMode, VkSamplerAddressMode uAddressMode, VkSamplerAddressMode vAddressMode, VkSamplerAddressMode wAddressMode, VulkanF32 lodBias, VulkanF32 minLod, VulkanF32 maxLod, bool anisotropyEnable, VulkanF32 maxAnisotropy, bool compareEnable, VkCompareOp compareOperator, VkBorderColor borderColor, bool unnormalizedCoords);

		VulkanSemaphorePtr createSemaphore();
		
		VulkanShaderModulePtr initShaderModule();

		VulkanSwapchainPtr createSwapchain(VulkanSurface& surface, std::vector<FrameResources>& framesResources);

		bool waitForAllSubmittedCommands();

		bool isInitialized() const;
		const VkDevice& getHandle() const;
		const VulkanPhysicalDevice& getPhysicalDevice() const;
		const VkPhysicalDevice& getPhysicalDeviceHandle() const;
		
		// VulkanPhysicalDevice 
		const std::vector<VkExtensionProperties>& getAvailableExtensions() const;
		const VkPhysicalDeviceFeatures& getSupportedFeatures() const;
		const VkPhysicalDeviceProperties& getProperties() const;
		const std::vector<VkQueueFamilyProperties>& getQueueFamiliesProperties() const;
		const VkPhysicalDeviceMemoryProperties& getMemoryProperties() const;
		const VkFormatProperties& getFormatProperties(VkFormat format) const;
		bool isExtensionSupported(const char* extensionName) const;

		// VulkanQueueManager
		VulkanQueue* getQueue(VulkanU32 index);
		const VulkanQueue* getQueue(VulkanU32 index) const;
		VulkanU32 getQueueCount() const;

	private:
		VulkanDevice(VulkanPhysicalDevice& physicalDevice);
		~VulkanDevice();
		
		// NonCopyable and NonMovable
		VulkanDevice(const VulkanDevice& other) = delete;
		VulkanDevice(VulkanDevice&& other) = delete;
		VulkanDevice& operator=(const VulkanDevice& other) = delete;
		VulkanDevice& operator=(VulkanDevice&& other) = delete;

		bool initializeInternal(VkPhysicalDeviceFeatures* desiredFeatures, const VulkanQueueParameters& queueParameters, VulkanSurface* surface = nullptr);
		bool uninitializeInternal();

		VkDevice mDevice;
		VulkanPhysicalDevice& mPhysicalDevice;
		VulkanQueueManager mQueueManager;
		
		#if (defined VULKAN_MONO_DEVICE)
		static VulkanDevice* sDevice;
		#else
	public:
		VulkanDeviceId getDeviceId() const { return mDeviceId; }
	private:	
		VulkanDeviceId mDeviceId;
		static std::unordered_map<VulkanDeviceId, VulkanDevice*> sDevices;
		#endif
};

VULKAN_NAMESPACE_END