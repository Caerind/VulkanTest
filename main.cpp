#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#define TINYOBJLOADER_IMPLEMENTATION
#include "tiny_obj_loader.h"

#include "Math/Vector2.hpp"
#include "Math/Vector3.hpp"
#include "Math/Matrix4.hpp"

#include <iostream>
#include <stdexcept>
#include <functional>
#include <cstdlib>
#include <cstring>
#include <vector>
#include <set>
#include <algorithm>
#include <fstream>
#include <array>
#include <chrono>
#include <unordered_map>

struct Vertex
{
	nu::Vector3f position;
	nu::Vector3f color;
	nu::Vector2f texCoord;

	bool operator==(const Vertex& other) const 
	{
		return position == other.position && color == other.color && texCoord == other.texCoord;
	}

	static VkVertexInputBindingDescription getBindingDescription() 
	{
		VkVertexInputBindingDescription bindingDescription = {};
		bindingDescription.binding = 0;
		bindingDescription.stride = sizeof(Vertex);
		bindingDescription.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;
		return bindingDescription;
	}

	static std::array<VkVertexInputAttributeDescription, 3> getAttributeDescriptions() 
	{
		std::array<VkVertexInputAttributeDescription, 3> attributeDescriptions = {};

		attributeDescriptions[0].binding = 0;
		attributeDescriptions[0].location = 0;
		attributeDescriptions[0].format = VK_FORMAT_R32G32B32_SFLOAT;
		attributeDescriptions[0].offset = offsetof(Vertex, position);

		attributeDescriptions[1].binding = 0;
		attributeDescriptions[1].location = 1;
		attributeDescriptions[1].format = VK_FORMAT_R32G32B32_SFLOAT;
		attributeDescriptions[1].offset = offsetof(Vertex, color);

		attributeDescriptions[2].binding = 0;
		attributeDescriptions[2].location = 2;
		attributeDescriptions[2].format = VK_FORMAT_R32G32_SFLOAT;
		attributeDescriptions[2].offset = offsetof(Vertex, texCoord);

		return attributeDescriptions;
	}
};

namespace std 
{
	template<> struct hash<Vertex> 
	{
		unsigned int operator()(Vertex const& vertex) const 
		{
			return ((hash<nu::Vector3f>()(vertex.position) ^ (hash<nu::Vector3f>()(vertex.color) << 1)) >> 1) ^ (hash<nu::Vector2f>()(vertex.texCoord) << 1);
		}
	};
}

struct UniformBufferObject 
{
	nu::Matrix4f model;
	nu::Matrix4f view;
	nu::Matrix4f proj;
};

const int winWidth = 800;
const int winHeight = 600;

const std::string modelPath = "../Models/chalet.obj";
const std::string texturePath = "../Textures/chalet.jpg";

const unsigned int maxFramesInFlight = 2;

const std::vector<const char*> validationLayers = 
{
	"VK_LAYER_LUNARG_standard_validation"
};

const std::vector<const char*> deviceExtensions = 
{
	VK_KHR_SWAPCHAIN_EXTENSION_NAME
};

const bool useAnisotropy = true;

#ifdef NDEBUG
	const bool enableValidationLayers = false;
	const bool checkExtensions = false;
	const bool printExtensions = false;
#else
	const bool enableValidationLayers = true;
	const bool checkExtensions = true;
	const bool printExtensions = true;
#endif

struct QueueFamilyIndices 
{
	int graphicsFamily = -1;
	int presentFamily = -1;

	bool isComplete() 
	{
		return graphicsFamily >= 0 && presentFamily >= 0;
	}
};

struct SwapChainSupportDetails 
{
	VkSurfaceCapabilitiesKHR capabilities;
	std::vector<VkSurfaceFormatKHR> formats;
	std::vector<VkPresentModeKHR> presentModes;
};

VkResult CreateDebugUtilsMessengerEXT(VkInstance instance, const VkDebugUtilsMessengerCreateInfoEXT* pCreateInfo, const VkAllocationCallbacks* pAllocator, VkDebugUtilsMessengerEXT* pCallback) 
{
	auto func = (PFN_vkCreateDebugUtilsMessengerEXT)vkGetInstanceProcAddr(instance, "vkCreateDebugUtilsMessengerEXT");
	if (func != nullptr) 
	{
		return func(instance, pCreateInfo, pAllocator, pCallback);
	}
	else 
	{
		return VK_ERROR_EXTENSION_NOT_PRESENT;
	}
}

void DestroyDebugUtilsMessengerEXT(VkInstance instance, VkDebugUtilsMessengerEXT callback, const VkAllocationCallbacks* pAllocator) 
{
	auto func = (PFN_vkDestroyDebugUtilsMessengerEXT)vkGetInstanceProcAddr(instance, "vkDestroyDebugUtilsMessengerEXT");
	if (func != nullptr) 
	{
		func(instance, callback, pAllocator);
	}
}

static std::vector<char> ReadFile(const std::string& filename) 
{
	std::ifstream file(filename, std::ios::ate | std::ios::binary);
	if (!file.is_open())
	{
		throw std::runtime_error("Failed to open file");
	}

	unsigned int fileSize = (unsigned int)file.tellg();

	std::vector<char> buffer(fileSize); 

	file.seekg(0);
	file.read(buffer.data(), fileSize);

	file.close();

	return buffer;
}

class HelloTriangleApplication 
{
	public:
		HelloTriangleApplication(int winWidth, int winHeight, const std::string& winTitle)
		{
			initWindow(winWidth, winHeight, winTitle);
			initVulkan();
		}

		~HelloTriangleApplication()
		{
			cleanup();
		}

		void run() 
		{
			mainLoop();
		}

	private:
		void initWindow(int winWidth, int winHeight, const std::string& winTitle)
		{
			glfwInit();

			glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);

			mWindow = glfwCreateWindow(winWidth, winHeight, winTitle.c_str(), nullptr, nullptr);
			glfwSetWindowUserPointer(mWindow, this);
			glfwSetFramebufferSizeCallback(mWindow, framebufferResizeCallback);
		}

		void initVulkan() 
		{
			createInstance();
			setupDebugCallback();
			createSurface();
			pickPhysicalDevice();
			createLogicalDevice();
			//createAllocator();
			createSwapChain();
			createImageViews();
			createRenderPass();
			createDescriptorSetLayout();
			createGraphicsPipeline();
			createCommandPool();
			createColorResources();
			createDepthResources();
			createFramebuffers();
			createTextureImage();
			createTextureImageView();
			createTextureSampler();
			loadModel();
			createVertexBuffer();
			createIndexBuffer();
			createUniformBuffer();
			createDescriptorPool();
			createDescriptorSets();
			createCommandBuffers();
			createSyncObjects();
		}

		void mainLoop() 
		{
			while (!glfwWindowShouldClose(mWindow))
			{
				glfwPollEvents();
				drawFrame();
			}

			// Wait before cleaning up
			vkDeviceWaitIdle(mDevice);
		}

		void drawFrame()
		{
			vkWaitForFences(mDevice, 1, &mInFlightFences[mCurrentFrame], VK_TRUE, std::numeric_limits<unsigned long long>::max());

			unsigned int imageIndex;
			VkResult result = vkAcquireNextImageKHR(mDevice, mSwapChain, std::numeric_limits<unsigned long long>::max(), mImageAvailableSemaphores[mCurrentFrame], VK_NULL_HANDLE, &imageIndex);

			if (result == VK_ERROR_OUT_OF_DATE_KHR)
			{
				recreateSwapChain();
				return;
			}
			else if (result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR)
			{
				throw std::runtime_error("Failed to acquire swap chain image");
			}

			updateUniformBuffer(imageIndex);

			VkSemaphore waitSemaphores[] = { mImageAvailableSemaphores[mCurrentFrame] };
			VkPipelineStageFlags waitStages[] = { VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT }; 
			VkSemaphore signalSemaphores[] = { mRenderFinishedSemaphores[mCurrentFrame] };

			VkSubmitInfo submitInfo = {};
			submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
			submitInfo.waitSemaphoreCount = 1;
			submitInfo.pWaitSemaphores = waitSemaphores;
			submitInfo.pWaitDstStageMask = waitStages;
			submitInfo.commandBufferCount = 1;
			submitInfo.pCommandBuffers = &mCommandBuffers[imageIndex]; 
			submitInfo.signalSemaphoreCount = 1;
			submitInfo.pSignalSemaphores = signalSemaphores;

			vkResetFences(mDevice, 1, &mInFlightFences[mCurrentFrame]);

			if (vkQueueSubmit(mGraphicsQueue, 1, &submitInfo, mInFlightFences[mCurrentFrame]) != VK_SUCCESS) 
			{
				throw std::runtime_error("Failed to submit draw command buffer");
			}

			VkSwapchainKHR swapChains[] = { mSwapChain };

			VkPresentInfoKHR presentInfo = {};
			presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
			presentInfo.waitSemaphoreCount = 1;
			presentInfo.pWaitSemaphores = signalSemaphores;
			presentInfo.swapchainCount = 1;
			presentInfo.pSwapchains = swapChains;
			presentInfo.pImageIndices = &imageIndex;
			presentInfo.pResults = nullptr;

			result = vkQueuePresentKHR(mPresentQueue, &presentInfo); 

			if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR || mFramebufferResized)
			{
				mFramebufferResized = false;
				recreateSwapChain();
			}
			else if (result != VK_SUCCESS)
			{
				throw std::runtime_error("Failed to present swap chain image");
			}
			
			mCurrentFrame = (mCurrentFrame + 1) % maxFramesInFlight;
		}

		void cleanup() 
		{
			cleanupSwapChain();

			vkDestroySampler(mDevice, mTextureSampler, nullptr);
			vkDestroyImageView(mDevice, mTextureImageView, nullptr);

			vkDestroyImage(mDevice, mTextureImage, nullptr);
			vkFreeMemory(mDevice, mTextureImageMemory, nullptr);

			vkDestroyDescriptorPool(mDevice, mDescriptorPool, nullptr);

			vkDestroyDescriptorSetLayout(mDevice, mDescriptorSetLayout, nullptr);

			for (unsigned int i = 0; i < mSwapChainImages.size(); i++) 
			{
				vkDestroyBuffer(mDevice, mUniformBuffers[i], nullptr);
				vkFreeMemory(mDevice, mUniformBuffersMemory[i], nullptr);
			}

			vkDestroyBuffer(mDevice, mIndexBuffer, nullptr);
			vkFreeMemory(mDevice, mIndexBufferMemory, nullptr);

			vkDestroyBuffer(mDevice, mVertexBuffer, nullptr);
			vkFreeMemory(mDevice, mVertexBufferMemory, nullptr);

			for (unsigned int i = 0; i < maxFramesInFlight; i++)
			{
				vkDestroySemaphore(mDevice, mRenderFinishedSemaphores[i], nullptr);
				vkDestroySemaphore(mDevice, mImageAvailableSemaphores[i], nullptr);
				vkDestroyFence(mDevice, mInFlightFences[i], nullptr);
			}

			vkDestroyCommandPool(mDevice, mCommandPool, nullptr);

			//vmaDestroyAllocator(mAllocator);

			vkDestroyDevice(mDevice, nullptr);

			if (enableValidationLayers)
			{
				DestroyDebugUtilsMessengerEXT(mInstance, mCallback, nullptr);
			}

			vkDestroySurfaceKHR(mInstance, mSurface, nullptr);
			vkDestroyInstance(mInstance, nullptr);

			glfwDestroyWindow(mWindow);

			glfwTerminate();
		}

		void cleanupSwapChain()
		{
			vkDestroyImageView(mDevice, mColorImageView, nullptr);
			vkDestroyImage(mDevice, mColorImage, nullptr);
			vkFreeMemory(mDevice, mColorImageMemory, nullptr);

			vkDestroyImageView(mDevice, mDepthImageView, nullptr);
			vkDestroyImage(mDevice, mDepthImage, nullptr);
			vkFreeMemory(mDevice, mDepthImageMemory, nullptr);

			for (unsigned int i = 0; i < mSwapChainFramebuffers.size(); i++) 
			{
				vkDestroyFramebuffer(mDevice, mSwapChainFramebuffers[i], nullptr);
			}

			vkFreeCommandBuffers(mDevice, mCommandPool, static_cast<unsigned int>(mCommandBuffers.size()), mCommandBuffers.data());

			vkDestroyPipeline(mDevice, mGraphicsPipeline, nullptr);
			vkDestroyPipelineLayout(mDevice, mPipelineLayout, nullptr);
			vkDestroyRenderPass(mDevice, mRenderPass, nullptr);

			for (unsigned int i = 0; i < mSwapChainImageViews.size(); i++) 
			{
				vkDestroyImageView(mDevice, mSwapChainImageViews[i], nullptr);
			}

			vkDestroySwapchainKHR(mDevice, mSwapChain, nullptr);
		}

		void recreateSwapChain()
		{
			int width = 0, height = 0;
			while (width == 0 || height == 0) 
			{
				glfwGetFramebufferSize(mWindow, &width, &height);
				glfwWaitEvents();
			}

			vkDeviceWaitIdle(mDevice);

			cleanupSwapChain();

			createSwapChain();
			createImageViews();
			createRenderPass();
			createGraphicsPipeline();
			createColorResources();
			createDepthResources();
			createFramebuffers();
			createCommandBuffers();
		}

		void createInstance()
		{
			// Application Info
			VkApplicationInfo appInfo = {};
			appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
			appInfo.pApplicationName = "Hello Triangle";
			appInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
			appInfo.pEngineName = "No Engine";
			appInfo.engineVersion = VK_MAKE_VERSION(1, 0, 0);
			appInfo.apiVersion = VK_API_VERSION_1_0;

			auto extensions = getRequiredInstanceExtensions();

			if (checkExtensions && !checkExtensionsSupport(extensions, printExtensions))
			{
				throw std::runtime_error("Some extensions are requested by GLFW, but are not available from Vulkan");
			}

			if (enableValidationLayers && !checkValidationLayerSupport()) 
			{
				throw std::runtime_error("Validation layers requested, but not available");
			}

			// Instance Create Info
			VkInstanceCreateInfo createInfo = {};
			createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
			createInfo.pApplicationInfo = &appInfo;
			createInfo.enabledExtensionCount = static_cast<unsigned int>(extensions.size());
			createInfo.ppEnabledExtensionNames = extensions.data();
			if (enableValidationLayers) 
			{
				createInfo.enabledLayerCount = static_cast<unsigned int>(validationLayers.size());
				createInfo.ppEnabledLayerNames = validationLayers.data();
			}
			else 
			{
				createInfo.enabledLayerCount = 0;
			}

			// Create
			if (vkCreateInstance(&createInfo, nullptr, &mInstance) != VK_SUCCESS)
			{
				throw std::runtime_error("Failed to create instance");
			}
		}

		void setupDebugCallback()
		{
			if (!enableValidationLayers)
			{
				return;
			}

			VkDebugUtilsMessengerCreateInfoEXT createInfo = {};
			createInfo.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
			createInfo.messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
			createInfo.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
			createInfo.pfnUserCallback = debugCallback;
			createInfo.pUserData = nullptr;

			if (CreateDebugUtilsMessengerEXT(mInstance, &createInfo, nullptr, &mCallback) != VK_SUCCESS)
			{
				throw std::runtime_error("Failed to set up Debug Callback");
			}
		}

		void createSurface()
		{
			if (glfwCreateWindowSurface(mInstance, mWindow, nullptr, &mSurface) != VK_SUCCESS)
			{
				throw std::runtime_error("Failed to create window surface");
			}
		}

		void pickPhysicalDevice()
		{
			unsigned int deviceCount = 0;
			vkEnumeratePhysicalDevices(mInstance, &deviceCount, nullptr);

			if (deviceCount == 0)
			{
				throw std::runtime_error("Failed to find GPUs with Vulkan support");
			}

			std::vector<VkPhysicalDevice> devices(deviceCount);
			vkEnumeratePhysicalDevices(mInstance, &deviceCount, devices.data());

			for (const auto& device : devices)
			{
				if (isDeviceSuitable(device)) 
				{
					mPhysicalDevice = device;
					mMsaaSamples = getMaxUsableSampleCount();
					break;
				}
			}

			if (mPhysicalDevice == VK_NULL_HANDLE) 
			{
				throw std::runtime_error("Failed to find a suitable GPU");
			}
		}

		void createLogicalDevice()
		{
			QueueFamilyIndices indices = queryQueueFamilies(mPhysicalDevice);

			std::vector<VkDeviceQueueCreateInfo> queueCreateInfos;
			std::set<int> uniqueQueueFamilies = { indices.graphicsFamily, indices.presentFamily };

			float queuePriority = 1.0f;
			for (int queueFamily : uniqueQueueFamilies) 
			{
				VkDeviceQueueCreateInfo queueCreateInfo = {};
				queueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
				queueCreateInfo.queueFamilyIndex = queueFamily;
				queueCreateInfo.queueCount = 1;
				queueCreateInfo.pQueuePriorities = &queuePriority;
				queueCreateInfos.push_back(queueCreateInfo);
			}

			VkPhysicalDeviceFeatures deviceFeatures = {};
			if (useAnisotropy)
			{
				deviceFeatures.samplerAnisotropy = VK_TRUE;
			}
			deviceFeatures.sampleRateShading = VK_TRUE;

			VkDeviceCreateInfo createInfo = {};
			createInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
			createInfo.queueCreateInfoCount = static_cast<unsigned int>(queueCreateInfos.size());
			createInfo.pQueueCreateInfos = queueCreateInfos.data();
			createInfo.pEnabledFeatures = &deviceFeatures;

			createInfo.enabledExtensionCount = static_cast<unsigned int>(deviceExtensions.size());
			createInfo.ppEnabledExtensionNames = deviceExtensions.data();

			if (enableValidationLayers) 
			{
				createInfo.enabledLayerCount = static_cast<uint32_t>(validationLayers.size());
				createInfo.ppEnabledLayerNames = validationLayers.data();
			}
			else 
			{
				createInfo.enabledLayerCount = 0;
			}

			if (vkCreateDevice(mPhysicalDevice, &createInfo, nullptr, &mDevice) != VK_SUCCESS) 
			{
				throw std::runtime_error("Failed to create logical device");
			}

			vkGetDeviceQueue(mDevice, indices.graphicsFamily, 0, &mGraphicsQueue);
			vkGetDeviceQueue(mDevice, indices.presentFamily, 0, &mPresentQueue);
		}

		/*
		void createAllocator()
		{
			VmaAllocatorCreateInfo allocatorInfo = {};
			allocatorInfo.physicalDevice = mPhysicalDevice;
			allocatorInfo.device = mDevice;

			vmaCreateAllocator(&allocatorInfo, &mAllocator);
		}
		*/

		void createSwapChain()
		{
			SwapChainSupportDetails swapChainSupport = querySwapChainSupport(mPhysicalDevice);

			VkSurfaceFormatKHR surfaceFormat = chooseSwapSurfaceFormat(swapChainSupport.formats);
			VkPresentModeKHR presentMode = chooseSwapPresentMode(swapChainSupport.presentModes);
			VkExtent2D extent = chooseSwapExtent(swapChainSupport.capabilities);

			unsigned int imageCount = swapChainSupport.capabilities.minImageCount + 1;
			if (swapChainSupport.capabilities.maxImageCount > 0 && imageCount > swapChainSupport.capabilities.maxImageCount) 
			{
				imageCount = swapChainSupport.capabilities.maxImageCount;
			}

			QueueFamilyIndices indices = queryQueueFamilies(mPhysicalDevice);
			unsigned int queueFamilyIndices[] = { (unsigned int)indices.graphicsFamily, (unsigned int)indices.presentFamily };

			VkSwapchainCreateInfoKHR createInfo = {};
			createInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
			createInfo.surface = mSurface;
			createInfo.minImageCount = imageCount;
			createInfo.imageFormat = surfaceFormat.format;
			createInfo.imageColorSpace = surfaceFormat.colorSpace;
			createInfo.imageExtent = extent;
			createInfo.imageArrayLayers = 1;
			createInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
			if (indices.graphicsFamily != indices.presentFamily) 
			{
				createInfo.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
				createInfo.queueFamilyIndexCount = 2;
				createInfo.pQueueFamilyIndices = queueFamilyIndices;
			}
			else 
			{
				createInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
				createInfo.queueFamilyIndexCount = 0;
				createInfo.pQueueFamilyIndices = nullptr;
			}
			createInfo.preTransform = swapChainSupport.capabilities.currentTransform; 
			createInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
			createInfo.presentMode = presentMode;
			createInfo.clipped = VK_TRUE; 
			createInfo.oldSwapchain = VK_NULL_HANDLE;

			if (vkCreateSwapchainKHR(mDevice, &createInfo, nullptr, &mSwapChain) != VK_SUCCESS) 
			{
				throw std::runtime_error("Failed to create swap chain");
			}

			mSwapChainImageFormat = surfaceFormat.format;
			mSwapChainExtent = extent;

			vkGetSwapchainImagesKHR(mDevice, mSwapChain, &imageCount, nullptr);
			mSwapChainImages.resize(imageCount);
			vkGetSwapchainImagesKHR(mDevice, mSwapChain, &imageCount, mSwapChainImages.data());
		}

		void createImageViews()
		{
			mSwapChainImageViews.resize(mSwapChainImages.size());
			for (unsigned int i = 0; i < mSwapChainImages.size(); i++) 
			{
				mSwapChainImageViews[i] = createImageView(mSwapChainImages[i], mSwapChainImageFormat, VK_IMAGE_ASPECT_COLOR_BIT, 1);
			}
		}
		
		void createRenderPass()
		{
			VkAttachmentDescription colorAttachment = {};
			colorAttachment.format = mSwapChainImageFormat;
			colorAttachment.samples = mMsaaSamples; 
			colorAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
			colorAttachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
			colorAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
			colorAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
			colorAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
			colorAttachment.finalLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

			VkAttachmentDescription depthAttachment = {};
			depthAttachment.format = findDepthFormat();
			depthAttachment.samples = mMsaaSamples;
			depthAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
			depthAttachment.storeOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
			depthAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
			depthAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
			depthAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
			depthAttachment.finalLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

			VkAttachmentDescription colorAttachmentResolve = {};
			colorAttachmentResolve.format = mSwapChainImageFormat;
			colorAttachmentResolve.samples = VK_SAMPLE_COUNT_1_BIT;
			colorAttachmentResolve.loadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
			colorAttachmentResolve.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
			colorAttachmentResolve.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
			colorAttachmentResolve.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
			colorAttachmentResolve.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
			colorAttachmentResolve.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

			VkAttachmentReference colorAttachmentRef = {};
			colorAttachmentRef.attachment = 0;
			colorAttachmentRef.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

			VkAttachmentReference depthAttachmentRef = {};
			depthAttachmentRef.attachment = 1;
			depthAttachmentRef.layout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

			VkAttachmentReference colorAttachmentResolveRef = {};
			colorAttachmentResolveRef.attachment = 2;
			colorAttachmentResolveRef.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

			VkSubpassDescription subpass = {};
			subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
			subpass.colorAttachmentCount = 1;
			subpass.pColorAttachments = &colorAttachmentRef;
			subpass.pDepthStencilAttachment = &depthAttachmentRef; 
			subpass.pResolveAttachments = &colorAttachmentResolveRef;

			VkSubpassDependency dependency = {};
			dependency.srcSubpass = VK_SUBPASS_EXTERNAL;
			dependency.dstSubpass = 0;
			dependency.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
			dependency.srcAccessMask = 0;
			dependency.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
			dependency.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_READ_BIT | VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;

			std::array<VkAttachmentDescription, 3> attachments = { colorAttachment, depthAttachment, colorAttachmentResolve };

			VkRenderPassCreateInfo renderPassInfo = {};
			renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
			renderPassInfo.attachmentCount = static_cast<unsigned int>(attachments.size());
			renderPassInfo.pAttachments = attachments.data();
			renderPassInfo.subpassCount = 1;
			renderPassInfo.pSubpasses = &subpass;
			renderPassInfo.dependencyCount = 1;
			renderPassInfo.pDependencies = &dependency;

			if (vkCreateRenderPass(mDevice, &renderPassInfo, nullptr, &mRenderPass) != VK_SUCCESS) 
			{
				throw std::runtime_error("Failed to create render pass");
			}
		}

		void createDescriptorSetLayout()
		{
			VkDescriptorSetLayoutBinding uboLayoutBinding = {};
			uboLayoutBinding.binding = 0;
			uboLayoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
			uboLayoutBinding.descriptorCount = 1;
			uboLayoutBinding.stageFlags = VK_SHADER_STAGE_VERTEX_BIT;
			uboLayoutBinding.pImmutableSamplers = nullptr;

			VkDescriptorSetLayoutBinding samplerLayoutBinding = {};
			samplerLayoutBinding.binding = 1;
			samplerLayoutBinding.descriptorCount = 1;
			samplerLayoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
			samplerLayoutBinding.pImmutableSamplers = nullptr;
			samplerLayoutBinding.stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;

			std::array<VkDescriptorSetLayoutBinding, 2> bindings = { uboLayoutBinding, samplerLayoutBinding };

			VkDescriptorSetLayoutCreateInfo layoutInfo = {};
			layoutInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
			layoutInfo.bindingCount = static_cast<unsigned int>(bindings.size());
			layoutInfo.pBindings = bindings.data();

			if (vkCreateDescriptorSetLayout(mDevice, &layoutInfo, nullptr, &mDescriptorSetLayout) != VK_SUCCESS) 
			{
				throw std::runtime_error("Failed to create descriptor set layout");
			}
		}

		void createGraphicsPipeline()
		{
			auto vertShaderCode = ReadFile("../Shaders/vert.spv");
			auto fragShaderCode = ReadFile("../Shaders/frag.spv");

			VkShaderModule vertShaderModule = createShaderModule(vertShaderCode);
			VkShaderModule fragShaderModule = createShaderModule(fragShaderCode);

			VkPipelineShaderStageCreateInfo vertShaderStageInfo = {};
			vertShaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
			vertShaderStageInfo.stage = VK_SHADER_STAGE_VERTEX_BIT; 
			vertShaderStageInfo.module = vertShaderModule;
			vertShaderStageInfo.pName = "main";
			vertShaderStageInfo.pSpecializationInfo = nullptr;

			VkPipelineShaderStageCreateInfo fragShaderStageInfo = {};
			fragShaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
			fragShaderStageInfo.stage = VK_SHADER_STAGE_FRAGMENT_BIT;
			fragShaderStageInfo.module = fragShaderModule;
			fragShaderStageInfo.pName = "main";
			fragShaderStageInfo.pSpecializationInfo = nullptr;

			VkPipelineShaderStageCreateInfo shaderStages[] = { vertShaderStageInfo, fragShaderStageInfo };

			auto bindingDescription = Vertex::getBindingDescription();
			auto attributeDescriptions = Vertex::getAttributeDescriptions();

			VkPipelineVertexInputStateCreateInfo vertexInputInfo = {};
			vertexInputInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
			vertexInputInfo.vertexBindingDescriptionCount = 1;
			vertexInputInfo.pVertexBindingDescriptions = &bindingDescription;
			vertexInputInfo.vertexAttributeDescriptionCount = static_cast<unsigned int>(attributeDescriptions.size());
			vertexInputInfo.pVertexAttributeDescriptions = attributeDescriptions.data();

			VkPipelineInputAssemblyStateCreateInfo inputAssembly = {};
			inputAssembly.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
			inputAssembly.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
			inputAssembly.primitiveRestartEnable = VK_FALSE;

			VkViewport viewport = {};
			viewport.x = 0.0f;
			viewport.y = 0.0f;
			viewport.width = (float)mSwapChainExtent.width;
			viewport.height = (float)mSwapChainExtent.height;
			viewport.minDepth = 0.0f;
			viewport.maxDepth = 1.0f;

			VkRect2D scissor = {};
			scissor.offset = { 0, 0 };
			scissor.extent = mSwapChainExtent;

			VkPipelineViewportStateCreateInfo viewportState = {};
			viewportState.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
			viewportState.viewportCount = 1;
			viewportState.pViewports = &viewport;
			viewportState.scissorCount = 1;
			viewportState.pScissors = &scissor;

			VkPipelineRasterizationStateCreateInfo rasterizer = {};
			rasterizer.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
			rasterizer.depthClampEnable = VK_FALSE;
			rasterizer.rasterizerDiscardEnable = VK_FALSE;
			rasterizer.polygonMode = VK_POLYGON_MODE_FILL;
			rasterizer.lineWidth = 1.0f;
			rasterizer.cullMode = VK_CULL_MODE_BACK_BIT;
			rasterizer.frontFace = VK_FRONT_FACE_COUNTER_CLOCKWISE;
			rasterizer.depthBiasEnable = VK_FALSE;
			rasterizer.depthBiasConstantFactor = 0.0f;
			rasterizer.depthBiasClamp = 0.0f;
			rasterizer.depthBiasSlopeFactor = 0.0f;

			VkPipelineMultisampleStateCreateInfo multisampling = {};
			multisampling.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
			multisampling.sampleShadingEnable = VK_TRUE;
			multisampling.rasterizationSamples = mMsaaSamples;
			multisampling.minSampleShading = 0.2f;
			multisampling.pSampleMask = nullptr;
			multisampling.alphaToCoverageEnable = VK_FALSE;
			multisampling.alphaToOneEnable = VK_FALSE;

			VkPipelineDepthStencilStateCreateInfo depthStencil = {};
			depthStencil.sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
			depthStencil.depthTestEnable = VK_TRUE;
			depthStencil.depthWriteEnable = VK_TRUE;
			depthStencil.depthCompareOp = VK_COMPARE_OP_LESS;
			depthStencil.depthBoundsTestEnable = VK_FALSE;
			depthStencil.minDepthBounds = 0.0f;
			depthStencil.maxDepthBounds = 1.0f;
			depthStencil.stencilTestEnable = VK_FALSE;
			depthStencil.front = {};
			depthStencil.back = {};

			VkPipelineColorBlendAttachmentState colorBlendAttachment = {};
			colorBlendAttachment.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
			colorBlendAttachment.blendEnable = VK_FALSE;
			colorBlendAttachment.srcColorBlendFactor = VK_BLEND_FACTOR_ONE;
			colorBlendAttachment.dstColorBlendFactor = VK_BLEND_FACTOR_ZERO;
			colorBlendAttachment.colorBlendOp = VK_BLEND_OP_ADD;
			colorBlendAttachment.srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE;
			colorBlendAttachment.dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO;
			colorBlendAttachment.alphaBlendOp = VK_BLEND_OP_ADD;

			VkPipelineColorBlendStateCreateInfo colorBlending = {};
			colorBlending.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
			colorBlending.logicOpEnable = VK_FALSE;
			colorBlending.logicOp = VK_LOGIC_OP_COPY;
			colorBlending.attachmentCount = 1;
			colorBlending.pAttachments = &colorBlendAttachment;
			colorBlending.blendConstants[0] = 0.0f;
			colorBlending.blendConstants[1] = 0.0f;
			colorBlending.blendConstants[2] = 0.0f;
			colorBlending.blendConstants[3] = 0.0f;

			VkPipelineLayoutCreateInfo pipelineLayoutInfo = {};
			pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
			pipelineLayoutInfo.setLayoutCount = 1;
			pipelineLayoutInfo.pSetLayouts = &mDescriptorSetLayout;
			pipelineLayoutInfo.pushConstantRangeCount = 0;
			pipelineLayoutInfo.pPushConstantRanges = nullptr;

			if (vkCreatePipelineLayout(mDevice, &pipelineLayoutInfo, nullptr, &mPipelineLayout) != VK_SUCCESS) 
			{
				throw std::runtime_error("Failed to create pipeline layout");
			}

			VkGraphicsPipelineCreateInfo pipelineInfo = {};
			pipelineInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
			pipelineInfo.stageCount = 2;
			pipelineInfo.pStages = shaderStages;
			pipelineInfo.pVertexInputState = &vertexInputInfo;
			pipelineInfo.pInputAssemblyState = &inputAssembly;
			pipelineInfo.pViewportState = &viewportState;
			pipelineInfo.pRasterizationState = &rasterizer;
			pipelineInfo.pMultisampleState = &multisampling;
			pipelineInfo.pDepthStencilState = &depthStencil;
			pipelineInfo.pColorBlendState = &colorBlending;
			pipelineInfo.pDynamicState = nullptr; 
			pipelineInfo.layout = mPipelineLayout;
			pipelineInfo.renderPass = mRenderPass;
			pipelineInfo.subpass = 0;
			pipelineInfo.basePipelineHandle = VK_NULL_HANDLE;
			pipelineInfo.basePipelineIndex = -1;

			if (vkCreateGraphicsPipelines(mDevice, VK_NULL_HANDLE, 1, &pipelineInfo, nullptr, &mGraphicsPipeline) != VK_SUCCESS) 
			{
				throw std::runtime_error("Failed to create graphics pipeline");
			}

			vkDestroyShaderModule(mDevice, fragShaderModule, nullptr);
			vkDestroyShaderModule(mDevice, vertShaderModule, nullptr);
		}

		void createFramebuffers()
		{
			mSwapChainFramebuffers.resize(mSwapChainImageViews.size());

			for (unsigned int i = 0; i < mSwapChainImageViews.size(); i++) 
			{
				std::array<VkImageView, 3> attachments =
				{
					mColorImageView,
					mDepthImageView,
					mSwapChainImageViews[i]
				};

				VkFramebufferCreateInfo framebufferInfo = {};
				framebufferInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
				framebufferInfo.renderPass = mRenderPass;
				framebufferInfo.attachmentCount = static_cast<unsigned int>(attachments.size());
				framebufferInfo.pAttachments = attachments.data();
				framebufferInfo.width = mSwapChainExtent.width;
				framebufferInfo.height = mSwapChainExtent.height;
				framebufferInfo.layers = 1;

				if (vkCreateFramebuffer(mDevice, &framebufferInfo, nullptr, &mSwapChainFramebuffers[i]) != VK_SUCCESS) 
				{
					throw std::runtime_error("Failed to create framebuffer");
				}
			}
		}

		void createCommandPool()
		{
			QueueFamilyIndices queueFamilyIndices = queryQueueFamilies(mPhysicalDevice);

			VkCommandPoolCreateInfo poolInfo = {};
			poolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
			poolInfo.queueFamilyIndex = queueFamilyIndices.graphicsFamily;
			poolInfo.flags = 0;

			if (vkCreateCommandPool(mDevice, &poolInfo, nullptr, &mCommandPool) != VK_SUCCESS) 
			{
				throw std::runtime_error("Failed to create command pool");
			}
		}

		void createColorResources() 
		{
			VkFormat colorFormat = mSwapChainImageFormat;

			createImage(mSwapChainExtent.width, mSwapChainExtent.height, 1, mMsaaSamples, colorFormat, VK_IMAGE_TILING_OPTIMAL, VK_IMAGE_USAGE_TRANSIENT_ATTACHMENT_BIT | VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, mColorImage, mColorImageMemory);
			mColorImageView = createImageView(mColorImage, colorFormat, VK_IMAGE_ASPECT_COLOR_BIT, 1);

			transitionImageLayout(mColorImage, colorFormat, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL, 1);
		}

		void createDepthResources()
		{
			VkFormat depthFormat = findDepthFormat();

			createImage(mSwapChainExtent.width, mSwapChainExtent.height, 1, mMsaaSamples, depthFormat, VK_IMAGE_TILING_OPTIMAL, VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, mDepthImage, mDepthImageMemory);
			mDepthImageView = createImageView(mDepthImage, depthFormat, VK_IMAGE_ASPECT_DEPTH_BIT, 1);

			transitionImageLayout(mDepthImage, depthFormat, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL, 1);
		}

		void createTextureImage()
		{
			int texWidth, texHeight, texChannels;
			unsigned char* pixels = stbi_load(texturePath.c_str(), &texWidth, &texHeight, &texChannels, STBI_rgb_alpha);
			VkDeviceSize imageSize = texWidth * texHeight * 4;
			if (!pixels) 
			{
				throw std::runtime_error("Failed to load texture image");
			}

			mMipLevels = static_cast<unsigned int>(std::floor(std::log2(std::max(texWidth, texHeight)))) + 1;

			VkBuffer stagingBuffer;
			VkDeviceMemory stagingBufferMemory;

			createBuffer(imageSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, stagingBuffer, stagingBufferMemory);

			void* data;
			vkMapMemory(mDevice, stagingBufferMemory, 0, imageSize, 0, &data);
			memcpy(data, pixels, static_cast<size_t>(imageSize));
			vkUnmapMemory(mDevice, stagingBufferMemory);

			stbi_image_free(pixels);

			createImage(texWidth, texHeight, mMipLevels, VK_SAMPLE_COUNT_1_BIT, VK_FORMAT_R8G8B8A8_UNORM, VK_IMAGE_TILING_OPTIMAL, VK_IMAGE_USAGE_TRANSFER_SRC_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, mTextureImage, mTextureImageMemory);
		
			transitionImageLayout(mTextureImage, VK_FORMAT_R8G8B8A8_UNORM, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, mMipLevels);
			copyBufferToImage(stagingBuffer, mTextureImage, static_cast<unsigned int>(texWidth), static_cast<unsigned int>(texHeight));
		
			//transitionImageLayout(mTextureImage, VK_FORMAT_R8G8B8A8_UNORM, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL, mMipLevels);
			generateMipmaps(mTextureImage, VK_FORMAT_R8G8B8A8_UNORM, texWidth, texHeight, mMipLevels);

			vkDestroyBuffer(mDevice, stagingBuffer, nullptr);
			vkFreeMemory(mDevice, stagingBufferMemory, nullptr);
		}

		void createTextureImageView()
		{
			mTextureImageView = createImageView(mTextureImage, VK_FORMAT_R8G8B8A8_UNORM, VK_IMAGE_ASPECT_COLOR_BIT, mMipLevels);
		}

		void createTextureSampler()
		{
			VkSamplerCreateInfo samplerInfo = {};
			samplerInfo.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
			samplerInfo.magFilter = VK_FILTER_LINEAR;
			samplerInfo.minFilter = VK_FILTER_LINEAR;
			samplerInfo.addressModeU = VK_SAMPLER_ADDRESS_MODE_REPEAT;
			samplerInfo.addressModeV = VK_SAMPLER_ADDRESS_MODE_REPEAT;
			samplerInfo.addressModeW = VK_SAMPLER_ADDRESS_MODE_REPEAT;
			if (useAnisotropy)
			{
				samplerInfo.anisotropyEnable = VK_TRUE;
				samplerInfo.maxAnisotropy = 16;
			}
			else
			{
				samplerInfo.anisotropyEnable = VK_FALSE;
				samplerInfo.maxAnisotropy = 1;
			}
			samplerInfo.borderColor = VK_BORDER_COLOR_INT_OPAQUE_BLACK;
			samplerInfo.unnormalizedCoordinates = VK_FALSE;
			samplerInfo.compareEnable = VK_FALSE;
			samplerInfo.compareOp = VK_COMPARE_OP_ALWAYS;
			samplerInfo.mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR;
			samplerInfo.mipLodBias = 0.0f;
			samplerInfo.minLod = 0.0f;
			samplerInfo.maxLod = static_cast<float>(mMipLevels);

			if (vkCreateSampler(mDevice, &samplerInfo, nullptr, &mTextureSampler) != VK_SUCCESS) 
			{
				throw std::runtime_error("Failed to create texture sampler");
			}
		}

		void loadModel()
		{
			tinyobj::attrib_t attrib;
			std::vector<tinyobj::shape_t> shapes;
			std::vector<tinyobj::material_t> materials;
			std::string err;

			if (!tinyobj::LoadObj(&attrib, &shapes, &materials, &err, modelPath.c_str())) 
			{
				throw std::runtime_error(err);
			}

			std::unordered_map<Vertex, uint32_t> uniqueVertices = {};

			for (const auto& shape : shapes) 
			{
				for (const auto& index : shape.mesh.indices) 
				{
					Vertex vertex = {};

					vertex.position = 
					{
						attrib.vertices[3 * index.vertex_index + 0],
						attrib.vertices[3 * index.vertex_index + 1],
						attrib.vertices[3 * index.vertex_index + 2]
					};

					vertex.texCoord = 
					{
						attrib.texcoords[2 * index.texcoord_index + 0],
						1.0f - attrib.texcoords[2 * index.texcoord_index + 1]
					};

					vertex.color = { 1.0f, 1.0f, 1.0f };

					if (uniqueVertices.count(vertex) == 0) 
					{
						uniqueVertices[vertex] = static_cast<uint32_t>(mVertices.size());
						mVertices.push_back(vertex);
					}

					mIndices.push_back(uniqueVertices[vertex]);
				}
			}
		}

		void createVertexBuffer()
		{
			VkDeviceSize bufferSize = sizeof(mVertices[0]) * mVertices.size();

			VkBuffer stagingBuffer;
			VkDeviceMemory stagingBufferMemory;
			createBuffer(bufferSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, stagingBuffer, stagingBufferMemory);

			void* data;
			vkMapMemory(mDevice, stagingBufferMemory, 0, bufferSize, 0, &data);
			memcpy(data, mVertices.data(), (size_t)bufferSize);
			vkUnmapMemory(mDevice, stagingBufferMemory);

			createBuffer(bufferSize, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, mVertexBuffer, mVertexBufferMemory);

			copyBuffer(stagingBuffer, mVertexBuffer, bufferSize);

			vkDestroyBuffer(mDevice, stagingBuffer, nullptr);
			vkFreeMemory(mDevice, stagingBufferMemory, nullptr);
		}

		void createIndexBuffer()
		{
			VkDeviceSize bufferSize = sizeof(mIndices[0]) * mIndices.size();

			VkBuffer stagingBuffer;
			VkDeviceMemory stagingBufferMemory;
			createBuffer(bufferSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, stagingBuffer, stagingBufferMemory);

			void* data;
			vkMapMemory(mDevice, stagingBufferMemory, 0, bufferSize, 0, &data);
			memcpy(data, mIndices.data(), (size_t)bufferSize);
			vkUnmapMemory(mDevice, stagingBufferMemory);

			createBuffer(bufferSize, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_INDEX_BUFFER_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, mIndexBuffer, mIndexBufferMemory);

			copyBuffer(stagingBuffer, mIndexBuffer, bufferSize);

			vkDestroyBuffer(mDevice, stagingBuffer, nullptr);
			vkFreeMemory(mDevice, stagingBufferMemory, nullptr);
		}

		void createUniformBuffer()
		{
			VkDeviceSize bufferSize = sizeof(UniformBufferObject);

			mUniformBuffers.resize(mSwapChainImages.size());
			mUniformBuffersMemory.resize(mSwapChainImages.size());

			for (unsigned int i = 0; i < mSwapChainImages.size(); i++) 
			{
				createBuffer(bufferSize, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, mUniformBuffers[i], mUniformBuffersMemory[i]);
			}
		}

		void createDescriptorPool()
		{
			std::array<VkDescriptorPoolSize, 2> poolSizes = {};
			poolSizes[0].type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
			poolSizes[0].descriptorCount = static_cast<uint32_t>(mSwapChainImages.size());
			poolSizes[1].type = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
			poolSizes[1].descriptorCount = static_cast<uint32_t>(mSwapChainImages.size());

			VkDescriptorPoolCreateInfo poolInfo = {};
			poolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
			poolInfo.poolSizeCount = static_cast<unsigned int>(poolSizes.size());
			poolInfo.pPoolSizes = poolSizes.data();
			poolInfo.maxSets = static_cast<unsigned int>(mSwapChainImages.size());

			if (vkCreateDescriptorPool(mDevice, &poolInfo, nullptr, &mDescriptorPool) != VK_SUCCESS) 
			{
				throw std::runtime_error("Failed to create descriptor pool");
			}
		}

		void createDescriptorSets()
		{
			std::vector<VkDescriptorSetLayout> layouts(mSwapChainImages.size(), mDescriptorSetLayout);

			VkDescriptorSetAllocateInfo allocInfo = {};
			allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
			allocInfo.descriptorPool = mDescriptorPool;
			allocInfo.descriptorSetCount = static_cast<unsigned int>(mSwapChainImages.size());
			allocInfo.pSetLayouts = layouts.data();

			mDescriptorSets.resize(mSwapChainImages.size());
			if (vkAllocateDescriptorSets(mDevice, &allocInfo, &mDescriptorSets[0]) != VK_SUCCESS) 
			{
				throw std::runtime_error("Failed to allocate descriptor sets");
			}

			for (unsigned int i = 0; i < mSwapChainImages.size(); i++) 
			{
				std::array<VkWriteDescriptorSet, 2> descriptorWrites = {};

				// UBO

				VkDescriptorBufferInfo bufferInfo = {};
				bufferInfo.buffer = mUniformBuffers[i];
				bufferInfo.offset = 0;
				bufferInfo.range = sizeof(UniformBufferObject);

				descriptorWrites[0].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
				descriptorWrites[0].dstSet = mDescriptorSets[i];
				descriptorWrites[0].dstBinding = 0;
				descriptorWrites[0].dstArrayElement = 0;
				descriptorWrites[0].descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
				descriptorWrites[0].descriptorCount = 1;
				descriptorWrites[0].pBufferInfo = &bufferInfo;
				descriptorWrites[0].pImageInfo = nullptr;
				descriptorWrites[0].pTexelBufferView = nullptr;

				// Sampler

				VkDescriptorImageInfo imageInfo = {};
				imageInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
				imageInfo.imageView = mTextureImageView;
				imageInfo.sampler = mTextureSampler;

				descriptorWrites[1].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
				descriptorWrites[1].dstSet = mDescriptorSets[i];
				descriptorWrites[1].dstBinding = 1;
				descriptorWrites[1].dstArrayElement = 0;
				descriptorWrites[1].descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
				descriptorWrites[1].descriptorCount = 1;
				descriptorWrites[1].pBufferInfo = nullptr;
				descriptorWrites[1].pImageInfo = &imageInfo;
				descriptorWrites[1].pTexelBufferView = nullptr;


				vkUpdateDescriptorSets(mDevice, static_cast<unsigned int>(descriptorWrites.size()), descriptorWrites.data(), 0, nullptr);
			}
		}

		void createCommandBuffers()
		{
			mCommandBuffers.resize(mSwapChainFramebuffers.size());

			VkCommandBufferAllocateInfo allocInfo = {};
			allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
			allocInfo.commandPool = mCommandPool;
			allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
			allocInfo.commandBufferCount = (unsigned int)mCommandBuffers.size();

			if (vkAllocateCommandBuffers(mDevice, &allocInfo, mCommandBuffers.data()) != VK_SUCCESS) 
			{
				throw std::runtime_error("Failed to allocate command buffers");
			}

			for (unsigned int i = 0; i < mCommandBuffers.size(); i++)
			{
				VkCommandBufferBeginInfo beginInfo = {};
				beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
				beginInfo.flags = VK_COMMAND_BUFFER_USAGE_SIMULTANEOUS_USE_BIT;
				beginInfo.pInheritanceInfo = nullptr;

				if (vkBeginCommandBuffer(mCommandBuffers[i], &beginInfo) != VK_SUCCESS)
				{
					throw std::runtime_error("Failed to begin recording command buffer");
				}

				std::array<VkClearValue, 2> clearValues = {};
				clearValues[0].color = { 0.0f, 0.0f, 0.0f, 1.0f };
				clearValues[1].depthStencil = { 1.0f, 0 };

				VkRenderPassBeginInfo renderPassInfo = {};
				renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
				renderPassInfo.renderPass = mRenderPass;
				renderPassInfo.framebuffer = mSwapChainFramebuffers[i];
				renderPassInfo.renderArea.offset = { 0, 0 };
				renderPassInfo.renderArea.extent = mSwapChainExtent;
				renderPassInfo.clearValueCount = static_cast<unsigned int>(clearValues.size());
				renderPassInfo.pClearValues = clearValues.data();

				vkCmdBeginRenderPass(mCommandBuffers[i], &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);

					vkCmdBindPipeline(mCommandBuffers[i], VK_PIPELINE_BIND_POINT_GRAPHICS, mGraphicsPipeline);

					VkBuffer vertexBuffers[] = { mVertexBuffer };
					VkDeviceSize offsets[] = { 0 };

					vkCmdBindVertexBuffers(mCommandBuffers[i], 0, 1, vertexBuffers, offsets); 
					vkCmdBindIndexBuffer(mCommandBuffers[i], mIndexBuffer, 0, VK_INDEX_TYPE_UINT32);

					vkCmdBindDescriptorSets(mCommandBuffers[i], VK_PIPELINE_BIND_POINT_GRAPHICS, mPipelineLayout, 0, 1, &mDescriptorSets[i], 0, nullptr);

					vkCmdDrawIndexed(mCommandBuffers[i], static_cast<unsigned int>(mIndices.size()), 1, 0, 0, 0);

				vkCmdEndRenderPass(mCommandBuffers[i]);

				if (vkEndCommandBuffer(mCommandBuffers[i]) != VK_SUCCESS) 
				{
					throw std::runtime_error("Failed to record command buffer");
				}
			}
		}

		void createSyncObjects()
		{
			mImageAvailableSemaphores.resize(maxFramesInFlight);
			mRenderFinishedSemaphores.resize(maxFramesInFlight);
			mInFlightFences.resize(maxFramesInFlight);

			VkSemaphoreCreateInfo semaphoreInfo = {};
			semaphoreInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

			VkFenceCreateInfo fenceInfo = {};
			fenceInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
			fenceInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;

			for (unsigned int i = 0; i < maxFramesInFlight; i++)
			{
				if (vkCreateSemaphore(mDevice, &semaphoreInfo, nullptr, &mImageAvailableSemaphores[i]) != VK_SUCCESS 
				|| vkCreateSemaphore(mDevice, &semaphoreInfo, nullptr, &mRenderFinishedSemaphores[i]) != VK_SUCCESS
				|| vkCreateFence(mDevice, &fenceInfo, nullptr, &mInFlightFences[i]) != VK_SUCCESS)
				{
					throw std::runtime_error("Failed to create synchronization objects for a frame");
				}
			}
		}

		void updateUniformBuffer(unsigned int currentImage)
		{
			static auto startTime = std::chrono::high_resolution_clock::now();

			auto currentTime = std::chrono::high_resolution_clock::now();
			float time = std::chrono::duration<float, std::chrono::seconds::period>(currentTime - startTime).count();

			UniformBufferObject ubo = {};
			ubo.model = nu::Matrix4f(nu::Matrix3f::rotationZ(30.0f * time));
			ubo.view = nu::Matrix4f::lookAt(nu::Vector3f(2.0f, 2.0f, 2.0f), nu::Vector3f(0.0f, 0.0f, 0.0f), nu::Vector3f(0.0f, 0.0f, 1.0f));
			ubo.proj = nu::Matrix4f::perspective(45.0f, mSwapChainExtent.width / (float)mSwapChainExtent.height, 0.1f, 10.0f);
			ubo.proj(1, 1) *= -1.0f;

			void* data;
			vkMapMemory(mDevice, mUniformBuffersMemory[currentImage], 0, sizeof(ubo), 0, &data);
			memcpy(data, &ubo, sizeof(ubo));
			vkUnmapMemory(mDevice, mUniformBuffersMemory[currentImage]);
		}

		std::vector<const char*> getRequiredInstanceExtensions()
		{
			unsigned int glfwExtensionCount = 0;
			const char** glfwExtensions;
			glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);

			std::vector<const char*> extensions(glfwExtensions, glfwExtensions + glfwExtensionCount);

			if (enableValidationLayers)
			{
				extensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
			}

			return extensions;
		}

		bool checkExtensionsSupport(std::vector<const char*>& extensions, bool printExtensions)
		{
			unsigned int vkExtensionCount = 0;
			vkEnumerateInstanceExtensionProperties(nullptr, &vkExtensionCount, nullptr);
			std::vector<VkExtensionProperties> vkExtensions(vkExtensionCount);
			vkEnumerateInstanceExtensionProperties(nullptr, &vkExtensionCount, vkExtensions.data());

			if (printExtensions)
			{
				printf("Vulkan available extensions : \n");
				for (const auto& extension : vkExtensions)
				{
					printf("\t%s\n", extension.extensionName);
				}

				printf("\nRequired extensions : \n");
			}

			bool extensionMissing = false;
			for (const auto& requiredExtension : extensions)
			{
				bool extensionFound = false;
				for (const auto& extension : vkExtensions)
				{
					if (strcmp(requiredExtension, extension.extensionName) == 0)
					{
						extensionFound = true;
						break;
					}
				}

				if (printExtensions)
				{
					printf("\t%s : %s\n", requiredExtension, ((extensionFound) ? "Found" : "Not found"));
				}

				if (!extensionFound)
				{
					extensionMissing = true;
				}
			}

			return !extensionMissing;
		}

		bool checkValidationLayerSupport()
		{
			unsigned int layerCount;
			vkEnumerateInstanceLayerProperties(&layerCount, nullptr);

			std::vector<VkLayerProperties> availableLayers(layerCount);
			vkEnumerateInstanceLayerProperties(&layerCount, availableLayers.data());

			for (const char* layerName : validationLayers)
			{
				bool layerFound = false;
				for (const auto& layerProperties : availableLayers)
				{
					if (strcmp(layerName, layerProperties.layerName) == 0)
					{
						layerFound = true;
						break;
					}
				}
				if (!layerFound)
				{
					return false;
				}
			}

			return true;
		}

		bool isDeviceSuitable(VkPhysicalDevice device)
		{
			QueueFamilyIndices indices = queryQueueFamilies(device);

			bool extensionsSupported = checkDeviceExtensionSupport(device);

			bool swapChainAdequate = false;
			if (extensionsSupported)
			{
				SwapChainSupportDetails swapChainSupport = querySwapChainSupport(device);
				swapChainAdequate = !swapChainSupport.formats.empty() && !swapChainSupport.presentModes.empty();
			}

			if (useAnisotropy)
			{
				VkPhysicalDeviceFeatures supportedFeatures;
				vkGetPhysicalDeviceFeatures(device, &supportedFeatures);

				return indices.isComplete() && extensionsSupported && swapChainAdequate && supportedFeatures.samplerAnisotropy;
			}
			else
			{
				return indices.isComplete() && extensionsSupported && swapChainAdequate;
			}
		}

		QueueFamilyIndices queryQueueFamilies(VkPhysicalDevice device)
		{
			QueueFamilyIndices indices;

			uint32_t queueFamilyCount = 0;
			vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, nullptr);

			std::vector<VkQueueFamilyProperties> queueFamilies(queueFamilyCount);
			vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, queueFamilies.data());

			int i = 0;
			for (const auto& queueFamily : queueFamilies)
			{
				VkBool32 presentSupport = false;
				vkGetPhysicalDeviceSurfaceSupportKHR(device, i, mSurface, &presentSupport);

				if (queueFamily.queueCount > 0 && queueFamily.queueFlags & VK_QUEUE_GRAPHICS_BIT) 
				{
					indices.graphicsFamily = i;
				}
				if (queueFamily.queueCount > 0 && presentSupport)
				{
					indices.presentFamily = i;
				}

				if (indices.isComplete())
				{
					break;
				}

				i++;
			}

			return indices;
		}

		bool checkDeviceExtensionSupport(VkPhysicalDevice device)
		{
			unsigned int extensionCount;
			vkEnumerateDeviceExtensionProperties(device, nullptr, &extensionCount, nullptr);

			std::vector<VkExtensionProperties> availableExtensions(extensionCount);
			vkEnumerateDeviceExtensionProperties(device, nullptr, &extensionCount, availableExtensions.data());

			std::set<std::string> requiredExtensions(deviceExtensions.begin(), deviceExtensions.end());

			for (const auto& extension : availableExtensions) 
			{
				requiredExtensions.erase(extension.extensionName);
			}

			return requiredExtensions.empty();
		}

		SwapChainSupportDetails querySwapChainSupport(VkPhysicalDevice device) 
		{
			SwapChainSupportDetails details;

			// Surface capabilities
			vkGetPhysicalDeviceSurfaceCapabilitiesKHR(device, mSurface, &details.capabilities);

			// Supported surface formats
			unsigned int formatCount;
			vkGetPhysicalDeviceSurfaceFormatsKHR(device, mSurface, &formatCount, nullptr);
			if (formatCount != 0) 
			{
				details.formats.resize(formatCount);
				vkGetPhysicalDeviceSurfaceFormatsKHR(device, mSurface, &formatCount, details.formats.data());
			}

			// Supported present modes
			unsigned int presentModeCount;
			vkGetPhysicalDeviceSurfacePresentModesKHR(device, mSurface, &presentModeCount, nullptr);
			if (presentModeCount != 0) 
			{
				details.presentModes.resize(presentModeCount);
				vkGetPhysicalDeviceSurfacePresentModesKHR(device, mSurface, &presentModeCount, details.presentModes.data());
			}

			return details;
		}

		VkSurfaceFormatKHR chooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& availableFormats) 
		{
			if (availableFormats.size() == 1 && availableFormats[0].format == VK_FORMAT_UNDEFINED) 
			{
				return { VK_FORMAT_B8G8R8A8_UNORM, VK_COLOR_SPACE_SRGB_NONLINEAR_KHR };
			}

			for (const auto& availableFormat : availableFormats) 
			{
				if (availableFormat.format == VK_FORMAT_B8G8R8A8_UNORM && availableFormat.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR) 
				{
					return availableFormat;
				}
			}

			return availableFormats[0];
		}

		VkPresentModeKHR chooseSwapPresentMode(const std::vector<VkPresentModeKHR> availablePresentModes) 
		{
			VkPresentModeKHR bestMode = VK_PRESENT_MODE_FIFO_KHR;

			for (const auto& availablePresentMode : availablePresentModes) 
			{
				if (availablePresentMode == VK_PRESENT_MODE_MAILBOX_KHR) 
				{
					return availablePresentMode;
				}
				else if (availablePresentMode == VK_PRESENT_MODE_IMMEDIATE_KHR) 
				{
					bestMode = availablePresentMode;
				}
			}

			return bestMode;
		}

		VkExtent2D chooseSwapExtent(const VkSurfaceCapabilitiesKHR& capabilities) 
		{
			if (capabilities.currentExtent.width != std::numeric_limits<unsigned int>::max()) 
			{
				return capabilities.currentExtent;
			}
			else 
			{
				int width, height;
				glfwGetFramebufferSize(mWindow, &width, &height);

				VkExtent2D actualExtent =
				{
					static_cast<unsigned int>(width),
					static_cast<unsigned int>(height)
				};

				actualExtent.width = std::max(capabilities.minImageExtent.width, std::min(capabilities.maxImageExtent.width, actualExtent.width));
				actualExtent.height = std::max(capabilities.minImageExtent.height, std::min(capabilities.maxImageExtent.height, actualExtent.height));

				return actualExtent;
			}
		}

		VkShaderModule createShaderModule(const std::vector<char>& code)
		{
			VkShaderModuleCreateInfo createInfo = {};
			createInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
			createInfo.codeSize = code.size();
			createInfo.pCode = reinterpret_cast<const unsigned int*>(code.data());

			VkShaderModule shaderModule;
			if (vkCreateShaderModule(mDevice, &createInfo, nullptr, &shaderModule) != VK_SUCCESS)
			{
				throw std::runtime_error("Failed to create shader module");
			}

			return shaderModule;
		}

		unsigned int findMemoryType(unsigned int typeFilter, VkMemoryPropertyFlags properties) 
		{
			VkPhysicalDeviceMemoryProperties memProperties;
			vkGetPhysicalDeviceMemoryProperties(mPhysicalDevice, &memProperties);

			for (unsigned int i = 0; i < memProperties.memoryTypeCount; i++) 
			{
				if ((typeFilter & (1 << i)) && (memProperties.memoryTypes[i].propertyFlags & properties) == properties) 
				{
					return i;
				}
			}

			throw std::runtime_error("Failed to find suitable memory type");
		}

		void createBuffer(VkDeviceSize size, VkBufferUsageFlags usage, VkMemoryPropertyFlags properties, VkBuffer& buffer, VkDeviceMemory& bufferMemory)
		{
			VkBufferCreateInfo bufferInfo = {};
			bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
			bufferInfo.size = size;
			bufferInfo.usage = usage;
			bufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

			if (vkCreateBuffer(mDevice, &bufferInfo, nullptr, &buffer) != VK_SUCCESS) 
			{
				throw std::runtime_error("failed to create buffer!");
			}

			VkMemoryRequirements memRequirements;
			vkGetBufferMemoryRequirements(mDevice, buffer, &memRequirements);

			VkMemoryAllocateInfo allocInfo = {};
			allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
			allocInfo.allocationSize = memRequirements.size;
			allocInfo.memoryTypeIndex = findMemoryType(memRequirements.memoryTypeBits, properties);

			if (vkAllocateMemory(mDevice, &allocInfo, nullptr, &bufferMemory) != VK_SUCCESS) 
			{
				throw std::runtime_error("failed to allocate buffer memory!");
			}

			vkBindBufferMemory(mDevice, buffer, bufferMemory, 0);
		}

		void copyBuffer(VkBuffer srcBuffer, VkBuffer dstBuffer, VkDeviceSize size)
		{
			VkCommandBuffer commandBuffer = beginSingleTimeCommands();

			VkBufferCopy copyRegion = {};
			copyRegion.size = size;
			vkCmdCopyBuffer(commandBuffer, srcBuffer, dstBuffer, 1, &copyRegion);

			endSingleTimeCommands(commandBuffer);
		}

		void createImage(unsigned int width, unsigned int height, unsigned int mipLevels, VkSampleCountFlagBits numSamples, VkFormat format, VkImageTiling tiling, VkImageUsageFlags usage, VkMemoryPropertyFlags properties, VkImage& image, VkDeviceMemory& imageMemory)
		{
			VkImageCreateInfo imageInfo = {};
			imageInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
			imageInfo.imageType = VK_IMAGE_TYPE_2D;
			imageInfo.extent.width = width;
			imageInfo.extent.height = height;
			imageInfo.extent.depth = 1;
			imageInfo.mipLevels = mipLevels;
			imageInfo.arrayLayers = 1;
			imageInfo.format = format;
			imageInfo.tiling = tiling;
			imageInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
			imageInfo.usage = usage;
			imageInfo.samples = numSamples;
			imageInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

			if (vkCreateImage(mDevice, &imageInfo, nullptr, &image) != VK_SUCCESS) 
			{
				throw std::runtime_error("Failed to create image");
			}

			VkMemoryRequirements memRequirements;
			vkGetImageMemoryRequirements(mDevice, image, &memRequirements);

			VkMemoryAllocateInfo allocInfo = {};
			allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
			allocInfo.allocationSize = memRequirements.size;
			allocInfo.memoryTypeIndex = findMemoryType(memRequirements.memoryTypeBits, properties);

			if (vkAllocateMemory(mDevice, &allocInfo, nullptr, &imageMemory) != VK_SUCCESS) 
			{
				throw std::runtime_error("Failed to allocate image memory");
			}

			vkBindImageMemory(mDevice, image, imageMemory, 0);
		}

		void transitionImageLayout(VkImage image, VkFormat format, VkImageLayout oldLayout, VkImageLayout newLayout, unsigned int mipLevels) 
		{
			VkCommandBuffer commandBuffer = beginSingleTimeCommands();

			VkImageMemoryBarrier barrier = {};
			barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
			barrier.oldLayout = oldLayout;
			barrier.newLayout = newLayout;
			barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
			barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
			barrier.image = image;

			if (newLayout == VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL)
			{
				barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_DEPTH_BIT;
				if (hasStencilComponent(format))
				{
					barrier.subresourceRange.aspectMask |= VK_IMAGE_ASPECT_STENCIL_BIT;
				}
			}
			else
			{
				barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
			}

			barrier.subresourceRange.baseMipLevel = 0;
			barrier.subresourceRange.levelCount = mipLevels;
			barrier.subresourceRange.baseArrayLayer = 0;
			barrier.subresourceRange.layerCount = 1;

			VkPipelineStageFlags sourceStage;
			VkPipelineStageFlags destinationStage;

			if (oldLayout == VK_IMAGE_LAYOUT_UNDEFINED && newLayout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL) 
			{
				barrier.srcAccessMask = 0;
				barrier.dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;

				sourceStage = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
				destinationStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
			}
			else if (oldLayout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL && newLayout == VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL) 
			{
				barrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
				barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;

				sourceStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
				destinationStage = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
			}
			else if (oldLayout == VK_IMAGE_LAYOUT_UNDEFINED && newLayout == VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL) 
			{
				barrier.srcAccessMask = 0;
				barrier.dstAccessMask = VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_READ_BIT | VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;

				sourceStage = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
				destinationStage = VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
			}
			else if (oldLayout == VK_IMAGE_LAYOUT_UNDEFINED && newLayout == VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL) 
			{
				barrier.srcAccessMask = 0;
				barrier.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_READ_BIT | VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;

				sourceStage = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
				destinationStage = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
			}
			else 
			{
				throw std::invalid_argument("Unsupported layout transition");
			}

			vkCmdPipelineBarrier(
				commandBuffer,
				sourceStage, destinationStage,
				0,
				0, nullptr,
				0, nullptr,
				1, &barrier
			);

			endSingleTimeCommands(commandBuffer);
		}

		void copyBufferToImage(VkBuffer buffer, VkImage image, uint32_t width, uint32_t height) 
		{
			VkCommandBuffer commandBuffer = beginSingleTimeCommands();

			VkBufferImageCopy region = {};
			region.bufferOffset = 0;
			region.bufferRowLength = 0;
			region.bufferImageHeight = 0;
			region.imageSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
			region.imageSubresource.mipLevel = 0;
			region.imageSubresource.baseArrayLayer = 0;
			region.imageSubresource.layerCount = 1;
			region.imageOffset = { 0, 0, 0 };
			region.imageExtent = { width, height, 1 };

			vkCmdCopyBufferToImage(commandBuffer, buffer, image, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 1, &region);

			endSingleTimeCommands(commandBuffer);
		}

		VkImageView createImageView(VkImage image, VkFormat format, VkImageAspectFlags aspectFlags, unsigned int mipLevels)
		{
			VkImageViewCreateInfo viewInfo = {};
			viewInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
			viewInfo.image = image;
			viewInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
			viewInfo.format = format;
			viewInfo.subresourceRange.aspectMask = aspectFlags;
			viewInfo.subresourceRange.baseMipLevel = 0;
			viewInfo.subresourceRange.levelCount = mipLevels;
			viewInfo.subresourceRange.baseArrayLayer = 0;
			viewInfo.subresourceRange.layerCount = 1;

			VkImageView imageView;
			if (vkCreateImageView(mDevice, &viewInfo, nullptr, &imageView) != VK_SUCCESS) 
			{
				throw std::runtime_error("Failed to create texture image view");
			}

			return imageView;
		}

		VkCommandBuffer beginSingleTimeCommands() 
		{
			VkCommandBufferAllocateInfo allocInfo = {};
			allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
			allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
			allocInfo.commandPool = mCommandPool;
			allocInfo.commandBufferCount = 1;

			VkCommandBuffer commandBuffer;
			vkAllocateCommandBuffers(mDevice, &allocInfo, &commandBuffer);

			VkCommandBufferBeginInfo beginInfo = {};
			beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
			beginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;

			vkBeginCommandBuffer(commandBuffer, &beginInfo);

			return commandBuffer;
		}

		void endSingleTimeCommands(VkCommandBuffer commandBuffer) 
		{
			vkEndCommandBuffer(commandBuffer);

			VkSubmitInfo submitInfo = {};
			submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
			submitInfo.commandBufferCount = 1;
			submitInfo.pCommandBuffers = &commandBuffer;

			vkQueueSubmit(mGraphicsQueue, 1, &submitInfo, VK_NULL_HANDLE);
			vkQueueWaitIdle(mGraphicsQueue);

			vkFreeCommandBuffers(mDevice, mCommandPool, 1, &commandBuffer);
		}

		void generateMipmaps(VkImage image, VkFormat imageFormat, unsigned int texWidth, unsigned int texHeight, unsigned int mipLevels)
		{
			// Check if image format supports linear blitting
			VkFormatProperties formatProperties;
			vkGetPhysicalDeviceFormatProperties(mPhysicalDevice, imageFormat, &formatProperties);

			if (!(formatProperties.optimalTilingFeatures & VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_LINEAR_BIT)) 
			{
				throw std::runtime_error("Texture image format does not support linear blitting");
			}

			VkCommandBuffer commandBuffer = beginSingleTimeCommands();

			VkImageMemoryBarrier barrier = {};
			barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
			barrier.image = image;
			barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
			barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
			barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
			barrier.subresourceRange.baseArrayLayer = 0;
			barrier.subresourceRange.layerCount = 1;
			barrier.subresourceRange.levelCount = 1;

			int mipWidth = texWidth;
			int mipHeight = texHeight;

			for (unsigned int i = 1; i < mipLevels; i++) 
			{
				barrier.subresourceRange.baseMipLevel = i - 1;
				barrier.oldLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
				barrier.newLayout = VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL;
				barrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
				barrier.dstAccessMask = VK_ACCESS_TRANSFER_READ_BIT;

				vkCmdPipelineBarrier(commandBuffer,
					VK_PIPELINE_STAGE_TRANSFER_BIT, VK_PIPELINE_STAGE_TRANSFER_BIT, 0,
					0, nullptr,
					0, nullptr,
					1, &barrier);

				VkImageBlit blit = {};
				blit.srcOffsets[0] = { 0, 0, 0 };
				blit.srcOffsets[1] = { mipWidth, mipHeight, 1 };
				blit.srcSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
				blit.srcSubresource.mipLevel = i - 1;
				blit.srcSubresource.baseArrayLayer = 0;
				blit.srcSubresource.layerCount = 1;
				blit.dstOffsets[0] = { 0, 0, 0 };
				blit.dstOffsets[1] = { mipWidth > 1 ? mipWidth / 2 : 1, mipHeight > 1 ? mipHeight / 2 : 1, 1 };
				blit.dstSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
				blit.dstSubresource.mipLevel = i;
				blit.dstSubresource.baseArrayLayer = 0;
				blit.dstSubresource.layerCount = 1;

				vkCmdBlitImage(commandBuffer,
					image, VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL,
					image, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
					1, &blit,
					VK_FILTER_LINEAR);

				barrier.oldLayout = VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL;
				barrier.newLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
				barrier.srcAccessMask = VK_ACCESS_TRANSFER_READ_BIT;
				barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;

				vkCmdPipelineBarrier(commandBuffer,
					VK_PIPELINE_STAGE_TRANSFER_BIT, VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT, 0,
					0, nullptr,
					0, nullptr,
					1, &barrier);

				if (mipWidth > 1) mipWidth /= 2;
				if (mipHeight > 1) mipHeight /= 2;
			}

			barrier.subresourceRange.baseMipLevel = mipLevels - 1;
			barrier.oldLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
			barrier.newLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
			barrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
			barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;

			vkCmdPipelineBarrier(commandBuffer,
				VK_PIPELINE_STAGE_TRANSFER_BIT, VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT, 0,
				0, nullptr,
				0, nullptr,
				1, &barrier);

			endSingleTimeCommands(commandBuffer);
		}

		VkFormat findSupportedFormat(const std::vector<VkFormat>& candidates, VkImageTiling tiling, VkFormatFeatureFlags features) 
		{
			for (VkFormat format : candidates) 
			{
				VkFormatProperties props;
				vkGetPhysicalDeviceFormatProperties(mPhysicalDevice, format, &props);

				if (tiling == VK_IMAGE_TILING_LINEAR && (props.linearTilingFeatures & features) == features) 
				{
					return format;
				}
				else if (tiling == VK_IMAGE_TILING_OPTIMAL && (props.optimalTilingFeatures & features) == features) 
				{
					return format;
				}
			}

			throw std::runtime_error("Failed to find supported format");
		}

		VkFormat findDepthFormat() 
		{
			return findSupportedFormat({ VK_FORMAT_D32_SFLOAT, VK_FORMAT_D32_SFLOAT_S8_UINT, VK_FORMAT_D24_UNORM_S8_UINT }, VK_IMAGE_TILING_OPTIMAL, VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT);
		}

		bool hasStencilComponent(VkFormat format) 
		{
			return format == VK_FORMAT_D32_SFLOAT_S8_UINT || format == VK_FORMAT_D24_UNORM_S8_UINT;
		}

		VkSampleCountFlagBits getMaxUsableSampleCount() 
		{
			VkPhysicalDeviceProperties physicalDeviceProperties;
			vkGetPhysicalDeviceProperties(mPhysicalDevice, &physicalDeviceProperties);

			VkSampleCountFlags counts = std::min(physicalDeviceProperties.limits.framebufferColorSampleCounts, physicalDeviceProperties.limits.framebufferDepthSampleCounts);
			if (counts & VK_SAMPLE_COUNT_64_BIT) { return VK_SAMPLE_COUNT_64_BIT; }
			if (counts & VK_SAMPLE_COUNT_32_BIT) { return VK_SAMPLE_COUNT_32_BIT; }
			if (counts & VK_SAMPLE_COUNT_16_BIT) { return VK_SAMPLE_COUNT_16_BIT; }
			if (counts & VK_SAMPLE_COUNT_8_BIT) { return VK_SAMPLE_COUNT_8_BIT; }
			if (counts & VK_SAMPLE_COUNT_4_BIT) { return VK_SAMPLE_COUNT_4_BIT; }
			if (counts & VK_SAMPLE_COUNT_2_BIT) { return VK_SAMPLE_COUNT_2_BIT; }

			return VK_SAMPLE_COUNT_1_BIT;
		}

		static VKAPI_ATTR VkBool32 VKAPI_CALL debugCallback(VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity, VkDebugUtilsMessageTypeFlagsEXT messageType, const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData, void* pUserData) 
		{
			printf("Validation layer: %s\n", pCallbackData->pMessage);
			return VK_FALSE;
		}

		static void framebufferResizeCallback(GLFWwindow* window, int width, int height) 
		{
			auto app = reinterpret_cast<HelloTriangleApplication*>(glfwGetWindowUserPointer(window));
			app->mFramebufferResized = true;
		}

	private:
		GLFWwindow* mWindow;
		VkInstance mInstance;
		VkDebugUtilsMessengerEXT mCallback;
		VkSurfaceKHR mSurface;
		VkPhysicalDevice mPhysicalDevice = VK_NULL_HANDLE;
		VkDevice mDevice;
		VkQueue mGraphicsQueue;
		VkQueue mPresentQueue; 
		//VmaAllocator mAllocator;
		VkSwapchainKHR mSwapChain;
		VkFormat mSwapChainImageFormat;
		VkExtent2D mSwapChainExtent;
		std::vector<VkImage> mSwapChainImages; 
		std::vector<VkImageView> mSwapChainImageViews;
		VkRenderPass mRenderPass;
		VkDescriptorSetLayout mDescriptorSetLayout;
		VkPipelineLayout mPipelineLayout;
		VkPipeline mGraphicsPipeline;
		VkSampleCountFlagBits mMsaaSamples = VK_SAMPLE_COUNT_1_BIT;
		VkImage mColorImage;
		VkDeviceMemory mColorImageMemory;
		VkImageView mColorImageView;
		std::vector<VkFramebuffer> mSwapChainFramebuffers;
		VkCommandPool mCommandPool;
		VkImage mDepthImage;
		VkDeviceMemory mDepthImageMemory;
		VkImageView mDepthImageView;
		unsigned int mMipLevels;
		VkImage mTextureImage;
		VkDeviceMemory mTextureImageMemory;
		VkImageView mTextureImageView;
		VkSampler mTextureSampler;
		std::vector<Vertex> mVertices;
		std::vector<unsigned int> mIndices;
		VkBuffer mVertexBuffer;
		VkDeviceMemory mVertexBufferMemory; 
		VkBuffer mIndexBuffer;
		VkDeviceMemory mIndexBufferMemory;
		std::vector<VkBuffer> mUniformBuffers;
		std::vector<VkDeviceMemory> mUniformBuffersMemory;
		VkDescriptorPool mDescriptorPool;
		std::vector<VkDescriptorSet> mDescriptorSets;
		std::vector<VkCommandBuffer> mCommandBuffers;
		std::vector<VkSemaphore> mImageAvailableSemaphores;
		std::vector<VkSemaphore> mRenderFinishedSemaphores; 
		std::vector<VkFence> mInFlightFences;
		unsigned int mCurrentFrame = 0;
		bool mFramebufferResized = false;
};

int main() 
{
	try 
	{
		HelloTriangleApplication app(winWidth, winHeight, "Vulkan");
		app.run();
	}
	catch (const std::exception& e) 
	{
		std::cerr << e.what() << std::endl;
		system("pause");
		return EXIT_FAILURE;
	}
	return EXIT_SUCCESS;
}