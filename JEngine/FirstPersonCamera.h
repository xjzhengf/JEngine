#pragma once
#include "stdafx.h"
#include "Camera.h"

class FirstPersonCamera : public Camera
{
public:
	//Ïà»úÒÆ¶¯
	void Strafe(float d) ;
	void Walk(float d) ;
	void UpDown(float d) ;
	void Pitch(float angle) ;
	void RotateY(float angle) ;
	void RotateLook(float angle) ;

	 void OnMouseDown( int x, int y);
	 void OnMouseMove(int x, int y) ;
	 void OnMouseUp(int x, int y) ;


	 bool CameraMove(const std::string& MouseKey, const WPARAM& wParam, LPARAM lParam) override;
private:
	POINT mLastMousePos;

};
