#include "VulkanCore.hpp"

#if (defined VULKAN_OPTION_LOG)
	#include <cstdarg>
	#include <cstdio>
#endif

VULKAN_NAMESPACE_BEGIN

namespace VulkanPriv
{
	const VulkanU32 gLogBufferSize = 1024;
	char gLogFormatBuffer[gLogBufferSize];
	char gLogFinalBuffer[gLogBufferSize];

	void printLog(const char* logType, const char* str, ...)
	{
		#if (defined VULKAN_OPTION_LOG || defined VULKAN_OPTION_ASSERT)
			va_list args;
			va_start(args, str);
			vsprintf_s(gLogFormatBuffer, str, args); // TODO : vsrpintf without _s for not VS
			va_end(args);
			sprintf_s(gLogFinalBuffer, "[%s] %s\n", logType, gLogFormatBuffer);

			printf(gLogFinalBuffer);
			#if (defined VULKAN_PLATFORM_WINDOWS && defined(_MSC_VER)) // VisualStudio
				OutputDebugStringA(gLogFinalBuffer);
			#endif
		#endif
	}
}

const char* objectTypeToCString(VulkanObjectType type)
{
	switch (type)
	{
		case VulkanObjectType_Unknown: return "Unknown";
		case VulkanObjectType_Buffer: return "Buffer";
		case VulkanObjectType_BufferView: return "BufferView";
		case VulkanObjectType_CommandBuffer: return "CommandBuffer";
		case VulkanObjectType_CommandPool: return "CommandPool";
		case VulkanObjectType_ComputePipeline: return "ComputePipeline";
		case VulkanObjectType_DescriptorPool: return "DescriptorPool";
		case VulkanObjectType_DescriptorSet: return "DescriptorSet";
		case VulkanObjectType_DescriptorSetLayout: return "DescriptorSetLayout";
		case VulkanObjectType_Device: return "Device";
		case VulkanObjectType_Fence: return "Fence";
		case VulkanObjectType_Framebuffer: return "Framebuffer";
		case VulkanObjectType_GraphicsPipeline: return "GraphicsPipeline";
		case VulkanObjectType_Image: return "Image";
		case VulkanObjectType_ImageView: return "ImageView";
		case VulkanObjectType_Instance: return "Instance";
		case VulkanObjectType_MemoryBlock: return "MemoryBlock";
		case VulkanObjectType_PipelineCache: return "PipelineCache";
		case VulkanObjectType_PipelineLayout: return "PipelineLayout";
		case VulkanObjectType_Queue: return "Queue";
		case VulkanObjectType_RenderPass: return "RenderPass";
		case VulkanObjectType_Sampler: return "Sampler";
		case VulkanObjectType_Semaphore: return "Semaphore";
		case VulkanObjectType_ShaderModule: return "ShaderModule";
		case VulkanObjectType_Surface: return "Surface";
		case VulkanObjectType_Swapchain: return "Swapchain";

		default: return "<unknown>"; // TODO : Find how to identify easily "Unknown" types
	}
}

VULKAN_NAMESPACE_END

#if (defined VULKAN_OPTION_OBJECTTRACKER)

	VULKAN_NAMESPACE_BEGIN

	void VulkanObjectTracker::printCurrentState()
	{
		/*
		
		// TODO : Log with VA_ARGS
		for (auto typeItr = sObjects.begin(); typeItr != sObjects.end(); typeItr++)
		{
			std::vector<VulkanObject*>& objectsOfType = typeItr->second;
			//VULKAN_LOG_INFO("Type: %s [%d]", objectTypeToCString(typeItr->first), objectsOfType.size());
	#if (defined VULKAN_OPTION_OBJECTNAMES)
			for (VulkanObject* object : objectsOfType)
			{
				VULKAN_LOG_INFO(" - %s", object->getName());
			}
	#endif // (defined VULKAN_OPTION_OBJECTNAMES)
		}

		*/
	}

	VulkanU32 VulkanObjectTracker::getLeaksCount()
	{
		VulkanU32 count = 0;
		/*

		for (auto typeItr = sObjects.begin(); typeItr != sObjects.end(); typeItr++)
		{
			count += typeItr->second.size();
		}

		*/
		return count;
	}

	bool VulkanObjectTracker::hasLeaks()
	{
		return sObjects.size() > 0;
	}

	VulkanU64 VulkanObjectTracker::getTotalObjectsCreatedForType(VulkanObjectType type)
	{
		return sObjectsCreatedTotal[type];
	}

	VulkanU64 VulkanObjectTracker::getTotalObjectsCreated()
	{
		VulkanU64 count = 0;
		
		/*
		for (VulkanU32 i = 0; i < VulkanObjectType_Max; i++)
		{
			count += sObjectsCreatedTotal[(VulkanObjectType)i];
		}
		*/

		return count;
	}

	void VulkanObjectTracker::registerObject(VulkanObjectBase* object)
	{
		/*
		VULKAN_ASSERT(object != nullptr);
		sObjects[object->getType()].push_back(object);
		sObjectsCreatedTotal[object->getType()]++;
		*/
	}

	void VulkanObjectTracker::unregisterObject(VulkanObjectBase* object)
	{
		/*
		VULKAN_ASSERT(object != nullptr);
		auto typeItr = sObjects.find(object->getType());
		if (typeItr != sObjects.end())
		{
			std::vector<VulkanObject*>& objectsOfType = typeItr->second;
			for (auto objItr = objectsOfType.rbegin(); objItr != objectsOfType.rend();)
			{
				if (*objItr == object)
				{
					objItr = objectsOfType.erase(objItr);
					break;
				}
				else
				{
					objItr++;
				}
			}
			if (objectsOfType.size() == 0)
			{
				sObjects.erase(typeItr);
			}
		}
		*/
	}

	std::unordered_multimap<VulkanObjectType, VulkanObjectBase*> VulkanObjectTracker::sObjects;
	VulkanU64 VulkanObjectTracker::sObjectsCreatedTotal[VulkanObjectType_Max];

	VULKAN_NAMESPACE_END

#endif // (defined VULKAN_OPTION_OBJECTTRACKER)