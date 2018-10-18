#include "VulkanShaderModule.hpp"

#include "VulkanDevice.hpp"

namespace nu
{
namespace Vulkan
{

ShaderModule::~ShaderModule()
{
	ObjectTracker::unregisterObject(ObjectType_ShaderModule);

	destroy();
}

bool ShaderModule::loadFromFile(const std::string& filename)
{
	mStages = ShaderStageFlags::None;
	mShaderStageCreateInfos.clear();

	mSpirvBlob.clear();

	std::ifstream file(filename, std::ios::binary);
	if (file.fail())
	{
		// TODO : Use Numea Log System
		printf("Could not open '%s' file\n", filename.c_str());
		return false;
	}

	std::streampos begin;
	std::streampos end;
	begin = file.tellg();
	file.seekg(0, std::ios::end);
	end = file.tellg();

	if ((end - begin) == 0)
	{
		// TODO : Use Numea Log System
		printf("The '%s' file is empty\n", filename.c_str());
		return false;
	}

	mSpirvBlob.resize(static_cast<size_t>(end - begin));
	file.seekg(0, std::ios::beg);
	file.read(reinterpret_cast<char*>(mSpirvBlob.data()), end - begin);
	file.close();

	return true;
}

void ShaderModule::setVertexEntrypointName(const std::string& entrypoint)
{
	mStages = mStages | ShaderStageFlags::Vertex;
	mVertexEntrypointName = entrypoint;
}

void ShaderModule::setTessellationControlEntrypointName(const std::string& entrypoint)
{
	mStages = mStages | ShaderStageFlags::TessellationControl;
	mTessellationControlEntrypointName = entrypoint;
}

void ShaderModule::setTessellationEvaluationEntrypointName(const std::string& entrypoint)
{
	mStages = mStages | ShaderStageFlags::TessellationEvaluation;
	mTessellationEvaluationEntrypointName = entrypoint;
}

void ShaderModule::setGeometryEntrypointName(const std::string& entrypoint)
{
	mStages = mStages | ShaderStageFlags::Geometry;
	mGeometryEntrypointName = entrypoint;
}

void ShaderModule::setFragmentEntrypointName(const std::string& entrypoint)
{
	mStages = mStages | ShaderStageFlags::Fragment;
	mFragmentEntrypointName = entrypoint;
}

void ShaderModule::setComputeEntrypointName(const std::string& entrypoint)
{
	mStages = mStages | ShaderStageFlags::Compute;
	mComputeEntrypointName = entrypoint;
}

bool ShaderModule::create()
{
	VkShaderModuleCreateInfo shaderModuleCreateInfo = {
		VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO,              // VkStructureType              sType
		nullptr,                                                  // const void                 * pNext
		0,                                                        // VkShaderModuleCreateFlags    flags
		mSpirvBlob.size(),                                        // size_t                       codeSize
		reinterpret_cast<uint32_t const *>(mSpirvBlob.data())     // const uint32_t             * pCode
	};

	VkResult result = vkCreateShaderModule(mDevice.getHandle(), &shaderModuleCreateInfo, nullptr, &mShaderModule);
	if (result != VK_SUCCESS || mShaderModule == VK_NULL_HANDLE)
	{
		// TODO : Use Numea System Log
		printf("Could not create a shader module\n");
		return false;
	}

	return true;
}

bool ShaderModule::destroy()
{
	if (mShaderModule != VK_NULL_HANDLE)
	{
		vkDestroyShaderModule(mDevice.getHandle(), mShaderModule, nullptr);
		mShaderModule = VK_NULL_HANDLE;
		return true;
	}
	return false;
}

bool ShaderModule::isCreated() const
{
	return mShaderModule != VK_NULL_HANDLE;
}

const VkShaderModule& ShaderModule::getHandle() const
{
	return mShaderModule;
}

const std::string& ShaderModule::getVertexEntrypointName() const
{
	return mVertexEntrypointName;
}

const std::string& ShaderModule::getTessellationControlEntrypointName() const
{
	return mTessellationControlEntrypointName;
}

const std::string& ShaderModule::getTessellationEvaluationEntrypointName() const
{
	return mTessellationEvaluationEntrypointName;
}

const std::string& ShaderModule::getGeometryEntrypointName() const
{
	return mGeometryEntrypointName;
}

const std::string& ShaderModule::getFragmentEntrypointName() const
{
	return mFragmentEntrypointName;
}

const std::string& ShaderModule::getComputeEntrypointName() const
{
	return mComputeEntrypointName;
}

const std::vector<unsigned char>& ShaderModule::getSpirvBlob() const
{
	return mSpirvBlob;
}

const ShaderModule::ShaderStageFlags& ShaderModule::getStages() const
{
	return mStages;
}

ShaderModule::ShaderModule(Device& device)
	: mDevice(device)
	, mShaderModule(VK_NULL_HANDLE)
	, mStages(ShaderStageFlags::None)
	, mSpirvBlob()
	, mSpecializationInfo(nullptr)
	, mVertexEntrypointName()
	, mTessellationControlEntrypointName()
	, mTessellationEvaluationEntrypointName()
	, mGeometryEntrypointName()
	, mFragmentEntrypointName()
	, mComputeEntrypointName()
	, mShaderStageCreateInfos()
{
	ObjectTracker::registerObject(ObjectType_ShaderModule);
}

void ShaderModule::addShaderStages(std::vector<VkPipelineShaderStageCreateInfo>& shaders)
{
	// TODO : Use Numea Assert System
	assert(isCreated());

	if (mShaderStageCreateInfos.empty())
	{
		generateShaderStages();
	}

	for (size_t i = 0; i < mShaderStageCreateInfos.size(); i++)
	{
		shaders.push_back(mShaderStageCreateInfos[i]);
	}
}

void ShaderModule::generateShaderStages() const
{
	VkPipelineShaderStageCreateInfo ci;
	ci.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO; // VkStructureType sType
	ci.pNext = nullptr; // const void* pNext
	ci.flags = 0; // VkPipelineShaderStageCreateFlags flags
	ci.module = mShaderModule; // VkShaderModule module
	ci.pSpecializationInfo = mSpecializationInfo; // const VkSpecializationInfo* pSpecializationInfo

	if ((mStages & ShaderStageFlags::Vertex) != 0)
	{
		ci.stage = VK_SHADER_STAGE_VERTEX_BIT;
		ci.pName = mVertexEntrypointName.c_str();

		mShaderStageCreateInfos.push_back(ci);
	}
	if ((mStages & ShaderStageFlags::TessellationControl) != 0)
	{
		ci.stage = VK_SHADER_STAGE_TESSELLATION_CONTROL_BIT;
		ci.pName = mTessellationControlEntrypointName.c_str();

		mShaderStageCreateInfos.push_back(ci);
	}
	if ((mStages & ShaderStageFlags::TessellationEvaluation) != 0)
	{
		ci.stage = VK_SHADER_STAGE_TESSELLATION_EVALUATION_BIT;
		ci.pName = mTessellationEvaluationEntrypointName.c_str();

		mShaderStageCreateInfos.push_back(ci);
	}
	if ((mStages & ShaderStageFlags::Geometry) != 0)
	{
		ci.stage = VK_SHADER_STAGE_GEOMETRY_BIT;
		ci.pName = mGeometryEntrypointName.c_str();

		mShaderStageCreateInfos.push_back(ci);
	}
	if ((mStages & ShaderStageFlags::Fragment) != 0)
	{
		ci.stage = VK_SHADER_STAGE_FRAGMENT_BIT;
		ci.pName = mFragmentEntrypointName.c_str();

		mShaderStageCreateInfos.push_back(ci);
	}
	if ((mStages & ShaderStageFlags::Compute) != 0)
	{
		ci.stage = VK_SHADER_STAGE_COMPUTE_BIT;
		ci.pName = mComputeEntrypointName.c_str();

		mShaderStageCreateInfos.push_back(ci);
	}
}

} // namespace Vulkan
} // namespace nu