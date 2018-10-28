#include "Camera.hpp"

const nu::Matrix4f& Camera::getMatrix() const 
{
	if (mDirty) 
	{
		mViewMatrix = {
			mRightVector[0],
			mUpVector[0],
			-mForwardVector[0],
			0.0f,

			mRightVector[1],
			mUpVector[1],
			-mForwardVector[1],
			0.0f,

			mRightVector[2],
			mUpVector[2],
			-mForwardVector[2],
			0.0f,

			mPosition.dotProduct(mRightVector),
			mPosition.dotProduct(mUpVector),
			mPosition.dotProduct(mForwardVector),
			1.0f
		};
		mDirty = false;
	}

	return mViewMatrix;
}

const nu::Vector3f& Camera::getPosition() const 
{
	return mPosition;
}

const nu::Vector3f& Camera::getRightVector() const
{
	return mRightVector;
}

const nu::Vector3f& Camera::getUpVector() const
{
	return mUpVector;
}

const nu::Vector3f& Camera::getForwardVector() const
{
	return mForwardVector;
}

Camera::Camera() 
	: Camera({ 0.0f, 0.0f, 0.0f }, { 1.0f, 0.0f, 0.0f }, { 0.0f, 1.0f, 0.0f }, { 0.0f, 0.0f, -1.0f }) 
{
}

Camera::Camera(const nu::Vector3f& position,
			   const nu::Vector3f& rightVector,
			   const nu::Vector3f& upVector,
			   const nu::Vector3f& forwardVector )
	: mViewMatrix(nu::Matrix4f::identity)
	, mPosition(position)
	, mRightVector(rightVector)
	, mUpVector(upVector)
	, mForwardVector(forwardVector)
	, mDirty(true) 
{
}

Camera::Camera(const Camera& camera) 
{
	mViewMatrix = camera.mViewMatrix;
	mPosition = camera.mPosition;
	mRightVector = camera.mRightVector;
	mUpVector = camera.mUpVector;
	mForwardVector = camera.mForwardVector;
	mDirty = camera.mDirty;
}

Camera::~Camera() 
{
}

Camera& Camera::operator=(const Camera& camera) 
{
	if (this != &camera) 
	{
		mViewMatrix = camera.mViewMatrix;
		mPosition = camera.mPosition;
		mRightVector = camera.mRightVector;
		mUpVector = camera.mUpVector;
		mForwardVector = camera.mForwardVector;
		mDirty = camera.mDirty;
	}
	return *this;
}