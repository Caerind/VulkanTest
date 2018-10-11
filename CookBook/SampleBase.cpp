#include "SampleBase.hpp"

MouseStateParameters::MouseStateParameters() 
{
	buttons[0].isPressed = false;
	buttons[0].wasClicked = false;
	buttons[0].wasRelease = false;
	buttons[1].isPressed = false;
	buttons[1].wasClicked = false;
	buttons[1].wasRelease = false;
	position.x = 0;
	position.y = 0;
	position.delta.x = 0;
	position.delta.y = 0;
	wheel.wasMoved = false;
	wheel.distance = 0.0f;
}

MouseStateParameters::~MouseStateParameters()
{
}

TimerStateParameters::TimerStateParameters() 
{
	update();
}

TimerStateParameters::~TimerStateParameters()
{
}

float TimerStateParameters::getTime() const 
{
	auto duration = mTime.time_since_epoch();
	auto milliseconds = std::chrono::duration_cast<std::chrono::milliseconds>(duration).count();
	return static_cast<float>(milliseconds * 0.001f);
}

float TimerStateParameters::getDeltaTime() const 
{
	return mDeltaTime.count();
}

void TimerStateParameters::update() 
{
	auto previousTime = mTime;
	mTime = std::chrono::high_resolution_clock::now();
	mDeltaTime = mTime - previousTime;
}

SampleBase::SampleBase()
	: mReady(false)
{
}

SampleBase::~SampleBase()
{
}

void SampleBase::deinitialize()
{
	if (mLogicalDevice != nullptr)
	{
		mLogicalDevice->waitForAllSubmittedCommands();
	}
}

void SampleBase::mouseClick(size_t buttonIndex, bool state)
{
	if (buttonIndex < 2) 
	{
		mMouseState.buttons[buttonIndex].isPressed = state;
		mMouseState.buttons[buttonIndex].wasClicked = state;
		mMouseState.buttons[buttonIndex].wasRelease = !state;
		
		onMouseEvent();
	}
}

void SampleBase::mouseMove(int x, int y)
{
	mMouseState.position.delta.x = x - mMouseState.position.x;
	mMouseState.position.delta.y = y - mMouseState.position.y;
	mMouseState.position.x = x;
	mMouseState.position.y = y;

	onMouseEvent();
}

void SampleBase::mouseWheel(float distance)
{
	mMouseState.wheel.wasMoved = true;
	mMouseState.wheel.distance = distance;
	
	onMouseEvent();
}

void SampleBase::mouseReset()
{
	mMouseState.position.delta.x = 0;
	mMouseState.position.delta.y = 0;
	mMouseState.buttons[0].wasClicked = false;
	mMouseState.buttons[0].wasRelease = false;
	mMouseState.buttons[1].wasClicked = false;
	mMouseState.buttons[1].wasRelease = false;
	mMouseState.wheel.wasMoved = false;
	mMouseState.wheel.distance = 0.0f;
}

void SampleBase::updateTime()
{
	mTimerState.update();
}

bool SampleBase::isReady()
{
	return mReady;
}

void SampleBase::wait()
{
	mLogicalDevice->waitForAllSubmittedCommands();
}

void SampleBase::onMouseEvent()
{
}

bool SampleBase::initializeVulkan(nu::Vulkan::WindowParameters windowParameters, VkPhysicalDeviceFeatures * desiredDeviceFeatures)
{
	mInstance = nu::Vulkan::Instance::createInstance();
	if (mInstance == nullptr || !mInstance->isInitialized())
	{
		return false;
	}

	mSurface = mInstance->createSurface(windowParameters);
	if (mSurface == nullptr || !mSurface->isInitialized())
	{
		return false;
	}

	const std::vector<nu::Vulkan::PhysicalDevice>& physicalDevices = mInstance->getPhysicalDevices();
	for (uint32_t i = 0; i < physicalDevices.size(); i++)
	{
		const nu::Vulkan::PhysicalDevice& physicalDevice = physicalDevices[i];

		std::vector<uint32_t> requestedQueueFamilyIndexes;

		uint32_t graphicsQueueFamilyIndex = physicalDevice.getGraphicsQueueFamilyIndex();
		if (graphicsQueueFamilyIndex == nu::Vulkan::PhysicalDevice::InvalidQueueFamilyIndex)
		{
			continue;
		}

		requestedQueueFamilyIndexes.push_back(graphicsQueueFamilyIndex);

		uint32_t computeQueueFamilyIndex = physicalDevice.getComputeQueueFamilyIndex();
		if (computeQueueFamilyIndex == nu::Vulkan::PhysicalDevice::InvalidQueueFamilyIndex)
		{
			continue;
		}

		if (computeQueueFamilyIndex != graphicsQueueFamilyIndex)
		{
			requestedQueueFamilyIndexes.push_back(computeQueueFamilyIndex);
		}

		uint32_t presentQueueFamilyIndex = physicalDevice.getPresentQueueFamilyIndex(mSurface.get());
		if (presentQueueFamilyIndex == nu::Vulkan::PhysicalDevice::InvalidQueueFamilyIndex)
		{
			continue;
		}

		if (presentQueueFamilyIndex != graphicsQueueFamilyIndex && presentQueueFamilyIndex != computeQueueFamilyIndex)
		{
			requestedQueueFamilyIndexes.push_back(presentQueueFamilyIndex);
		}

		nu::Vulkan::PhysicalDevice& physicalDeviceRef = mInstance->getPhysicalDevice(i);
		mLogicalDevice = nu::Vulkan::Device::createDevice(physicalDeviceRef, requestedQueueFamilyIndexes, desiredDeviceFeatures);
		if (mLogicalDevice == nullptr || !mLogicalDevice->isInitialized())
		{
			continue;
		}
		else
		{
			mPhysicalDevice = &physicalDeviceRef;

			mGraphicsQueue = mLogicalDevice->getQueue(graphicsQueueFamilyIndex, 0);
			mComputeQueue = mLogicalDevice->getQueue(computeQueueFamilyIndex, 0);
			mPresentQueue = mLogicalDevice->getQueue(presentQueueFamilyIndex, 0);

			if (mGraphicsQueue == nullptr || mComputeQueue == nullptr || mPresentQueue == nullptr)
			{
				return false;
			}

			break;
		}
	}

	if (mLogicalDevice == nullptr || !mLogicalDevice->isInitialized())
	{
		return false;
	}

	// Prepare frame resources
	mCommandPool = mLogicalDevice->createCommandPool(VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT, mGraphicsQueue->getFamilyIndex());
	if (mCommandPool == nullptr || !mCommandPool->isInitialized())
	{
		return false;
	}

	const uint32_t framesCount = 3; // TODO : Unhardcode
	for (uint32_t i = 0; i < framesCount; i++)
	{
		nu::Vulkan::CommandBuffer::Ptr commandBuffer = mCommandPool->allocatePrimaryCommandBuffer();
		if (commandBuffer == nullptr || !commandBuffer->isInitialized())
		{
			return false;
		}

		nu::Vulkan::Semaphore::Ptr imageAcquiredSemaphore = mLogicalDevice->createSemaphore();
		if (imageAcquiredSemaphore == nullptr || !imageAcquiredSemaphore->isInitialized())
		{
			return false;
		}

		nu::Vulkan::Semaphore::Ptr readyToPresentSemaphore = mLogicalDevice->createSemaphore();
		if (readyToPresentSemaphore == nullptr || !readyToPresentSemaphore->isInitialized())
		{
			return false;
		}

		nu::Vulkan::Fence::Ptr drawingFinishedFence = mLogicalDevice->createFence(true);
		if (drawingFinishedFence == nullptr || !drawingFinishedFence->isInitialized())
		{
			return false;
		}

		mFramesResources.emplace_back(
			std::move(commandBuffer),
			std::move(imageAcquiredSemaphore),
			std::move(readyToPresentSemaphore),
			std::move(drawingFinishedFence),
			nullptr, // depthAttachment will be initialized on swapchain creation if depth is used
			nullptr  // framebuffer will be created using a render pass later on
		);
	}

	mSwapchain = mLogicalDevice->createSwapchain(mSurface.get(), mFramesResources);
	if (mSwapchain == nullptr || !mSwapchain->isInitialized())
	{
		return false;
	}

	mReady = true;

	return true;
}
