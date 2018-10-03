#include "VulkanGraphicsPipeline.hpp"

#include "VulkanDevice.hpp"

namespace nu
{
namespace Vulkan
{

GraphicsPipeline::Ptr GraphicsPipeline::createGraphicsPipeline(Device& device, const VkGraphicsPipelineCreateInfo& createInfo, PipelineCache* cache)
{
	GraphicsPipeline::Ptr graphicsPipeline(new GraphicsPipeline(device, createInfo, cache));
	if (graphicsPipeline != nullptr)
	{
		if (!graphicsPipeline->init())
		{
			graphicsPipeline.reset();
		}
	}
	return graphicsPipeline;
}

GraphicsPipeline::~GraphicsPipeline()
{
	ObjectTracker::unregisterObject(ObjectType_GraphicsPipeline);

	release();
}

bool GraphicsPipeline::isInitialized() const
{
	return mGraphicsPipeline != VK_NULL_HANDLE;
}

const VkPipeline& GraphicsPipeline::getHandle() const
{
	return mGraphicsPipeline;
}

const Device& GraphicsPipeline::getDeviceHandle() const
{
	return mDevice;
}

GraphicsPipeline::GraphicsPipeline(Device& device, const VkGraphicsPipelineCreateInfo& createInfo, PipelineCache* cache)
	: mDevice(device)
	, mGraphicsPipeline(VK_NULL_HANDLE)
	, mCreateInfo(createInfo)
	, mCache(cache)
{
	ObjectTracker::registerObject(ObjectType_GraphicsPipeline);
}

bool GraphicsPipeline::init()
{
	VkPipelineCache cacheHandle = (mCache != nullptr) ? mCache->getHandle() : VK_NULL_HANDLE;

	VkResult result = vkCreateGraphicsPipelines(mDevice.getHandle(), cacheHandle, 1, &mCreateInfo, nullptr, &mGraphicsPipeline);
	if (result != VK_SUCCESS || mGraphicsPipeline == VK_NULL_HANDLE)
	{
		// TODO : Use Numea System Log
		printf("Could not create a graphics pipeline\n");
		return false;
	}

	return true;
}

bool GraphicsPipeline::release()
{
	if (mGraphicsPipeline != VK_NULL_HANDLE)
	{
		vkDestroyPipeline(mDevice.getHandle(), mGraphicsPipeline, nullptr);
		mGraphicsPipeline = VK_NULL_HANDLE;
		return true;
	}
	return false;
}

} // namespace Vulkan
} // namespace nu