#ifndef NU_VULKAN_OBJECT_TRACKER_HPP
#define NU_VULKAN_OBJECT_TRACKER_HPP

#include <map>

namespace nu
{
namespace Vulkan
{

enum ObjectType
{
	ObjectType_Buffer,
	ObjectType_BufferView,
	ObjectType_CommandBuffer,
	ObjectType_CommandPool,
	ObjectType_ComputePipeline,
	ObjectType_DescriptorPool,
	ObjectType_DescriptorSet,
	ObjectType_DescriptorSetLayout,
	ObjectType_Device,
	ObjectType_Fence,
	ObjectType_Framebuffer,
	ObjectType_GraphicsPipeline,
	ObjectType_Image,
	ObjectType_ImageView,
	ObjectType_Instance,
	ObjectType_MemoryBlock,
	ObjectType_PipelineCache,
	ObjectType_PipelineLayout,
	ObjectType_Queue,
	ObjectType_RenderPass,
	ObjectType_Sampler,
	ObjectType_Semaphore,
	ObjectType_ShaderModule,
	ObjectType_Surface,
	ObjectType_Swapchain
};

class ObjectTracker
{
	public:
		static void printCurrentState();
		static bool checkForLeaks();

		static void registerObject(ObjectType type);
		static void unregisterObject(ObjectType type);

	private:
		ObjectTracker();

		static const char* objectTypeToCString(ObjectType type);

		static std::map<ObjectType, int> sAllocations;
};

} // namespace Vulkan
} // namespace nu

#endif // NU_VULKAN_OBJECT_TRACKER_HPP