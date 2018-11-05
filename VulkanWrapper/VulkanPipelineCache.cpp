#include "VulkanPipelineCache.hpp"

#include "VulkanDevice.hpp"

namespace nu
{
namespace Vulkan
{

PipelineCache::Ptr PipelineCache::createPipelineCache(Device& device, const std::vector<unsigned char>& cacheData)
{
	PipelineCache::Ptr pipelineCache(new PipelineCache(device, cacheData));
	if (pipelineCache != nullptr)
	{
		if (!pipelineCache->init())
		{
			pipelineCache.reset();
		}
	}
	return pipelineCache;
}

PipelineCache::~PipelineCache()
{
	ObjectTracker::unregisterObject(ObjectType_PipelineCache);

	release();
}

bool PipelineCache::retrieveData()
{
	mCacheData.clear();

	size_t dataSize = 0;
	VkResult result = VK_SUCCESS;

	result = vkGetPipelineCacheData(mDevice.getHandle(), mPipelineCache, &dataSize, nullptr);
	if (result != VK_SUCCESS || dataSize == 0)
	{
		// TODO : Use Numea Log System
		printf("Could not get the size of the pipeline cache\n");
		return false;
	}
	mCacheData.resize(dataSize);

	result = vkGetPipelineCacheData(mDevice.getHandle(), mPipelineCache, &dataSize, mCacheData.data());
	if (result != VK_SUCCESS || dataSize == 0)
	{
		// TODO : Use Numea Log System
		printf("Could not acquire pipeline cache data\n");
		return false;
	}

	return true;
}

const std::vector<unsigned char>& PipelineCache::getCacheData() const
{
	return mCacheData;
}

bool PipelineCache::merge(const std::vector<PipelineCache*>& sourcePipelineCaches)
{
	if (sourcePipelineCaches.size() > 0) 
	{
		std::vector<VkPipelineCache> sourceCaches;
		sourceCaches.reserve(sourcePipelineCaches.size());
		for (auto& cache : sourcePipelineCaches)
		{
			sourceCaches.push_back(cache->getHandle());
		}

		VkResult result = vkMergePipelineCaches(mDevice.getHandle(), mPipelineCache, static_cast<uint32_t>(sourceCaches.size()), sourceCaches.data());
		if (result != VK_SUCCESS) 
		{
			printf("Could not merge pipeline cache objects\n");
			return false;
		}
		return true;
	}
	return false;
}

bool PipelineCache::isInitialized() const
{
	return mPipelineCache != VK_NULL_HANDLE;
}

const VkPipelineCache& PipelineCache::getHandle() const
{
	return mPipelineCache;
}

const Device& PipelineCache::getDeviceHandle() const
{
	return mDevice;
}

PipelineCache::PipelineCache(Device& device, const std::vector<unsigned char>& cacheData)
	: mDevice(device)
	, mPipelineCache(VK_NULL_HANDLE)
	, mCacheData(cacheData)
{
	ObjectTracker::registerObject(ObjectType_PipelineCache);
}

bool PipelineCache::init()
{
	VkPipelineCacheCreateInfo pipelineCacheCreateInfo = {
		VK_STRUCTURE_TYPE_PIPELINE_CACHE_CREATE_INFO,     // VkStructureType                sType
		nullptr,                                          // const void                   * pNext
		0,                                                // VkPipelineCacheCreateFlags     flags
		static_cast<uint32_t>(mCacheData.size()),         // size_t                         initialDataSize
		mCacheData.data()                                 // const void                   * pInitialData
	};

	VkResult result = vkCreatePipelineCache(mDevice.getHandle(), &pipelineCacheCreateInfo, nullptr, &mPipelineCache);
	if (result != VK_SUCCESS || mPipelineCache == VK_NULL_HANDLE)
	{
		// TODO : Use Numea System Log
		printf("Could not create a pipeline cache\n");
		return false;
	}

	return true;
}

bool PipelineCache::release()
{
	if (mPipelineCache != VK_NULL_HANDLE)
	{
		vkDestroyPipelineCache(mDevice.getHandle(), mPipelineCache, nullptr);
		mPipelineCache = VK_NULL_HANDLE;
		return true;
	}
	return false;
}

} // namespace Vulkan
} // namespace nu