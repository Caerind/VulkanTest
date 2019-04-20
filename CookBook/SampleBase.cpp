#include "SampleBase.hpp"

#include "../VulkanWrapper/VulkanPhysicalDevice.hpp"
#include "../VulkanWrapper/VulkanQueue.hpp"

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
	if (VulkanDevice::initialized())
	{
		wait();
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
	VulkanDevice::get().waitForAllSubmittedCommands();
}

void SampleBase::onMouseEvent()
{
}

bool SampleBase::initializeVulkan(VulkanWindowParameters windowParameters, VkPhysicalDeviceFeatures* desiredDeviceFeatures)
{
	if (!VulkanInstance::initialize())
	{
		return false;
	}

	mSurface = VulkanInstance::get().createSurface(windowParameters);
	if (mSurface == nullptr)
	{
		return false;
	}

	for (uint32_t i = 0; i < VulkanInstance::get().getPhysicalDeviceCount(); i++)
	{
		VulkanPhysicalDevice& physicalDevice = VulkanInstance::get().getPhysicalDevice(i);

		physicalDevice.queryAllProperties();

		VulkanQueueParameters queueParameters;
		queueParameters.addQueueInfo(1, VulkanQueueParameters::Graphics);
		queueParameters.addQueueInfo(1, VulkanQueueParameters::Compute);
		queueParameters.addQueueInfo(1, VulkanQueueParameters::Present);

		VulkanPhysicalDevice& physicalDeviceRef = VulkanInstance::get().getPhysicalDevice(i);
		if (!VulkanDevice::initialize(physicalDeviceRef, desiredDeviceFeatures, queueParameters, mSurface.get()))
		{
			continue;
		}
		else
		{
			mGraphicsQueue = VulkanDevice::get().getQueue(0);
			mComputeQueue = VulkanDevice::get().getQueue(1);
			mPresentQueue = VulkanDevice::get().getQueue(2);

			if (mGraphicsQueue == nullptr || mComputeQueue == nullptr || mPresentQueue == nullptr)
			{
				return false;
			}

			break;
		}
	}

	if (!VulkanDevice::initialized())
	{
		return false;
	}

	// Prepare frame resources
	mCommandPool = VulkanDevice::get().createCommandPool(VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT, mGraphicsQueue->getFamilyIndex());
	if (mCommandPool == nullptr || !mCommandPool->isInitialized())
	{
		return false;
	}

	const uint32_t framesCount = 3; // TODO : Unhardcode
	for (uint32_t i = 0; i < framesCount; i++)
	{
		VulkanCommandBufferPtr commandBuffer = mCommandPool->allocatePrimaryCommandBuffer();
		if (commandBuffer == nullptr || !commandBuffer->isInitialized())
		{
			return false;
		}

		VulkanSemaphorePtr imageAcquiredSemaphore = VulkanDevice::get().createSemaphore();
		if (imageAcquiredSemaphore == nullptr)
		{
			return false;
		}

		VulkanSemaphorePtr readyToPresentSemaphore = VulkanDevice::get().createSemaphore();
		if (readyToPresentSemaphore == nullptr)
		{
			return false;
		}

		VulkanFencePtr drawingFinishedFence = VulkanDevice::get().createFence(true);
		if (drawingFinishedFence == nullptr)
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

	mSwapchain = VulkanDevice::get().createSwapchain(*(mSurface.get()), mFramesResources);
	if (mSwapchain == nullptr)
	{
		return false;
	}

	mReady = true;

	return true;
}
