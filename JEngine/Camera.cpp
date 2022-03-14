#include "stdafx.h"
#include "Camera.h"
#include "Engine.h"


 glm::vec3 Camera::Transform(glm::mat4x4 m,glm::vec3 v) {
	glm::vec4 Z = { v.z,v.z,v.z,v.z };
	glm::vec4 Y = { v.y,v.y,v.y,v.y };
	glm::vec4 X = { v.x,v.x,v.x,v.x };
	
	glm::vec3 Result = { Z.x * m[2].x,Z.y * m[2].y,Z.z * m[2].z };

	Result = VectorMultiplyAdd(Y, m[1], Result);
	Result = VectorMultiplyAdd(X, m[0], Result);

	return Result;
}

 glm::vec3 Camera::VectorMultiplyAdd(glm::vec3 MultiplyV1, glm::vec3 MultiplyV2, glm::vec3 addV) {
	glm::vec3 result;
	result.x = MultiplyV1.x * MultiplyV2.x + addV.x;
	result.y = MultiplyV1.y * MultiplyV2.y + addV.y;
	result.z = MultiplyV1.z * MultiplyV2.z + addV.z;

	return result;
}


void Camera::UpdateViewMat()
{
	if (mViewDirty) {
		glm::vec3 Right = mRight;
		glm::vec3 Up = mUp;
		glm::vec3 Look = mLook;
		glm::vec3 Pos = mPos;

		Look = glm::normalize(Look);
		Up = glm::normalize(glm::cross(Look,Right));
		Right = glm::cross(Up, Look);

		float x = -glm::dot(mPos,mRight);
		float y = -glm::dot(mPos,mUp);
		float z = -glm::dot(mPos,mLook);

		mRight = Right;
		mUp = Up;
		mLook = Look;

		mView[0][0] = mRight.x;
		mView[1][0] = mRight.y;
		mView[2][0] = mRight.z;
		mView[3][0] = x;

		mView[0][1] = mUp.x;
		mView[1][1] = mUp.y;
		mView[2][1] = mUp.z;
		mView[3][1] = y;

		mView[0][2] = mLook.x;
		mView[1][2] = mLook.y;
		mView[2][2] = mLook.z;
		mView[3][2] = z;
		
		mView[0][3] = 0.0f;
		mView[1][3] = 0.0f;
		mView[2][3] = 0.0f;
		mView[3][3] = 1.0f;

		mViewDirty = false;
	}
}

Camera::Camera()
{
	mhMainWnd = Engine::GetEngine()->GetWindow()->GetHWnd();
	SetLens(0.25f * glm::pi<float>(), 1.0f, 1.0f, 1000.0f);
}

Camera::~Camera()
{
}


glm::vec3 Camera::GetCameraPos3f() const
{
	return mPos;
}

void Camera::SetCameraPos(float x, float y, float z)
{
	mPos = glm::vec3( x,y,z );
}

void Camera::SetCameraPos(const glm::vec3& Pos)
{
	mPos = Pos;
}



glm::vec3 Camera::GetRight() const
{
	return mRight;
}



glm::vec3 Camera::GetUp() const
{
	return mUp;
}



glm::vec3 Camera::GetLook() const
{
	return mLook;
}

void Camera::SetRight(const glm::vec3 right) 
{
	mRight = right;
}

void Camera::SetUp(const glm::vec3 up) 
{
	mUp = up;
}

void Camera::SetLook(const glm::vec3 look) 
{
	mLook = look;
}

void Camera::SetCameraWnd(const HWND& mhMainWnd)
{
	this->mhMainWnd = mhMainWnd;
}

void Camera::SetViewDirty(bool isUpdate)
{
	mViewDirty = isUpdate;
}




float Camera::GetNearZ() const
{
	return mNearZ;
}

float Camera::GetFarZ() const
{
	return mFarZ;
}

float Camera::GetFovY() const
{
	return mFovY;
}

float Camera::GetFovX() const
{
	float halfWidth = 0.5f * GetNearWindowWidth();
	return 2.0f*atan(halfWidth/mNearZ);
}

float Camera::GetAspect() const
{
	return mAspect;
}

float Camera::GetNearWindowWidth() const
{
	return mAspect*mNearWindowHeight;
}

float Camera::GetNearWindowHeight() const
{
	return mNearWindowHeight;
}

float Camera::GetFarWindowWidth() const
{
	return mAspect*mFarWindowHeight;
}

float Camera::GetFarWindowHeight() const
{
	return mFarWindowHeight;
}

void Camera::SetLens(float FovY, float aspect, float nearZ, float farZ)
{
	mFovY = FovY;
	mAspect = aspect;
	mNearZ = nearZ;
	mFarZ = farZ;

	mNearWindowHeight = 2.0f * mNearZ * tanf(0.5f * FovY);
	mFarWindowHeight = 2.0f * mFarZ * tanf(0.5f * FovY);
	//����ͶӰ����
	mProj = GetPerspectiveFovLH(mFovY, mAspect, mNearZ, mFarZ);
	//mProj=glm::perspectiveFovLH(mFovY, 800.0f, 600.0f, mNearZ, mFarZ);
	//XMMATRIX P = XMMatrixPerspectiveFovLH(mFovY, mAspect, mNearZ, mFarZ);
}


void Camera::LookAt(glm::vec3 pos, glm::vec3 target, glm::vec3 worldUp)
{
	mLook = glm::normalize(target - pos);
	mRight = glm::normalize(glm::cross(worldUp, mLook));
	mUp = glm::cross(mLook, mRight);
	mPos = pos;

	mViewDirty = true;
}

glm::mat4x4 Camera::GetView4x4() const
{
	return mView;
}

glm::mat4x4 Camera::GetProj4x4() const
{
	return mProj;
}

glm::mat4x4 Camera::GetPerspectiveFovLH(float fovY, float aspect, float nearZ, float farZ)
{

	//glm::mat4x4 P = {
	//	1 / (aspect * (tanf(fovY / 2))),0,0,0,
	//	0,1 / (tanf(fovY / 2)),0,0,
	//	0,0,farZ / (farZ - nearZ),1,
	//	0,0,-nearZ * farZ / (farZ - nearZ),0
	//};
	glm::mat4x4 P;
	P[0][0] = 1 / (aspect * (tanf(fovY / 2)));
	P[0][1] = 0.0f;
	P[0][2] = 0.0f;
	P[0][3] = 0.0f;

	P[1][0] = 0.0f;
	P[1][1] = 1 / (tanf(fovY / 2));
	P[1][2] = 0.0f;
	P[1][3] = 0.0f;

	P[2][0] = 0.0f;
	P[2][1] = 0.0f;
	P[2][2] = farZ / (farZ - nearZ);
	P[2][3] = 1.0f;

	P[3][0] = 0.0f;
	P[3][1] = 0.0f;
	P[3][2] = -nearZ * farZ / (farZ - nearZ);
	P[3][3] = 0.0f;
	return P;
}

float Camera::GetCameraSpeed()
{
	return mCameraMoveSpeed;
}

void Camera::AddCameraSpeed(float speed)
{
	mCameraMoveSpeed += speed;
	if (mCameraMoveSpeed > mCameraMoveMaxSpeed) {
		mCameraMoveSpeed = mCameraMoveMaxSpeed;
	}
	if (mCameraMoveSpeed < mCameraMoveMinSpeed) {
		mCameraMoveSpeed = mCameraMoveMinSpeed;
	}
}
