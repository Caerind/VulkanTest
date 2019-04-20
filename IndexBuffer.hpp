#pragma once

#include "VulkanWrapper/VulkanDevice.hpp"
#include "VulkanWrapper/VulkanBuffer.hpp"

namespace nu
{

class IndexBuffer
{
	public:
		typedef std::unique_ptr<IndexBuffer> Ptr;

		static IndexBuffer::Ptr createIndexBuffer(VulkanDevice& device);
		~IndexBuffer();

	private:
		IndexBuffer();

		VulkanBufferPtr mBuffer;
};

} // namespace nu