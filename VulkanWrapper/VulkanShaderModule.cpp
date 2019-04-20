#include "VulkanShaderModule.hpp"

#include "VulkanDevice.hpp"

#include <fstream>

VULKAN_NAMESPACE_BEGIN

VulkanShaderModule::~VulkanShaderModule()
{
	destroy();

	VULKAN_OBJECTTRACKER_UNREGISTER();
}

bool VulkanShaderModule::loadFromFile(const std::string& filename, ShaderStageFlags stage, const std::string& entrypoint)
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

	switch (stage)
	{
		case ShaderStageFlags::None: return true;
		case ShaderStageFlags::Vertex: setVertexEntrypointName(entrypoint); break;
		case ShaderStageFlags::TessellationControl: setTessellationControlEntrypointName(entrypoint); break;
		case ShaderStageFlags::TessellationEvaluation: setTessellationEvaluationEntrypointName(entrypoint); break;
		case ShaderStageFlags::Geometry: setGeometryEntrypointName(entrypoint); break;
		case ShaderStageFlags::Fragment: setFragmentEntrypointName(entrypoint); break;
		case ShaderStageFlags::Compute: setComputeEntrypointName(entrypoint); break;
		default: printf("Stages combination are not supported at loading"); return false; break;
	}

	return create();
}

void VulkanShaderModule::setVertexEntrypointName(const std::string& entrypoint)
{
	mStages = static_cast<ShaderStageFlags>(mStages | ShaderStageFlags::Vertex);
	mVertexEntrypointName = entrypoint;
}

void VulkanShaderModule::setTessellationControlEntrypointName(const std::string& entrypoint)
{
	mStages = static_cast<ShaderStageFlags>(mStages | ShaderStageFlags::TessellationControl);
	mTessellationControlEntrypointName = entrypoint;
}

void VulkanShaderModule::setTessellationEvaluationEntrypointName(const std::string& entrypoint)
{
	mStages = static_cast<ShaderStageFlags>(mStages | ShaderStageFlags::TessellationEvaluation);
	mTessellationEvaluationEntrypointName = entrypoint;
}

void VulkanShaderModule::setGeometryEntrypointName(const std::string& entrypoint)
{
	mStages = static_cast<ShaderStageFlags>(mStages | ShaderStageFlags::Geometry);
	mGeometryEntrypointName = entrypoint;
}

void VulkanShaderModule::setFragmentEntrypointName(const std::string& entrypoint)
{
	mStages = static_cast<ShaderStageFlags>(mStages | ShaderStageFlags::Fragment);
	mFragmentEntrypointName = entrypoint;
}

void VulkanShaderModule::setComputeEntrypointName(const std::string& entrypoint)
{
	mStages = static_cast<ShaderStageFlags>(mStages | ShaderStageFlags::Compute);
	mComputeEntrypointName = entrypoint;
}

bool VulkanShaderModule::create()
{
	VkShaderModuleCreateInfo shaderModuleCreateInfo = {
		VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO,              // VkStructureType              sType
		nullptr,                                                  // const void                 * pNext
		0,                                                        // VkShaderModuleCreateFlags    flags
		mSpirvBlob.size(),                                        // size_t                       codeSize
		reinterpret_cast<uint32_t const *>(mSpirvBlob.data())     // const uint32_t             * pCode
	};

	VkResult result = vkCreateShaderModule(getDeviceHandle(), &shaderModuleCreateInfo, nullptr, &mShaderModule);
	if (result != VK_SUCCESS || mShaderModule == VK_NULL_HANDLE)
	{
		// TODO : Use Numea System Log
		printf("Could not create a shader module\n");
		return false;
	}

	return true;
}

void VulkanShaderModule::destroy()
{
	if (mShaderModule != VK_NULL_HANDLE)
	{
		vkDestroyShaderModule(getDeviceHandle(), mShaderModule, nullptr);
		mShaderModule = VK_NULL_HANDLE;
	}
}

bool VulkanShaderModule::isCreated() const
{
	return mShaderModule != VK_NULL_HANDLE;
}

const VkShaderModule& VulkanShaderModule::getHandle() const
{
	return mShaderModule;
}

const std::string& VulkanShaderModule::getVertexEntrypointName() const
{
	return mVertexEntrypointName;
}

const std::string& VulkanShaderModule::getTessellationControlEntrypointName() const
{
	return mTessellationControlEntrypointName;
}

const std::string& VulkanShaderModule::getTessellationEvaluationEntrypointName() const
{
	return mTessellationEvaluationEntrypointName;
}

const std::string& VulkanShaderModule::getGeometryEntrypointName() const
{
	return mGeometryEntrypointName;
}

const std::string& VulkanShaderModule::getFragmentEntrypointName() const
{
	return mFragmentEntrypointName;
}

const std::string& VulkanShaderModule::getComputeEntrypointName() const
{
	return mComputeEntrypointName;
}

const std::vector<unsigned char>& VulkanShaderModule::getSpirvBlob() const
{
	return mSpirvBlob;
}

const VulkanShaderModule::ShaderStageFlags& VulkanShaderModule::getStages() const
{
	return mStages;
}

VulkanShaderModule::VulkanShaderModule()
	: mShaderModule(VK_NULL_HANDLE)
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
	VULKAN_OBJECTTRACKER_REGISTER();
}

void VulkanShaderModule::addShaderStages(std::vector<VkPipelineShaderStageCreateInfo>& shaders)
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

const VkPipelineShaderStageCreateInfo& VulkanShaderModule::getShaderStage()
{
	assert(isCreated());

	if (mShaderStageCreateInfos.empty())
	{
		generateShaderStages();
	}

	assert(mShaderStageCreateInfos.size() == 1);

	return mShaderStageCreateInfos[0];
}

void VulkanShaderModule::generateShaderStages() const
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

VULKAN_NAMESPACE_END