#ifndef NU_INDEX_BUFFER_HPP
#define NU_INDEX_BUFFER_HPP

#include "VulkanWrapper/VulkanDevice.hpp"
#include "VulkanWrapper/VulkanBuffer.hpp"
#include "VulkanWrapper/VulkanMemoryBlock.hpp"
#include "VulkanWrapper/VulkanBufferView.hpp"

#include <memory>

namespace nu
{

class IndexBuffer
{
	public:
		typedef std::unique_ptr<IndexBuffer> Ptr;

		static IndexBuffer::Ptr createIndexBuffer(Vulkan::Device& device);
		~IndexBuffer();

	private:
		IndexBuffer();

		Vulkan::Buffer::Ptr mBuffer;
};

} // namespace nu

#endif // NU_INDEX_BUFFER_HPP