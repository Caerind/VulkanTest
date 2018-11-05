#ifndef NU_VULKAN_PIPELINE_CACHE_HPP
#define NU_VULKAN_PIPELINE_CACHE_HPP

#include "VulkanFunctions.hpp"

#include <memory>
#include <vector>

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

namespace nu
{
namespace Vulkan
{

class Device;
class PipelineCache
{
	public:
		typedef std::unique_ptr<PipelineCache> Ptr;

		static PipelineCache::Ptr createPipelineCache(Device& device, const std::vector<unsigned char>& cacheData = {});

		~PipelineCache();

		// TODO : Load From File
		// TODO : Save To File

		bool retrieveData();
		const std::vector<unsigned char>& getCacheData() const;

		bool merge(const std::vector<PipelineCache*>& sourcePipelineCaches);

		bool isInitialized() const;
		const VkPipelineCache& getHandle() const;
		const Device& getDeviceHandle() const;

	private:
		PipelineCache(Device& device, const std::vector<unsigned char>& cacheData = {});

		bool init();
		bool release();

		Device& mDevice;
		VkPipelineCache mPipelineCache; 
		
		std::vector<unsigned char> mCacheData;
};

} // namespace Vulkan
} // namespace nu

#endif // NU_VULKAN_PIPELINE_CACHE_HPP