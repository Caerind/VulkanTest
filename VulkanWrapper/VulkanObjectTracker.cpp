#include "VulkanObjectTracker.hpp"

#include <cstdio> // TODO : Use Numea Log System

namespace nu
{
namespace Vulkan
{

void ObjectTracker::printCurrentState()
{
	for (auto itr = sAllocations.begin(); itr != sAllocations.end(); itr++)
	{
		if (itr->second != 0)
		{
			// TODO : Use Numea Log System
			printf("%s : %d objects\n", objectTypeToCString(itr->first), itr->second);
		}
	}
}

bool ObjectTracker::checkForLeaks()
{
	bool noLeaksFound = true;
	for (auto itr = sAllocations.begin(); itr != sAllocations.end(); itr++)
	{
		if (itr->second != 0)
		{
			// TODO : Use Numea Log System
			printf("%d objects of type %s has not been released\n", itr->second, objectTypeToCString(itr->first));
			noLeaksFound = false;
		}
	}
	return noLeaksFound;
}

void ObjectTracker::registerObject(ObjectType type)
{
	sAllocations[type]++;
}

void ObjectTracker::unregisterObject(ObjectType type)
{
	sAllocations[type]--;
}

const char* ObjectTracker::objectTypeToCString(ObjectType type)
{
	switch (type)
	{
		case ObjectType_Buffer: return "Buffer";
		case ObjectType_BufferView: return "BufferView";
		case ObjectType_CommandBuffer: return "CommandBuffer";
		case ObjectType_CommandPool: return "CommandPool";
		case ObjectType_ComputePipeline: return "ComputePipeline";
		case ObjectType_DescriptorPool: return "DescriptorPool";
		case ObjectType_DescriptorSet: return "DescriptorSet";
		case ObjectType_DescriptorSetLayout: return "DescriptorSetLayout";
		case ObjectType_Device: return "Device";
		case ObjectType_Fence: return "Fence";
		case ObjectType_Framebuffer: return "Framebuffer";
		case ObjectType_GraphicsPipeline: return "GraphicsPipeline";
		case ObjectType_Image: return "Image";
		case ObjectType_ImageView: return "ImageView";
		case ObjectType_Instance: return "Instance";
		case ObjectType_MemoryBlock: return "MemoryBlock";
		case ObjectType_PipelineCache: return "PipelineCache";
		case ObjectType_PipelineLayout: return "PipelineLayout";
		case ObjectType_Queue: return "Queue";
		case ObjectType_RenderPass: return "RenderPass";
		case ObjectType_Sampler: return "Sampler";
		case ObjectType_Semaphore: return "Semaphore";
		case ObjectType_ShaderModule: return "Semaphore";
		case ObjectType_Surface: return "Surface";
		case ObjectType_Swapchain: return "Swapchain";
		default: return "Unknown";
	}
	return "Unknown";
}

std::map<ObjectType, int> ObjectTracker::sAllocations;

} // namespace Vulkan
} // namespace nu