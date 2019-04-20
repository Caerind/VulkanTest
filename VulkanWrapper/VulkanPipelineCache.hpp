#pragma once

#include "VulkanFunctions.hpp"

/*

Very, very roughly, vkGetPipelineCache will get you a block of bytes 
(pData and pDataSize) that you can persist to disk like any other arbitrary blob.
To reload the cache, use vkCreatePipelineCache to turn the blob back into a 
VkPipelineCache - pData from vkGetPipelineCache becomes VkPipelineCacheCreateInfo::pInitialData 
while VkPipelineCacheCreateInfo:: initialDataSize should be filled in with *pDataSize. 
You now have a VkPipelineCache that you can use with vkCreateGraphicsPipelines 
and vkCreateComputePipelines to speed up the initial pipeline creation.

As (to me an interesting but likely) very niche aside: the first 32 bytes of the cache are 
not arbitrary! You can look at them and use them to match your blobs with a particular platform.
This is important if you're running on a heterogeneous GPU system like a laptop with integrated+discrete 
- you need to know which cache to give to which driver.
You can also try to do funky things like deliver a pre-generated cache with the app, 
or better, pull the cache from a server after checking which implementation you are running on. 
Time-to-first-run is incredibly important, especially on mobile, since if your app takes too long 
to start people often just close it and never run it again (even if next launch will be instantaneous).
https://www.khronos.org/registry/vulkan/specs/1.0/xhtml/vkspec.html#id-1.11.16.29

*/

VULKAN_NAMESPACE_BEGIN

class VulkanPipelineCache : public VulkanDeviceObject<VulkanObjectType_PipelineCache>
{
	public:
		static VulkanPipelineCachePtr createPipelineCache(const std::vector<unsigned char>& cacheData = {});

		~VulkanPipelineCache();

		// TODO : Load From File
		// TODO : Save To File

		bool retrieveData();
		const std::vector<unsigned char>& getCacheData() const;

		bool merge(const std::vector<VulkanPipelineCache*>& sourcePipelineCaches);

		bool isInitialized() const;
		const VkPipelineCache& getHandle() const;

	private:
		VulkanPipelineCache(const std::vector<unsigned char>& cacheData = {});

		bool init();
		bool release();

		VkPipelineCache mPipelineCache; 
		
		std::vector<unsigned char> mCacheData;
};

VULKAN_NAMESPACE_END