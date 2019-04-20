#pragma once

#include "VulkanConfig.hpp"
	
/////////////////////////////////////
// VULKAN_NAMESPACE
/////////////////////////////////////
#if (defined VULKAN_NAMESPACE_NAME)
	#define VULKAN_NAMESPACE_BEGIN namespace VULKAN_NAMESPACE_NAME {
	#define VULKAN_NAMESPACE_END }
#else
	#define VULKAN_NAMESPACE_BEGIN
	#define VULKAN_NAMESPACE_END
#endif

///////////////////////////////////////////
// VULKAN_LINKING
///////////////////////////////////////////
#if (defined VULKAN_LINKING_DYNAMIC && defined VULKAN_LINKING_STATIC)
	#undef VULKAN_LINKING_STATIC
#endif
#if (!defined VULKAN_LINKING_DYNAMIC && !defined VULKAN_LINKING_STATIC)
	#define VULKAN_LINKING_DYNAMIC
#endif
#if (defined VULKAN_LINKING_DYNAMIC)
	#define VK_NO_PROTOTYPES
#else
	#undef VK_NO_PROTOTYPES
#endif

///////////////////////////////////////////
// VULKAN_BUILD
///////////////////////////////////////////
#if (defined VULKAN_BUILD_DEBUG && (defined VULKAN_BUILD_RELEASE || defined VULKAN_BUILD_PROFILE || defined VULKAN_BUILD_FINAL) || (defined VULKAN_BUILD_RELEASE && (defined VULKAN_BUILD_PROFILE || defined VULKAN_BUILD_FINAL)) || (defined VULKAN_BUILD_PROFILE && defined VULKAN_BUILD_FINAL))
	#undef VULKAN_BUILD_RELEASE
	#undef VULKAN_BUILD_PROFILE
	#undef VULKAN_BUILD_FINAL
	#warning Too many build provided : using VULKAN_BUILD_DEBUG
#endif 
#if (!defined VULKAN_BUILD_DEBUG && !defined VULKAN_BUILD_RELEASE && !defined VULKAN_BUILD_PROFILE && !defined VULKAN_BUILD_FINAL)
	#define VULKAN_BUILD_DEBUG
	#warning No build provided : using VULKAN_BUILD_DEBUG
#endif

///////////////////////////////////////////
// VULKAN_OPTION
///////////////////////////////////////////
#if (!defined VULKAN_OPTION_OBJECTNAMES && (defined VULKAN_BUILD_DEBUG || defined VULKAN_BUILD_RELEASE))
	#define VULKAN_OPTION_OBJECTNAMES
#endif
#if (defined VULKAN_OPTION_NO_OBJECTNAMES)
	#undef VULKAN_OPTION_OBJECTNAMES
#endif

#if (!defined VULKAN_OPTION_OBJECTTRACKER && (defined VULKAN_BUILD_DEBUG || defined VULKAN_BUILD_RELEASE))
	#define VULKAN_OPTION_OBJECTTRACKER
#endif
#if (defined VULKAN_OPTION_NO_OBJECTTRACKER)
	#undef VULKAN_OPTION_OBJECTTRACKER
#endif

#if (!defined VULKAN_OPTION_ASSERT && (defined VULKAN_BUILD_DEBUG || defined VULKAN_BUILD_RELEASE))
	#define VULKAN_OPTION_ASSERT
#endif
#if (defined VULKAN_OPTION_NO_ASSERT)
	#undef VULKAN_OPTION_ASSERT
#endif
#if (defined VULKAN_OPTION_ASSERT && !defined VULKAN_ASSERT)
	#include <cassert>
	// TODO : Use ...
	#define VULKAN_ASSERT(x, ...) assert((x)); 
#endif

// TODO : Log with VA_ARGS
#if (!defined VULKAN_OPTION_LOG && (defined VULKAN_BUILD_DEBUG || defined VULKAN_BUILD_RELEASE))
	#define VULKAN_OPTION_LOG
#endif
#if (defined VULKAN_OPTION_NO_LOG)
	#undef VULKAN_OPTION_LOG
#endif
#if (defined VULKAN_OPTION_LOG)
	#if (!defined VULKAN_LOG_INFO)
		#define VULKAN_LOG_INFO(x, ...) VulkanPriv::printLog("INFO", (x), __VA_ARGS__);
	#endif
	#if (!defined VULKAN_LOG_WARNING)
		#define VULKAN_LOG_WARNING(x, ...) VulkanPriv::printLog("WARNING", (x), __VA_ARGS__);
	#endif
	#if (!defined VULKAN_LOG_ERROR) 
		#define VULKAN_LOG_ERROR(x, ...) VulkanPriv::printLog("ERROR", (x), __VA_ARGS__);
	#endif
#else
	#if (defined VULKAN_LOG_INFO)
		#undef VULKAN_LOG_INFO
		#define VULKAN_LOG_INFO(x, ...) ((void)0)
	#endif
	#if (defined VULKAN_LOG_WARNING)
		#undef VULKAN_LOG_WARNING
		#define VULKAN_LOG_WARNING(x, ...) ((void)0)
	#endif
	#if (defined VULKAN_LOG_ERROR)
		#undef VULKAN_LOG_ERROR
		#define VULKAN_LOG_ERROR(x, ...) ((void)0)
	#endif
#endif

#if (!defined VULKAN_OPTION_PROFILE && (defined VULKAN_BUILD_PROFILE))
	#define VULKAN_OPTION_PROFILE
#endif
#if (defined VULKAN_OPTION_NO_PROFILE)
	#undef VULKAN_OPTION_PROFILE
#endif
// TODO : Implement profiling

///////////////////////////////////////////
// VULKAN_PLATFORM
///////////////////////////////////////////
#if (!defined VULKAN_PLATFORM_WINDOWS && (defined _WIN32 || defined __WIN32__ || defined _WIN64)) 
	#define VULKAN_PLATFORM_WINDOWS
#endif
// TODO : Linux & Android
// TODO : zero platform defined test
// TODO : too many platforms defined test

///////////////////////////////////////////
// VULKAN_KHR
///////////////////////////////////////////
// TODO : Improve PlatformConnection/Window Types (pointer/not pointer)
// TODO : Do this for Linux XCB and XLIB
#if (defined VULKAN_KHR)
	#if (defined VULKAN_PLATFORM_WINDOWS)
		#define VK_USE_PLATFORM_WIN32_KHR
		#define VulkanPlatformConnectionType HINSTANCE
		#define VulkanPlatformWindowType HWND
	#endif
	// TODO : Others KHR
	// Mir
	// Wayland
	// Android
	// Xcb
	// Xlib
	// TODO : Don't forget to update Surface/Swapchain accordingly
#endif

/*
#if (defined VULKAN_OPTION_KHR && defined VULKAN_PLATFORM_WINDOWS)
	#define PlatformConnectionType HINSTANCE
	#define PlatformWindowType HWND
#elif (defined VULKAN_OPTION_KHR && defined VULKAN_PLATFORM_LINUX)
	#if (defined VULKAN_LINUX_KHR_XLIB) // VK_USE_PLATFORM_XLIB_KHR
		#define PlatformConnectionType Display*
		#define PlatformWindowType ::Window
	#elif defined VULKAN_LINUX_KHR_XCB // VK_USE_PLATFORM_XCB_KHR
		#define PlatformConnectionType xcb_connection_t*
		#define PlatformWindowType xcb_window_t
	#endif
#endif
*/

///////////////////////////////////////////
// VULKAN_ENDIANNESS
///////////////////////////////////////////
#if defined(__m68k__) || defined(mc68000) || defined(_M_M68K) || (defined(__MIPS__) && defined(__MISPEB__)) || \
		defined(__ppc__) || defined(__POWERPC__) || defined(_M_PPC) || defined(__sparc__) || defined(__hppa__)
	#define VULKAN_ENDIAN_BIG
#elif defined(__i386__) || defined(__i386) || defined(__X86__) || defined (__x86_64) || defined(_M_I86) || \
		defined(_M_IX86) || defined(_M_X64)
	#define VULKAN_ENDIAN_LITTLE
#else
	#error Failed to identify endianness
#endif

/////////////////////////////////////
// VULKAN_TYPES
/////////////////////////////////////
#include "../ThirdParty/vulkan/vulkan.h"

// TODO : Some includes
#include <cstdint> // C++11 types
#include <memory>
#include <vector>

VULKAN_NAMESPACE_BEGIN

using VulkanU8 = std::uint_least8_t;
using VulkanU16 = std::uint_least16_t;
using VulkanU32 = std::uint_least32_t;
using VulkanU64 = std::uint_least64_t;
using VulkanI8 = std::int_least8_t;
using VulkanI16 = std::int_least16_t;
using VulkanI32 = std::int_least32_t;
using VulkanI64 = std::int_least64_t;
using VulkanF32 = float;
using VulkanF64 = double;
using VulkanBool = bool;
const VulkanU8 VulkanInvalidU8 = -1;
const VulkanU16 VulkanInvalidU16 = -1;
const VulkanU32 VulkanInvalidU32 = -1;
const VulkanU64 VulkanInvalidU64 = -1;
static_assert(sizeof(VulkanU8) == 1, "U8 != 8");
static_assert(sizeof(VulkanU16) == 2, "U16 != 16");
static_assert(sizeof(VulkanU32) == 4, "U32 != 32");
static_assert(sizeof(VulkanU64) == 8, "U64 != 64");
static_assert(sizeof(VulkanI8) == 1, "I8 != 8");
static_assert(sizeof(VulkanI16) == 2, "I16 != 16");
static_assert(sizeof(VulkanI32) == 4, "I32 != 32");
static_assert(sizeof(VulkanI64) == 8, "I64 != 64");
static_assert(sizeof(VulkanF32) == 4, "F32 != 32");
static_assert(sizeof(VulkanF64) == 8, "F64 != 64");

using VulkanDeviceId = VulkanU8;

struct VulkanWindowParameters
{
	VulkanPlatformConnectionType platformConnection;
	VulkanPlatformWindowType platformWindow;
};

namespace VulkanPriv
{
	void printLog(const char* logType, const char* str, ...);
}

// TODO : Create our own unique/shared/weak/... ptr for precise usages
// TODO : Or like Reference<T> and/or Handle<T>
// TODO : Ex for Pooled type (CmdBuffer, DescSet)
#define VULKAN_UNIQUE_PTR_DECLARATION(Type) typedef std::unique_ptr<Type> Type##Ptr;
// TODO : ForwardDeclarations
// TODO : How to deal with enum ? (Like VulkanCommandBufferType)

class VulkanBuffer; VULKAN_UNIQUE_PTR_DECLARATION(VulkanBuffer);
class VulkanBufferView; VULKAN_UNIQUE_PTR_DECLARATION(VulkanBufferView);
class VulkanCommandBuffer; VULKAN_UNIQUE_PTR_DECLARATION(VulkanCommandBuffer);
class VulkanCommandPool; VULKAN_UNIQUE_PTR_DECLARATION(VulkanCommandPool);
class VulkanComputePipeline; VULKAN_UNIQUE_PTR_DECLARATION(VulkanComputePipeline);
class VulkanDescriptorPool; VULKAN_UNIQUE_PTR_DECLARATION(VulkanDescriptorPool);
class VulkanDescriptorSet; VULKAN_UNIQUE_PTR_DECLARATION(VulkanDescriptorSet);
class VulkanDescriptorSetLayout; VULKAN_UNIQUE_PTR_DECLARATION(VulkanDescriptorSetLayout);
class VulkanDevice; VULKAN_UNIQUE_PTR_DECLARATION(VulkanDevice);
class VulkanFence; VULKAN_UNIQUE_PTR_DECLARATION(VulkanFence);
class VulkanFramebuffer; VULKAN_UNIQUE_PTR_DECLARATION(VulkanFramebuffer);
class VulkanGraphicsPipeline; VULKAN_UNIQUE_PTR_DECLARATION(VulkanGraphicsPipeline);
class VulkanImage; VULKAN_UNIQUE_PTR_DECLARATION(VulkanImage);
class VulkanImageHelper; VULKAN_UNIQUE_PTR_DECLARATION(VulkanImageHelper);
class VulkanImageView; VULKAN_UNIQUE_PTR_DECLARATION(VulkanImageView);
class VulkanInstance; VULKAN_UNIQUE_PTR_DECLARATION(VulkanInstance);
class VulkanMemoryBlock; VULKAN_UNIQUE_PTR_DECLARATION(VulkanMemoryBlock);
class VulkanPhysicalDevice; VULKAN_UNIQUE_PTR_DECLARATION(VulkanPhysicalDevice);
class VulkanPipelineCache; VULKAN_UNIQUE_PTR_DECLARATION(VulkanPipelineCache);
class VulkanPipelineLayout; VULKAN_UNIQUE_PTR_DECLARATION(VulkanPipelineLayout);
class VulkanQueue; VULKAN_UNIQUE_PTR_DECLARATION(VulkanQueue);
class VulkanRenderPass; VULKAN_UNIQUE_PTR_DECLARATION(VulkanRenderPass);
class VulkanSampler; VULKAN_UNIQUE_PTR_DECLARATION(VulkanSampler);
class VulkanSemaphore; VULKAN_UNIQUE_PTR_DECLARATION(VulkanSemaphore);
class VulkanShaderModule; VULKAN_UNIQUE_PTR_DECLARATION(VulkanShaderModule);
class VulkanSurface; VULKAN_UNIQUE_PTR_DECLARATION(VulkanSurface);
class VulkanSwapchain; VULKAN_UNIQUE_PTR_DECLARATION(VulkanSwapchain);

enum VulkanObjectType : VulkanU32
{
	VulkanObjectType_Unknown,
	VulkanObjectType_Buffer,
	VulkanObjectType_BufferView,
	VulkanObjectType_CommandBuffer,
	VulkanObjectType_CommandPool,
	VulkanObjectType_ComputePipeline,
	VulkanObjectType_DescriptorPool,
	VulkanObjectType_DescriptorSet,
	VulkanObjectType_DescriptorSetLayout,
	VulkanObjectType_Device,
	VulkanObjectType_Fence,
	VulkanObjectType_Framebuffer,
	VulkanObjectType_GraphicsPipeline,
	VulkanObjectType_Image,
	VulkanObjectType_ImageView,
	VulkanObjectType_Instance,
	VulkanObjectType_MemoryBlock,
	VulkanObjectType_PipelineCache,
	VulkanObjectType_PipelineLayout,
	VulkanObjectType_Queue,
	VulkanObjectType_RenderPass,
	VulkanObjectType_Sampler,
	VulkanObjectType_Semaphore,
	VulkanObjectType_ShaderModule,
	VulkanObjectType_Surface,
	VulkanObjectType_Swapchain,
	VulkanObjectType_Max
};

const char* objectTypeToCString(VulkanObjectType type);

class VulkanObjectBase
{
	public:
		VulkanObjectBase()
		{
			#if (defined VULKAN_OPTION_OBJECTNAMES)
				mName = "<unknown>";
			#endif
		}
		
		#if (defined VULKAN_OPTION_OBJECTTRACKER)
		virtual void initializeObject() 
		{
			// TODO : ObjectTracker
		}
		
		virtual void releaseObject()
		{
			// TODO : ObjectTracker
		}
		#endif

	#if (defined VULKAN_OPTION_OBJECTNAMES)
	public:
		const char* getName() const { return mName; }
		void setName(const char* name) { mName = name; }
	private:
		const char* mName;
	#endif
};

template <VulkanObjectType T>
class VulkanObject : public VulkanObjectBase
{
public:
	VulkanObjectType getType() const { return T; }
	static VulkanObjectType getStaticType() { return T; }
};

template <VulkanObjectType T>
class VulkanInstanceObject : public VulkanObject<T>
{
	public:
		VulkanInstanceObject()
		{
			// TODO : static_assert(T is an Instance Type);
		}
		
		VulkanInstance& getInstance() { return VulkanInstance::get(); }
		const VulkanInstance& getInstance() const { return VulkanInstance::get(); }
		const VkInstance& getInstanceHandle() const { return VulkanInstance::get().getHandle(); }
};

template <VulkanObjectType T>
class VulkanDeviceObject : public VulkanObject<T>
{
	public:
		VulkanDeviceObject()
		{
			// TODO : static_assert(T is a Device Type);
		}

		const VkDevice& getDeviceHandle() const { return getDevice().getHandle(); }
		const VkPhysicalDevice& getPhysicalDeviceHandle() const { return getDevice().getPhysicalDeviceHandle(); }
		const VulkanPhysicalDevice& getPhysicalDevice() const { return getDevice().getPhysicalDevice(); }
	
		VulkanDevice& getDevice()
		{
			#if (defined VULKAN_MONO_DEVICE)
			return VulkanDevice::get();
			#else
			return VulkanDevice::get(mDeviceId);
			#endif
		}
	
		const VulkanDevice& getDevice() const 
		{ 
			#if (defined VULKAN_MONO_DEVICE)
			return VulkanDevice::get();
			#else
			return VulkanDevice::get(mDeviceId);
			#endif
		}

	#if (!defined VULKAN_MONO_DEVICE)		
		const VulkanDeviceId& getDeviceId() const { return mDeviceId; }
	protected:
		friend class VulkanDevice;
		void setDeviceId(const VulkanDeviceId& deviceId) { mDeviceId = deviceId; }
	private:
		VulkanDeviceId mDeviceId;
	#endif
};

VULKAN_NAMESPACE_END

#if (defined VULKAN_OPTION_OBJECTTRACKER)

	#include <unordered_map>

	VULKAN_NAMESPACE_BEGIN

	class VulkanObjectTracker
	{
		public:
			VulkanObjectTracker() = delete;
			~VulkanObjectTracker() = delete;

			static void printCurrentState();
			static VulkanU32 getLeaksCount();
			static bool hasLeaks();

			static VulkanU64 getTotalObjectsCreatedForType(VulkanObjectType type);
			static VulkanU64 getTotalObjectsCreated();

			static void registerObject(VulkanObjectBase* object);
			static void unregisterObject(VulkanObjectBase* object);

		private:
			static std::unordered_multimap<VulkanObjectType, VulkanObjectBase*> sObjects;
			static VulkanU64 sObjectsCreatedTotal[VulkanObjectType_Max];
	};

	VULKAN_NAMESPACE_END

	#define VULKAN_OBJECTTRACKER_REGISTER() VulkanObjectTracker::registerObject(this);
	#define VULKAN_OBJECTTRACKER_UNREGISTER() VulkanObjectTracker::unregisterObject(this);

#else

	#define VULKAN_OBJECTTRACKER_REGISTER() ((void)0)
	#define VULKAN_OBJECTTRACKER_UNREGISTER() ((void)0)

#endif // (defined VULKAN_OPTION_OBJECTTRACKER)

#undef VULKAN_UNIQUE_PTR_DECLARATION
		