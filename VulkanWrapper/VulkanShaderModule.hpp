#pragma once

#include "VulkanFunctions.hpp"

// TODO : SpecializationInfo param
// TODO : Add GLSL->Spirv tool
// TODO : Get info about variables in shaders
// TODO : Auto find entrypoints
// TODO : Find better interface ?

VULKAN_NAMESPACE_BEGIN

class VulkanShaderModule : public VulkanDeviceObject<VulkanObjectType_ShaderModule>
{
	public:
		// TODO : Add multi stage shaders
		enum ShaderStageFlags
		{
			None = 0,
			Vertex = 1,
			TessellationControl = 2,
			TessellationEvaluation = 4,
			Geometry = 8,
			Fragment = 16,
			Compute = 32
		};

		~VulkanShaderModule();

		bool loadFromFile(const std::string& filename, ShaderStageFlags stage = ShaderStageFlags::None, const std::string& entrypoint = "main");

		void setVertexEntrypointName(const std::string& entrypoint);
		void setTessellationControlEntrypointName(const std::string& entrypoint);
		void setTessellationEvaluationEntrypointName(const std::string& entrypoint);
		void setGeometryEntrypointName(const std::string& entrypoint);
		void setFragmentEntrypointName(const std::string& entrypoint);
		void setComputeEntrypointName(const std::string& entrypoint);

		bool create();
		void destroy();
		bool isCreated() const;
		const VkShaderModule& getHandle() const;

		const std::string& getVertexEntrypointName() const;
		const std::string& getTessellationControlEntrypointName() const;
		const std::string& getTessellationEvaluationEntrypointName() const;
		const std::string& getGeometryEntrypointName() const;
		const std::string& getFragmentEntrypointName() const;
		const std::string& getComputeEntrypointName() const;

		const std::vector<unsigned char>& getSpirvBlob() const;

		const ShaderStageFlags& getStages() const;

	private:
		friend class VulkanDevice;
		VulkanShaderModule();

		friend class VulkanGraphicsPipeline;
		void addShaderStages(std::vector<VkPipelineShaderStageCreateInfo>& shaders);

		friend class VulkanComputePipeline;
		const VkPipelineShaderStageCreateInfo& getShaderStage();

		void generateShaderStages() const;

	private:
		VkShaderModule mShaderModule;

		ShaderStageFlags mStages;
		std::vector<unsigned char> mSpirvBlob;
		VkSpecializationInfo* mSpecializationInfo;

		std::string mVertexEntrypointName;
		std::string mTessellationControlEntrypointName;
		std::string mTessellationEvaluationEntrypointName;
		std::string mGeometryEntrypointName;
		std::string mFragmentEntrypointName;
		std::string mComputeEntrypointName;

		mutable std::vector<VkPipelineShaderStageCreateInfo> mShaderStageCreateInfos;
};

VULKAN_NAMESPACE_END