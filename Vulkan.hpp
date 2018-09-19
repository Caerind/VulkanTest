#ifndef NU_VULKAN_HPP
#define NU_VULKAN_HPP

#include "System/DynamicLibrary.hpp"
#include "VulkanLoader.hpp"
#include "WindowParameters.hpp"

#include "Math/Matrix4.hpp"

namespace nu
{

class Vulkan
{
	public:
		Vulkan();
		~Vulkan();

		bool initialize(const WindowParameters& windowParameters);
		bool draw();
		bool resize();
		void deinitialize();
		void updateTime();
		bool isReady() const;
		
		bool updateMatrices(const Matrix4f& m, const Matrix4f& v, const Matrix4f& p);

		struct QueueParameters 
		{
			VkQueue handle;
			uint32_t familyIndex;
		};

		struct SwapchainParameters 
		{
			VkSmartSwapchainKHR handle;
			VkFormat format;
			VkExtent2D size;
			std::vector<VkImage> images;
			std::vector<VkSmartImageView> imageViews;
			std::vector<VkImageView> imageViewsRaw;
		};

	private:
		bool initializeVulkan(const WindowParameters& windowParameters, bool debugReport = true, VkPhysicalDeviceFeatures* desiredDeviceFeatures = nullptr, VkImageUsageFlags swapchainImageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT, bool useDepth = true, VkImageUsageFlags depthAttachmentUsage = VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT);
		bool createSwapchain(VkImageUsageFlags swapchainImageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT, bool useDepth = true, VkImageUsageFlags depthAttachmentUsage = VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT);

		bool updateStagingBuffer(bool force);


		static VkBool32 debugCallback(VkDebugReportFlagsEXT                       flags,
										VkDebugReportObjectTypeEXT                  objectType,
										uint64_t                                    object,
										size_t                                      location,
										int32_t                                     messageCode,
										const char*                                 pLayerPrefix,
										const char*                                 pMessage,
										void*                                       pUserData);


	private:
		// Loading
		DynamicLibrary mDynamicLibrary;
		bool mReady;

		// Instance/Device/Swapcahin
		VkSmartInstance mInstance;
		VkPhysicalDevice mPhysicalDevice;
		VkSmartDevice mLogicalDevice;
		VkSmartSurfaceKHR mPresentationSurface;
		QueueParameters mGraphicsQueue;
		QueueParameters mComputeQueue;
		QueueParameters mPresentQueue;
		SwapchainParameters mSwapchain;
		VkSmartCommandPool mCommandPool;
		std::vector<VkSmartImage> mDepthImages;
		std::vector<VkSmartDeviceMemory> mDepthImagesMemory;
		std::vector<VulkanLoader::FrameResources> mFramesResources;
		static uint32_t const framesCount = 3; // TODO : Unhardcode
		static VkFormat const depthFormat = VK_FORMAT_D16_UNORM; // TODO : Unhardcode

		// 
		VkDebugReportCallbackEXT mDebug;

		VulkanLoader::Mesh mModel;
		VkSmartBuffer mVertexBuffer;
		VkSmartDeviceMemory mVertexBufferMemory;

		VkSmartDescriptorSetLayout mDescriptorSetLayout;
		VkSmartDescriptorPool mDescriptorPool;
		std::vector<VkDescriptorSet> mDescriptorSets;

		VkSmartRenderPass mRenderPass;
		VkSmartPipelineLayout mPipelineLayout;
		VkSmartPipeline mPipeline;

		VkSmartBuffer mStagingBuffer;
		VkSmartDeviceMemory mStagingBufferMemory;
		bool mUpdateUniformBuffer;
		VkSmartBuffer mUniformBuffer;
		VkSmartDeviceMemory mUniformBufferMemory;

};

} // namespace nu

#endif // NU_VULKAN_HPP
