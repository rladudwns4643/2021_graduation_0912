//***************************************************************************************
// Camera.h by Frank Luna (C) 2011 All Rights Reserved.
//   
// Simple first person style camera class that lets the viewer explore the 3D scene.
//   -It keeps track of the camera coordinate system relative to the world space
//    so that the view matrix can be constructed.  
//   -It keeps track of the viewing frustum of the camera so that the projection
//    matrix can be obtained.
//***************************************************************************************

#ifndef CAMERA_H
#define CAMERA_H

namespace Core
{
	extern int g_DisplayWidth;
	extern int g_DisplayHeight;
}

class Character;
class Camera
{
private:
	CameraType m_CameraType;

public:
	void SetCamera(CameraType cameraType, Character* owner);
	void SetCamera(XMFLOAT3 look, XMFLOAT3 up, XMFLOAT3 right);

	CameraType GetCameraType();

	virtual void Initialize();
	virtual void Update(const XMFLOAT3& lookAt, float deltaT);

public:
	explicit Camera(CameraType cameraType = CameraType::eThird);
	virtual ~Camera();
	void OnResize();

	void CameraInitialize(SceneType sceneType);

	// Get/Set world camera position.
	XMVECTOR GetPosition()const;
	XMFLOAT3 GetPosition3f()const;
	void SetPosition(float x, float y, float z);
	void SetPosition(const XMFLOAT3& v);

	// Get camera basis vectors.
	XMVECTOR GetRight()const;
	XMFLOAT3 GetRight3f()const;
	XMVECTOR GetUp()const;
	XMFLOAT3 GetUp3f()const;
	XMVECTOR GetLook()const;
	XMFLOAT3 GetLook3f()const;

	void SetOffset(XMFLOAT3 offset);
	XMFLOAT3& GetOffset();

	XMFLOAT3 GetRotation();
	void SetRotation(XMFLOAT3 rotation);

	void SetTimeLag(float fTimeLag);
	float GetTimeLag() { return(mTimeLag); }

	// Get frustum properties.
	float GetNearZ()const;
	float GetFarZ()const;
	float GetAspect()const;
	float GetFovY()const;
	float GetFovX()const;

	// Get near and far plane dimensions in view space coordinates.
	float GetNearWindowWidth()const;
	float GetNearWindowHeight()const;
	float GetFarWindowWidth()const;
	float GetFarWindowHeight()const;

	// Set frustum.
	void SetLens(float fovY, float aspect, float zn, float zf);

	// Set Target
	void SetTarget(const XMFLOAT3& lookAt);

	// Define camera space via LookAt parameters.
	void LookAt(FXMVECTOR pos, FXMVECTOR target, FXMVECTOR worldUp);
	void LookAt(const XMFLOAT3& pos, const XMFLOAT3& target, const XMFLOAT3& up);

	// Get View/Proj matrices.
	XMMATRIX GetView()const;
	XMMATRIX GetProj()const;
	XMMATRIX GetOrtho()const;

	XMFLOAT4X4 GetView4x4f()const;
	XMFLOAT4X4 GetProj4x4f()const;
	XMFLOAT4X4 GetOrtho4x4f()const;

	// Strafe/Walk the camera a distance d.
	void Strafe(float d);
	void Walk(float d);
	void Up(float d);

	// Rotate the camera.
	void Move(const XMFLOAT3& xmf3Shift);
	void Rotate(float fPitch = 0.f, float fYaw = 0.f, float fRoll = 0.f);

	void Pitch(float angle);
	void RotateY(float angle);

	// After modifying camera position/orientation, call to rebuild the view matrix.
	void UpdateViewMatrix();

	void GenerateFrustum();
	bool IsInFrustum(const XMMATRIX& invWorld, const BoundingBox& otherBounds);


private:
	BoundingFrustum m_Frustum;
	Character* m_Owner = nullptr;

	XMFLOAT3	mPosition = { 0.0f, 0.0f, 0.0f };
	XMFLOAT3	mRight = { 1.0f, 0.0f, 0.0f };
	XMFLOAT3	mUp = { 0.0f, 1.0f, 0.0f };
	XMFLOAT3	mLook = { 0.0f, 0.0f, 1.0f };

	XMFLOAT3	mTarget = { 0.f,0.f,0.f };
	XMFLOAT3	mOffset = { 0.f,0.f,0.f };
	XMFLOAT3	mRotation = { 0.f,0.f,0.f };
	float				mTimeLag = 0.f;

	// frustum properties.
	float mNearZ = 0.0f;
	float mFarZ = 0.0f;
	float mAspect = 0.0f;
	float mFovY = 0.0f;
	float mNearWindowHeight = 0.0f;
	float mFarWindowHeight = 0.0f;

	bool mViewDirty = true;

	// View/Proj matrices.
	XMFLOAT4X4 mView = MathHelper::Identity4x4();
	XMFLOAT4X4 mProj = MathHelper::Identity4x4();
	XMFLOAT4X4 mOrtho = MathHelper::Identity4x4();
};

#endif // CAMERA_H