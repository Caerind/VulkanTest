#include "OrbitingCamera.hpp"

const nu::Vector3f& OrbitingCamera::getTarget() const 
{
	return mTarget;
}

float OrbitingCamera::getDistance() const 
{
	return mDistance;
}

float OrbitingCamera::getHorizontalAngle() const 
{
	return mHorizontalAngle;
}

float OrbitingCamera::getVerticalAngle() const 
{
	return mVerticalAngle;
}

void OrbitingCamera::setDistance(float distance) 
{
	mDistance = distance;
	if (mDistance < 0.0f) 
	{
		mDistance = 0.0f;
	}
	mPosition = mTarget - mDistance * mForwardVector;
	mDirty = true;
}

void OrbitingCamera::rotateHorizontally(float angle) 
{
	const nu::Matrix4f rotation = nu::Quaternionf(angle, { 0.0f, -1.0f, 0.0f }).toMatrix4();
	mHorizontalAngle += angle;
	mForwardVector = rotation * mForwardVector;
	mUpVector = rotation * mUpVector;
	mRightVector = rotation * mRightVector;
	mPosition = mTarget - mDistance * mForwardVector;
	mDirty = true;
}

void OrbitingCamera::rotateVertically(float angle) 
{
	float const oldAngle = mVerticalAngle;
	float tmp = angle;

	mVerticalAngle += angle;
	if (mVerticalAngle > 90.0f ) 
	{
		mVerticalAngle = 90.0f;
		tmp = 90.0f - oldAngle;
	} 
	else if (mVerticalAngle < -90.0f) 
	{
		mVerticalAngle = -90.0f;
		tmp = -90.0f - oldAngle;
	}

	const nu::Matrix4f rotation = nu::Quaternionf(tmp, mRightVector).toMatrix4();

	mForwardVector = rotation * mForwardVector;
	mUpVector = rotation * mUpVector;
	mPosition = mTarget - mDistance * mForwardVector;
	mDirty = true;
}

OrbitingCamera::OrbitingCamera() 
	: OrbitingCamera({ 0.0f, 0.0f, 0.0f }, 1.0f, 0.0f, 0.0f) 
{
}

OrbitingCamera::OrbitingCamera(const nu::Vector3f& target,
                               float distance,
                               float horizontalAngle,
                               float verticalAngle) 
	: Camera(target - distance * nu::Vector3f(0.0f, 0.0f, -1.0f), { 1.0f, 0.0f, 0.0f }, { 0.0f, 1.0f, 0.0f }, { 0.0f, 0.0f, -1.0f })
	, mTarget(target)
	, mDistance(distance)
	, mHorizontalAngle(0.0f)
	, mVerticalAngle(0.0f) 
{
	rotateHorizontally(horizontalAngle);
	rotateVertically(verticalAngle);
}

OrbitingCamera::OrbitingCamera(const OrbitingCamera& other) 
{
	Camera::operator=(other);
	mTarget = other.mTarget;
	mDistance = other.mDistance;
	mHorizontalAngle = other.mHorizontalAngle;
	mVerticalAngle = other.mVerticalAngle;
}

OrbitingCamera::~OrbitingCamera() 
{
}

OrbitingCamera& OrbitingCamera::operator=(const OrbitingCamera& other) 
{
	if (this != &other) 
	{
		Camera::operator=(other);
		mTarget = other.mTarget;
		mDistance = other.mDistance;
		mHorizontalAngle = other.mHorizontalAngle;
		mVerticalAngle = other.mVerticalAngle;
	}
	return *this;
}