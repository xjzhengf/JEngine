#pragma once
#include "stdafx.h"
class Camera {
public:
	Camera();
	~Camera();

	//获取及设置相机的位置
	glm::vec3 GetCameraPos3f() const;
	void SetCameraPos(float x,float y, float z);
	void SetCameraPos(const glm::vec3 &Pos);


	//获取相机的基向量
	glm::vec3 GetRight() const;
	glm::vec3 GetUp() const;
	glm::vec3 GetLook() const;
	//更新相机的基向量
	void SetRight( const glm::vec3 right) ;
	void SetUp(const glm::vec3 up) ;
	void SetLook(const glm::vec3 look) ;

	void SetCameraWnd(const HWND& mhMainWnd);
	void SetViewDirty(bool isUpdate);
	//获取视锥体的属性
	float GetNearZ() const;
	float GetFarZ() const;
	float GetFovY() const;
	float GetFovX() const;
	float GetAspect() const;


	//获取用观察空间坐标表示的近、远平面的大小
	float GetNearWindowWidth() const;
	float GetNearWindowHeight() const;
	float GetFarWindowWidth() const;
	float GetFarWindowHeight() const;

	//设置视锥体
	void SetLens(float FovY,float aspect,float nearZ,float farZ);

	//通过LookAt方法的参数定义摄像机空间
	void LookAt(glm::vec3 pos , glm::vec3 target,glm::vec3 worldUp);

	//获取观察矩阵与投影矩阵
	glm::mat4x4 GetView4x4() const;
	glm::mat4x4 GetProj4x4() const;

	glm::mat4x4 GetPerspectiveFovLH(float FovY,float aspect,float nearZ,float farZ);

	float GetCameraSpeed();
	void AddCameraSpeed(float speed);


	//相机移动
	virtual bool CameraMove(const std::string& MouseKey, const WPARAM& wParam, LPARAM lParam) =0;
	HWND mhMainWnd;
	glm::vec3 VectorMultiplyAdd(glm::vec3 MultiplyV1, glm::vec3 MultiplyV2, glm::vec3 addV);
	glm::vec3 Transform(glm::mat4x4 m, glm::vec3 v);
	void UpdateViewMat();
private:
	glm::vec3 mPos = { 1.0f, 1.0f, 1.0f };
	glm::vec3 mRight = { 0.0f,1.0f,0.0f };
	glm::vec3 mUp = { 0.0f,0.0f,1.0f };
	glm::vec3 mLook = { 1.0f,0.0f,0.0f };

	float mNearZ = 0.0f;
	float mFarZ = 0.0f;
	float mAspect = 0.0f;
	float mFovY = 0.0f;
	float mNearWindowHeight = 0.0f;
	float mFarWindowHeight = 0.0f;
	
	bool mViewDirty = true;

	float mCameraMoveSpeed = 30.0f;
	float mCameraMoveMaxSpeed = 100.0f;
	float mCameraMoveMinSpeed = 10.0f;
	glm::mat4x4 mView = glm::identity<glm::mat4x4>();
	glm::mat4x4 mProj = glm::identity<glm::mat4x4>();


};