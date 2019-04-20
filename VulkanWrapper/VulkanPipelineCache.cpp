#include "VulkanPipelineCache.hpp"

#include "VulkanDevice.hpp"

VULKAN_NAMESPACE_BEGIN

VulkanPipelineCachePtr VulkanPipelineCache::createPipelineCache(const std::vector<unsigned char>& cacheData)
{
	VulkanPipelineCachePtr pipelineCache(new VulkanPipelineCache(cacheData));
	if (pipelineCache != nullptr)
	{
		if (!pipelineCache->init())
		{
			pipelineCache.reset();
		}
	}
	return pipelineCache;
}

VulkanPipelineCache::~VulkanPipelineCache()
{
	release();

	VULKAN_OBJECTTRACKER_UNREGISTER();
}

bool VulkanPipelineCache::retrieveData()
{
	mCacheData.clear();

	size_t dataSize = 0;
	VkResult result = VK_SUCCESS;

	result = vkGetPipelineCacheData(getDeviceHandle(), mPipelineCache, &dataSize, nullptr);
	if (result != VK_SUCCESS || dataSize == 0)
	{
		// TODO : Use Numea Log System
		printf("Could not get the size of the pipeline cache\n");
		return false;
	}
	mCacheData.resize(dataSize);

	result = vkGetPipelineCacheData(getDeviceHandle(), mPipelineCache, &dataSize, mCacheData.data());
	if (result != VK_SUCCESS || dataSize == 0)
	{
		// TODO : Use Numea Log System
		printf("Could not acquire pipeline cache data\n");
		return false;
	}

	return true;
}

const std::vector<unsigned char>& VulkanPipelineCache::getCacheData() const
{
	return mCacheData;
}

bool VulkanPipelineCache::merge(const std::vector<VulkanPipelineCache*>& sourcePipelineCaches)
{
	if (sourcePipelineCaches.size() > 0) 
	{
		std::vector<VkPipelineCache> sourceCaches;
		sourceCaches.reserve(sourcePipelineCaches.size());
		for (auto& cache : sourcePipelineCaches)
		{
			sourceCaches.push_back(cache->getHandle());
		}

		VkResult result = vkMergePipelineCaches(getDeviceHandle(), mPipelineCache, static_cast<uint32_t>(sourceCaches.size()), sourceCaches.data());
		if (result != VK_SUCCESS) 
		{
			printf("Could not merge pipeline cache objects\n");
			return false;
		}
		return true;
	}
	return false;
}

bool VulkanPipelineCache::isInitialized() const
{
	return mPipelineCache != VK_NULL_HANDLE;
}

const VkPipelineCache& VulkanPipelineCache::getHandle() const
{
	return mPipelineCache;
}

VulkanPipelineCache::VulkanPipelineCache(const std::vector<unsigned char>& cacheData)
	: mPipelineCache(VK_NULL_HANDLE)
	, mCacheData(cacheData)
{
	VULKAN_OBJECTTRACKER_REGISTER();
}

bool VulkanPipelineCache::init()
{
	VkPipelineCacheCreateInfo pipelineCacheCreateInfo = {
		VK_STRUCTURE_TYPE_PIPELINE_CACHE_CREATE_INFO,     // VkStructureType                sType
		nullptr,                                          // const void                   * pNext
		0,                                                // VkPipelineCacheCreateFlags     flags
		static_cast<uint32_t>(mCacheData.size()),         // size_t                         initialDataSize
		mCacheData.data()                                 // const void                   * pInitialData
	};

	VkResult result = vkCreatePipelineCache(getDeviceHandle(), &pipelineCacheCreateInfo, nullptr, &mPipelineCache);
	if (result != VK_SUCCESS || mPipelineCache == VK_NULL_HANDLE)
	{
		// TODO : Use Numea System Log
		printf("Could not create a pipeline cache\n");
		return false;
	}

	return true;
}

bool VulkanPipelineCache::release()
{
	if (mPipelineCache != VK_NULL_HANDLE)
	{
		vkDestroyPipelineCache(getDeviceHandle(), mPipelineCache, nullptr);
		mPipelineCache = VK_NULL_HANDLE;
		return true;
	}
	return false;
}

VULKAN_NAMESPACE_END