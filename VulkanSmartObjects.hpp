#ifndef NU_VULKAN_SMART_OBJECTS_HPP
#define NU_VULKAN_SMART_OBJECTS_HPP

#include <functional>
#include "VulkanFunctions.hpp"

namespace nu 
{

#define NU_VK_SMART_OBJECT(VkName, Name, vkDestroyName)                                           \
	class VkSmart##Name                                                                           \
	{                                                                                             \
		public:                                                                                   \
			VkSmart##Name()                                                                       \
			{                                                                                     \
				mHandle = VK_NULL_HANDLE;                                                         \
				mDeviceHandle = VK_NULL_HANDLE;                                                   \
			}                                                                                     \
																								  \
			VkSmart##Name(VkDevice device)                                                        \
			{                                                                                     \
				mHandle = VK_NULL_HANDLE;                                                         \
				mDeviceHandle = device;                                                           \
			}                                                                                     \
                                                                                                  \
			VkSmart##Name(##VkName handle, VkDevice device)                                       \
			{                                                                                     \
				mHandle = handle;                                                                 \
				mDeviceHandle = device;                                                           \
			}                                                                                     \
                                                                                                  \
			~VkSmart##Name()                                                                      \
			{                                                                                     \
				if (mHandle != VK_NULL_HANDLE)                                                    \
				{                                                                                 \
					if (mDeviceHandle != VK_NULL_HANDLE)                                          \
					{                                                                             \
						/*##vkDestroyName(mDeviceHandle, mHandle, nullptr);*/                     \
						mHandle = VK_NULL_HANDLE;                                                 \
						mDeviceHandle = VK_NULL_HANDLE;                                           \
					}                                                                             \
					else                                                                          \
					{                                                                             \
						printf("VkSmart%s is not initialized, so it couldn't be destroyed correctly\n", #Name); \
					}                                                                             \
				}                                                                                 \
			}                                                                                     \
                                                                                                  \
			VkSmart##Name(VkSmart ##Name && other)                                                \
			{                                                                                     \
				mHandle = other.mHandle;                                                          \
				mDeviceHandle = other.mDeviceHandle;                                              \
                                                                                                  \
                other.mHandle = VK_NULL_HANDLE;                                                   \
                other.mDeviceHandle = VK_NULL_HANDLE;                                             \
			}                                                                                     \
			                                                                                      \
	        VkSmart##Name& operator=(VkSmart##Name&& other)                                       \
			{                                                                                     \
				if (this != &other)                                                               \
				{                                                                                 \
					VkName handle = mHandle;                                                      \
					VkDevice deviceHandle = mDeviceHandle;                                        \
                                                                                                  \
					mHandle = other.mHandle;                                                      \
					mDeviceHandle = other.mDeviceHandle;                                          \
                                                                                                  \
					other.mHandle = handle;                                                       \
					other.mDeviceHandle = deviceHandle;                                           \
				}                                                                                 \
				return *this;                                                                     \
			}                                                                                     \
                                                                                                  \
			VkName& getHandle()                                                                   \
			{                                                                                     \
				return mHandle;                                                                   \
			}                                                                                     \
                                                                                                  \
			const VkName& getHandle() const                                                       \
			{                                                                                     \
				return mHandle;                                                                   \
			}                                                                                     \
                                                                                                  \
			VkDevice& getDeviceHandle()                                                           \
			{                                                                                     \
				return mDeviceHandle;                                                             \
			}                                                                                     \
                                                                                                  \
			const VkDevice& getDeviceHandle() const                                               \
			{                                                                                     \
				return mDeviceHandle;                                                             \
			}                                                                                     \
                                                                                                  \
			VkName& operator*()                                                                   \
			{                                                                                     \
				return mHandle;                                                                   \
			}                                                                                     \
                                                                                                  \
			const VkName& operator*() const                                                       \
			{                                                                                     \
				return mHandle;                                                                   \
			}                                                                                     \
                                                                                                  \
			bool operator!() const                                                                \
			{                                                                                     \
				return mHandle == VK_NULL_HANDLE;                                                 \
			}                                                                                     \
					                                                                              \
			operator bool() const                                                                 \
			{                                                                                     \
				return mHandle != VK_NULL_HANDLE;                                                 \
			}                                                                                     \
                                                                                                  \
			bool isValidHandle() const                                                            \
			{                                                                                     \
				return mHandle != VK_NULL_HANDLE;                                                 \
			}                                                                                     \
                                                                                                  \
			bool isValidDeviceHandle() const                                                      \
			{                                                                                     \
				return mDeviceHandle != VK_NULL_HANDLE;                                           \
            }                                                                                     \
                                                                                                  \
            VkSmart##Name(const VkSmart ##Name &) = delete;                                       \
			VkSmart##Name& operator=(const VkSmart ##Name &) = delete;                            \
                                                                                                  \
		private:                                                                                  \
			VkName mHandle;                                                                       \
			VkDevice mDeviceHandle;                                                               \
	};


NU_VK_SMART_OBJECT(VkSemaphore, Semaphore, vkDestroySemaphore);
// NU_VK_SMART_OBJECT(VkCommandBuffer, CommandBuffer, vkFreeCommandBuffers); <- command buffers are freed along with the pool
NU_VK_SMART_OBJECT(VkFence, Fence, vkDestroyFence);
NU_VK_SMART_OBJECT(VkDeviceMemory, DeviceMemory, vkFreeMemory);
NU_VK_SMART_OBJECT(VkBuffer, Buffer, vkDestroyBuffer);
NU_VK_SMART_OBJECT(VkImage, Image, vkDestroyImage);
NU_VK_SMART_OBJECT(VkEvent, Event, vkDestroyEvent);
NU_VK_SMART_OBJECT(VkQueryPool, QueryPool, vkDestroyQueryPool);
NU_VK_SMART_OBJECT(VkBufferView, BufferView, vkDestroyBufferView);
NU_VK_SMART_OBJECT(VkImageView, ImageView, vkDestroyImageView);
NU_VK_SMART_OBJECT(VkShaderModule, ShaderModule, vkDestroyShaderModule);
NU_VK_SMART_OBJECT(VkPipelineCache, PipelineCache, vkDestroyPipelineCache);
NU_VK_SMART_OBJECT(VkPipelineLayout, PipelineLayout, vkDestroyPipelineLayout);
NU_VK_SMART_OBJECT(VkRenderPass, RenderPass, vkDestroyRenderPass);
NU_VK_SMART_OBJECT(VkPipeline, Pipeline, vkDestroyPipeline);
NU_VK_SMART_OBJECT(VkDescriptorSetLayout, DescriptorSetLayout, vkDestroyDescriptorSetLayout);
NU_VK_SMART_OBJECT(VkSampler, Sampler, vkDestroySampler);
NU_VK_SMART_OBJECT(VkDescriptorPool, DescriptorPool, vkDestroyDescriptorPool);
// NU_VK_SMART_OBJECT(VkDescriptorSet, DescriptorSet, vkFreeDescriptorSets); <- descriptor sets are freed along with the pool
NU_VK_SMART_OBJECT(VkFramebuffer, Framebuffer, vkDestroyFramebuffer);
NU_VK_SMART_OBJECT(VkCommandPool, CommandPool, vkDestroyCommandPool);
NU_VK_SMART_OBJECT(VkSwapchainKHR, SwapchainKHR, vkDestroySwapchainKHR);

class VkSmartInstance
{
	public: 
		VkSmartInstance()
		{
			mHandle = VK_NULL_HANDLE;
		}

		VkSmartInstance(VkInstance instance)
		{
			mHandle = instance;
		}

		~VkSmartInstance()
		{
			if (mHandle != VK_NULL_HANDLE)
			{
				//vkDestroyInstance(mHandle, nullptr);
				mHandle = VK_NULL_HANDLE;
			}
		} 

		VkSmartInstance(VkSmartInstance&& other) 
		{  
			mHandle = other.mHandle;      
			other.mHandle = VK_NULL_HANDLE;
		}

		VkSmartInstance& operator=(VkSmartInstance&& other)
		{
			if (this != &other)
			{
				VkInstance handle = mHandle;

				mHandle = other.mHandle;

				other.mHandle = handle;
			}
			return *this;
		}

		VkInstance& getHandle()
		{
			return mHandle;
		}

		const VkInstance& getHandle() const
		{
			return mHandle;
		}

		VkInstance& operator*() 
		{
			return mHandle;
		}

		const VkInstance& operator*() const
		{
			return mHandle;
		}

		bool operator!() const
		{
			return mHandle == VK_NULL_HANDLE;
		}

		operator bool() const
		{
			return mHandle != VK_NULL_HANDLE;
		}

		bool isValidHandle() const
		{
			return mHandle != VK_NULL_HANDLE;
		}

		VkSmartInstance(const VkSmartInstance&) = delete;
		VkSmartInstance& operator=(const VkSmartInstance&) = delete;

	private:
		VkInstance mHandle;
};

class VkSmartDevice
{
	public:
		VkSmartDevice()
		{
			mHandle = VK_NULL_HANDLE;
		}

		VkSmartDevice(VkDevice device)
		{
			mHandle = device;
		}

		~VkSmartDevice()
		{
			if (mHandle != VK_NULL_HANDLE)
			{
				//vkDestroyDevice(mHandle, nullptr);
				mHandle = VK_NULL_HANDLE;
			}
		}

		VkSmartDevice(VkSmartDevice&& other)
		{
			mHandle = other.mHandle;
			other.mHandle = VK_NULL_HANDLE;
		}

		VkSmartDevice& operator=(VkSmartDevice&& other)
		{
			if (this != &other)
			{
				VkDevice handle = mHandle;

				mHandle = other.mHandle;

				other.mHandle = handle;
			}
			return *this;
		}

		VkDevice& getHandle()
		{
			return mHandle;
		}

		const VkDevice& getHandle() const
		{
			return mHandle;
		}

		VkDevice& operator*()
		{
			return mHandle;
		}

		const VkDevice& operator*() const
		{
			return mHandle;
		}

		bool operator!() const
		{
			return mHandle == VK_NULL_HANDLE;
		}

		operator bool() const
		{
			return mHandle != VK_NULL_HANDLE;
		}

		bool isValidHandle() const
		{
			return mHandle != VK_NULL_HANDLE;
		}

		VkSmartDevice(const VkSmartDevice&) = delete;
		VkSmartDevice& operator=(const VkSmartDevice&) = delete;

	private:
		VkDevice mHandle;
};


class VkSmartSurfaceKHR
{
	public:
		VkSmartSurfaceKHR()
		{
			mHandle = VK_NULL_HANDLE;
			mInstanceHandle = VK_NULL_HANDLE;
		}

		VkSmartSurfaceKHR(VkInstance instance)
		{
			mHandle = VK_NULL_HANDLE;
			mInstanceHandle = instance;
		}

		VkSmartSurfaceKHR(VkSurfaceKHR handle, VkInstance instance)
		{
			mHandle = handle;
			mInstanceHandle = instance;
		}

		~VkSmartSurfaceKHR()
		{
			if (mHandle != VK_NULL_HANDLE)
			{
				if (mInstanceHandle != VK_NULL_HANDLE)
				{
					//vkDestroySurfaceKHR(mInstanceHandle, mHandle, nullptr);
					mHandle = VK_NULL_HANDLE;
					mInstanceHandle = VK_NULL_HANDLE;
				}
				else
				{
					printf("VkSmartSurfaceKHR is not initialized, so it couldn't be destroyed correctly\n");
				}
			}
		}

		VkSmartSurfaceKHR(VkSmartSurfaceKHR&& other)
		{
			mHandle = other.mHandle;
			mInstanceHandle = other.mInstanceHandle;

            other.mHandle = VK_NULL_HANDLE;
            other.mInstanceHandle = VK_NULL_HANDLE;
		}

	    VkSmartSurfaceKHR& operator=(VkSmartSurfaceKHR&& other)
		{
			if (this != &other)
			{
				VkSurfaceKHR handle = mHandle;
				VkInstance instanceHandle = mInstanceHandle;

				mHandle = other.mHandle;
				mInstanceHandle = other.mInstanceHandle;

				other.mHandle = handle;
				other.mInstanceHandle = instanceHandle;
			}
			return *this;
		}
		
		void initialize(VkInstance instance)
		{
			mInstanceHandle = instance;
		}

		void initialize(const VkSmartInstance& instance)
		{
			mInstanceHandle = instance.getHandle();
		}

		bool isInitialized() const
		{
			return mInstanceHandle != VK_NULL_HANDLE;
		}

		VkSurfaceKHR& getHandle()
		{
			return mHandle;
		}

		const VkSurfaceKHR& getHandle() const 
		{
			return mHandle;
		}

		const VkInstance& getInstanceHandle() const
		{
			return mInstanceHandle;
		}

		VkSurfaceKHR& operator*()
		{
			return mHandle;
		}

		const VkSurfaceKHR& operator*() const
		{
			return mHandle;
		}

		bool operator!() const
		{
			return mHandle == VK_NULL_HANDLE;
		}

		operator bool() const
		{
			return mHandle != VK_NULL_HANDLE;
		}

		bool isValidHandle() const
		{
			return mHandle != VK_NULL_HANDLE;
		}

		bool isValidInstanceHandle() const
		{
			return mInstanceHandle != VK_NULL_HANDLE;
        }

        VkSmartSurfaceKHR(const VkSmartSurfaceKHR&) = delete;
		VkSmartSurfaceKHR& operator=(const VkSmartSurfaceKHR&) = delete;

	private:
		VkSurfaceKHR mHandle;
		VkInstance mInstanceHandle;
};

} // namespace nu

#endif // NU_VULKAN_SMART_OBJECTS_HPP