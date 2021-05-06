#include "pch.h"
#include "Camera.h"
#include "Character.h"

void Camera::SetCamera(CameraType cameraType, Character* owner)
{
	m_CameraType = cameraType;
	m_Owner = owner;

	switch (m_CameraType)
	{
	case CameraType::eFirst:
	case CameraType::eThird:
		mUp = owner->GetUp();
		mRight = owner->GetRight();
		mLook = owner->GetLook();
		mRight = MathHelper::Normalize(mRight);
		mLook = MathHelper::Normalize(mLook);
		break;
	case CameraType::eFree:
		mUp = owner->GetUp();
		mRight = owner->GetRight();
		mLook = owner->GetLook();
		mRight = MathHelper::Normalize(mRight);
		mLook = MathHelper::Normalize(mLook);
		break;
	}

	mViewDirty = true;
}

void Camera::SetCamera(XMFLOAT3 look, XMFLOAT3 up, XMFLOAT3 right)
{
	m_CameraType = CameraType::eFree;

	mUp = up;
	mRight = right;
	mLook = look;
	mRight = MathHelper::Normalize(mRight);
	mLook = MathHelper::Normalize(mLook);

	mViewDirty = true;
}


CameraType Camera::GetCameraType()
{
	return m_CameraType;
}

void Camera::Initialize()
{
	m_Owner = nullptr;

	mPosition = { 0.0f, 0.0f, 0.0f };
	mRight = { 1.0f, 0.0f, 0.0f };
	mUp = { 0.0f, 1.0f, 0.0f };
	mLook = { 0.0f, 0.0f, 1.0f };

	mTarget = { 0.f,0.f,0.f };
	mOffset = { 0.f,0.f,0.f };
	mRotation = { 0.f,0.f,0.f };
	mTimeLag = 0.f;

	mViewDirty = true;
}

void Camera::Update(const DirectX::XMFLOAT3& lookAt, float deltaT)
{
	switch (m_CameraType)
	{
	case CameraType::eFree:
	case CameraType::eFirst:
	{
		XMFLOAT4X4 xmf4x4Rotate = MathHelper::Identity4x4();
		XMFLOAT3 xmf3Right = mRight;
		XMFLOAT3 xmf3Up = mUp;
		XMFLOAT3 xmf3Look = mLook;
		xmf4x4Rotate._11 = xmf3Right.x; xmf4x4Rotate._21 = xmf3Up.x; xmf4x4Rotate._31 = xmf3Look.x;
		xmf4x4Rotate._12 = xmf3Right.y; xmf4x4Rotate._22 = xmf3Up.y; xmf4x4Rotate._32 = xmf3Look.y;
		xmf4x4Rotate._13 = xmf3Right.z; xmf4x4Rotate._23 = xmf3Up.z; xmf4x4Rotate._33 = xmf3Look.z;

		XMFLOAT3 xmf3Offset = MathHelper::TransformCoord(mOffset, xmf4x4Rotate);
		XMFLOAT3 xmf3Position = MathHelper::Add(m_Owner->GetPosition(), xmf3Offset);


		XMFLOAT4X4 LookAtMat;
		XMStoreFloat4x4(&LookAtMat, DirectX::XMMatrixLookToLH(DirectX::XMLoadFloat3(&mPosition), DirectX::XMLoadFloat3(&GetLook3f()), DirectX::XMLoadFloat3(&GetUp3f())));

		mRight = XMFLOAT3(LookAtMat._11, LookAtMat._21, LookAtMat._31);
		mUp = XMFLOAT3(LookAtMat._12, LookAtMat._22, LookAtMat._32);
		mLook = XMFLOAT3(LookAtMat._13, LookAtMat._23, LookAtMat._33);
		break;
	}
	case CameraType::eThird:
	{
		if (!m_Owner) return;

		XMFLOAT4X4 xmf4x4Rotate = MathHelper::Identity4x4();
		XMFLOAT3 xmf3Right = mRight;
		XMFLOAT3 xmf3Up = mUp;
		XMFLOAT3 xmf3Look = mLook;
		xmf4x4Rotate._11 = xmf3Right.x; xmf4x4Rotate._21 = xmf3Up.x; xmf4x4Rotate._31 = xmf3Look.x;
		xmf4x4Rotate._12 = xmf3Right.y; xmf4x4Rotate._22 = xmf3Up.y; xmf4x4Rotate._32 = xmf3Look.y;
		xmf4x4Rotate._13 = xmf3Right.z; xmf4x4Rotate._23 = xmf3Up.z; xmf4x4Rotate._33 = xmf3Look.z;

		XMFLOAT3 xmf3Offset = MathHelper::TransformCoord(mOffset, xmf4x4Rotate);
		XMFLOAT3 xmf3Position = MathHelper::Add(m_Owner->GetPosition(), xmf3Offset);
		XMFLOAT3 xmf3Direction = MathHelper::Subtract(xmf3Position, mPosition);

		float fLength = MathHelper::Length(xmf3Direction);
		xmf3Direction = MathHelper::Normalize(xmf3Direction);
		float fTimeLagScale = (mTimeLag) ? deltaT * (1.0f / mTimeLag) : 1.0f;
		float fDistance = fLength * fTimeLagScale;
		if (fDistance > fLength) fDistance = fLength;
		if (fLength < 0.01f) fDistance = fLength;
		if (fDistance > 0)
		{
			mPosition = MathHelper::Add(mPosition, xmf3Direction, fDistance);
			//LookAt(mPosition, lookAt,m_Owner->GetUp());
		}

		break;
	}
	}

	if (mPosition.y < 0.f)
		mPosition.y = 10.f;
	mViewDirty = true;
}

Camera::Camera(CameraType cameraType) :
	m_CameraType(cameraType)
{
	Initialize();
	OnResize();
}

Camera::~Camera()
{
}

void Camera::OnResize()
{
	if (mFovY == 0.f) mFovY = 0.25f;

	SetLens(mFovY, static_cast<float>(Core::g_DisplayWidth) / Core::g_DisplayHeight, CAMERA_ZNEAR, CAMERA_ZFAR);
	GenerateFrustum();
}

void Camera::CameraInitialize(SceneType sceneType)
{
	switch (sceneType)
	{
	case SceneType::eLobby:
		// Set FovY
		SetLens(0.25f * MathHelper::Pi, static_cast<float>(Core::g_DisplayWidth) / Core::g_DisplayHeight, CAMERA_ZNEAR, CAMERA_ZFAR);

		m_Owner = nullptr;
		mPosition = { 1106.77, 238.978, 471.743 };
		mRight = { 0.81325, 0.00145013, -0.581913 };
		mUp = { 0.136672, 0.971549, 0.193427 };
		mLook = { 0.565637, -0.236835, 0.789913 };

		mTarget = { 0.f,0.f,0.f };
		mOffset = { 0.f,0.f,0.f };
		mRotation = { 0, 0, 0 };
		mTimeLag = 0.f;

		m_CameraType = CameraType::eFree;
		mViewDirty = true;

		break;

	case SceneType::eGamePlay:
		// Set FovY
		SetLens(0.35f * MathHelper::Pi, static_cast<float>(Core::g_DisplayWidth) / Core::g_DisplayHeight, CAMERA_ZNEAR, CAMERA_ZFAR);

		if (!m_Owner)	return;

		mUp = m_Owner->GetUp();
		mRight = m_Owner->GetRight();
		mLook = m_Owner->GetLook();
		mRight = MathHelper::Normalize(mRight);
		mLook = MathHelper::Normalize(mLook);

		m_CameraType = CameraType::eThird;
		mViewDirty = true;

		break;
	default:
		break;
	}
}

XMVECTOR Camera::GetPosition()const
{
	return XMLoadFloat3(&mPosition);
}

XMFLOAT3 Camera::GetPosition3f()const
{
	return mPosition;
}

void Camera::SetPosition(float x, float y, float z)
{
	mPosition = XMFLOAT3(x, y, z);
	mViewDirty = true;
}

void Camera::SetPosition(const XMFLOAT3& v)
{
	mPosition = v;
	mViewDirty = true;
}

XMVECTOR Camera::GetRight()const
{
	return XMLoadFloat3(&mRight);
}

XMFLOAT3 Camera::GetRight3f()const
{
	return mRight;
}

XMVECTOR Camera::GetUp()const
{
	return XMLoadFloat3(&mUp);
}

XMFLOAT3 Camera::GetUp3f()const
{
	return mUp;
}

XMVECTOR Camera::GetLook()const
{
	return XMLoadFloat3(&mLook);
}

XMFLOAT3 Camera::GetLook3f()const
{
	return mLook;
}


void Camera::SetOffset(DirectX::XMFLOAT3 offset)
{
	if (!m_Owner) return;

	mOffset = offset;
	mPosition.x = m_Owner->GetPosition().x + offset.x;
	mPosition.y = m_Owner->GetPosition().y + offset.y;
	mPosition.z = m_Owner->GetPosition().z + offset.z;

	mViewDirty = true;
}

DirectX::XMFLOAT3& Camera::GetOffset()
{
	return mOffset;
}

XMFLOAT3 Camera::GetRotation()
{
	return mRotation;
}

void Camera::SetRotation(XMFLOAT3 rotation)
{
	mRotation = rotation;
	// cout<< "change - " << mRotation.x << endl;
}

void Camera::SetTimeLag(float fTimeLag)
{
	mTimeLag = fTimeLag;
	mViewDirty = true;
}

float Camera::GetNearZ()const
{
	return mNearZ;
}

float Camera::GetFarZ()const
{
	return mFarZ;
}

float Camera::GetAspect()const
{
	return mAspect;
}

float Camera::GetFovY()const
{
	return mFovY;
}

float Camera::GetFovX()const
{
	float halfWidth = 0.5f * GetNearWindowWidth();
	return 2.0f * atan(halfWidth / mNearZ);
}

float Camera::GetNearWindowWidth()const
{
	return mAspect * mNearWindowHeight;
}

float Camera::GetNearWindowHeight()const
{
	return mNearWindowHeight;
}

float Camera::GetFarWindowWidth()const
{
	return mAspect * mFarWindowHeight;
}

float Camera::GetFarWindowHeight()const
{
	return mFarWindowHeight;
}

void Camera::SetLens(float fovY, float aspect, float zn, float zf)
{
	mFovY = fovY;
	mAspect = aspect;
	mNearZ = zn;
	mFarZ = zf;

	mNearWindowHeight = 2.0f * mNearZ * tanf(0.5f * mFovY);
	mFarWindowHeight = 2.0f * mFarZ * tanf(0.5f * mFovY);

	// Proj
	XMMATRIX P = DirectX::XMMatrixPerspectiveFovLH(mFovY, mAspect, mNearZ, mFarZ);
	XMStoreFloat4x4(&mProj, P);

	// Ortho
	// far = 1.f
	// near = 0.f
	XMMATRIX O = DirectX::XMMatrixOrthographicLH(Core::g_DisplayWidth, Core::g_DisplayHeight, 0.f, 1.f);
	XMStoreFloat4x4(&mOrtho, O);

	mViewDirty = true;
}

void Camera::SetTarget(const DirectX::XMFLOAT3& lookAt)
{
	switch (m_CameraType)
	{
	case CameraType::eThird:
	{
		XMFLOAT4X4 LookAtMat;
		XMStoreFloat4x4(&LookAtMat, DirectX::XMMatrixLookAtLH(XMLoadFloat3(&mPosition), XMLoadFloat3(&lookAt), XMLoadFloat3(&m_Owner->GetUp())));

		mRight = XMFLOAT3(LookAtMat._11, LookAtMat._21, LookAtMat._31);
		mUp = XMFLOAT3(LookAtMat._12, LookAtMat._22, LookAtMat._32);
		mLook = XMFLOAT3(LookAtMat._13, LookAtMat._23, LookAtMat._33);
		break;
	}
	case CameraType::eFree:
	{
		// mRotation 나중에 수정해야함 
		// mRotation은 각 축에 대해서 얼만큼씩 회전했는지를 가지고 있음 (누적)
		// degree -> radian으로 바꿈
		XMVECTOR Up = { 0.f,1.f,0.f };
		Up = XMVector3Transform(Up, DirectX::XMMatrixRotationQuaternion(XMLoadFloat3(&mRotation)));	// 이거 바꿔야할 수도 있음

		XMFLOAT4X4 LookAtMat;
		XMStoreFloat4x4(&LookAtMat, DirectX::XMMatrixLookAtLH(XMLoadFloat3(&mPosition), XMLoadFloat3(&lookAt), Up));

		mRight = XMFLOAT3(LookAtMat._11, LookAtMat._21, LookAtMat._31);
		mUp = XMFLOAT3(LookAtMat._12, LookAtMat._22, LookAtMat._32);
		mLook = XMFLOAT3(LookAtMat._13, LookAtMat._23, LookAtMat._33);
		break;
	}
	}

	mViewDirty = true;
}

void Camera::LookAt(FXMVECTOR pos, FXMVECTOR target, FXMVECTOR worldUp)
{
	XMVECTOR L = XMVector3Normalize(XMVectorSubtract(target, pos));
	XMVECTOR R = XMVector3Normalize(XMVector3Cross(worldUp, L));
	XMVECTOR U = XMVector3Cross(L, R);

	DirectX::XMStoreFloat3(&mPosition, pos);
	DirectX::XMStoreFloat3(&mLook, L);
	DirectX::XMStoreFloat3(&mRight, R);
	DirectX::XMStoreFloat3(&mUp, U);

	mViewDirty = true;
}

void Camera::LookAt(const XMFLOAT3& pos, const XMFLOAT3& target, const XMFLOAT3& up)
{
	XMVECTOR P = XMLoadFloat3(&pos);
	XMVECTOR T = XMLoadFloat3(&target);
	XMVECTOR U = XMLoadFloat3(&up);

	LookAt(P, T, U);

	mViewDirty = true;
}

XMMATRIX Camera::GetView()const
{
	//assert(!mViewDirty);
	return XMLoadFloat4x4(&mView);
}

XMMATRIX Camera::GetProj()const
{
	return XMLoadFloat4x4(&mProj);
}

DirectX::XMMATRIX Camera::GetOrtho() const
{
	return XMLoadFloat4x4(&mOrtho);
}


XMFLOAT4X4 Camera::GetView4x4f()const
{
	//assert(!mViewDirty);
	return mView;
}

XMFLOAT4X4 Camera::GetProj4x4f()const
{
	return mProj;
}

DirectX::XMFLOAT4X4 Camera::GetOrtho4x4f() const
{
	return mOrtho;
}

void Camera::Strafe(float d)
{
	// mPosition += d*mRight
	XMVECTOR s = XMVectorReplicate(d);
	XMVECTOR r = XMLoadFloat3(&mRight);
	XMVECTOR p = XMLoadFloat3(&mPosition);
	DirectX::XMStoreFloat3(&mPosition, XMVectorMultiplyAdd(s, r, p));

	mViewDirty = true;
}

void Camera::Walk(float d)
{

	// mPosition += d*mLook
	XMVECTOR s = XMVectorReplicate(d);
	XMVECTOR l = XMLoadFloat3(&mLook);
	XMVECTOR p = XMLoadFloat3(&mPosition);
	DirectX::XMStoreFloat3(&mPosition, XMVectorMultiplyAdd(s, l, p));

	mViewDirty = true;
}

void Camera::Up(float d)
{

	XMVECTOR s = XMVectorReplicate(d);
	XMVECTOR l = XMLoadFloat3(&mUp);
	XMVECTOR p = XMLoadFloat3(&mPosition);
	DirectX::XMStoreFloat3(&mPosition, XMVectorMultiplyAdd(s, l, p));

	mViewDirty = true;
}

void Camera::Move(const XMFLOAT3& xmf3Shift)
{
	if (m_CameraType == CameraType::eFree) return;

	mPosition = m_Owner->GetPosition();
	
	mPosition.x += xmf3Shift.x;
	mPosition.y += xmf3Shift.y + mOffset.y;
	mPosition.z += xmf3Shift.z;

	mViewDirty = true;
}


void Camera::Rotate(float fPitch, float fYaw, float fRoll)
{
	switch (m_CameraType)
	{
	case CameraType::eFirst:
	{
		if (fPitch)
		{
			// - 고개 관련
			XMFLOAT3 xmf3Right = mRight;
			XMMATRIX xmmtxRotate = DirectX::XMMatrixRotationAxis(XMLoadFloat3(&xmf3Right), fPitch);
			mLook = MathHelper::TransformNormal(mLook, xmmtxRotate);
			mUp = MathHelper::TransformNormal(mUp, xmmtxRotate);
			mRight = MathHelper::TransformNormal(mRight, xmmtxRotate);
		}
		if (m_Owner && fYaw)
		{
			XMFLOAT3 off = mOffset;
			float distance = sqrtf((mOffset.x * mOffset.x) + (mOffset.z * mOffset.z));
			
			mPosition = m_Owner->GetPosition();
		
			XMFLOAT3 xmf3Up = m_Owner->GetUp();
			XMMATRIX xmmtxRotate = DirectX::XMMatrixRotationAxis(XMLoadFloat3(&xmf3Up), (fYaw));
			
			mLook = MathHelper::TransformNormal(mLook, xmmtxRotate);
			mUp = MathHelper::TransformNormal(mUp, xmmtxRotate);
			mRight = MathHelper::TransformNormal(mRight, xmmtxRotate);
			
			mPosition = MathHelper::Add(mPosition, mLook, distance);
			mPosition.y += mOffset.y;
		}
		if (m_Owner && fRoll)
		{
			XMFLOAT3 xmf3Look = m_Owner->GetLook();
			XMMATRIX xmmtxRotate = DirectX::XMMatrixRotationAxis(XMLoadFloat3(&xmf3Look), (fRoll));
			XMFLOAT3 xmPos = m_Owner->GetPosition();
		
			mPosition = MathHelper::Subtract(mPosition, xmPos);
			mPosition = MathHelper::TransformCoord(mPosition, xmmtxRotate);
			mPosition = MathHelper::Add(mPosition, m_Owner->GetPosition());
		
			mLook = MathHelper::TransformNormal(mLook, xmmtxRotate);
			mUp = MathHelper::TransformNormal(mUp, xmmtxRotate);
			mRight = MathHelper::TransformNormal(mRight, xmmtxRotate);
		}
		m_Owner->Rotate(0.f, fYaw, 0.f);
		break;
	}
	case CameraType::eThird:
	{
		if (m_Owner && fPitch)
		{
			XMFLOAT3 xmf3Right = mRight;
			XMMATRIX xmmtxRotate = DirectX::XMMatrixRotationAxis(XMLoadFloat3(&xmf3Right), fPitch);

			XMFLOAT3 calLook = MathHelper::TransformNormal(mLook, xmmtxRotate);
			XMFLOAT3 upVec = { 0.f, 1.f, 0.f };

			XMVECTOR projVec = XMVectorMultiply(XMVector3Dot(XMLoadFloat3(&calLook), XMLoadFloat3(&upVec)), XMLoadFloat3(&upVec));


			float cosValue = DirectX::XMVectorGetX(DirectX::XMVector3Dot(XMLoadFloat3(&calLook), projVec));
			float acosValue = 0.f;
			if (cosValue <= -1.f)
				acosValue = 3.141592;
			else if (cosValue > 1.f)
				acosValue = 0.f;
			else
				acosValue = acos(cosValue);
			float degree = XMConvertToDegrees(acosValue);

			float ccw = DirectX::XMVectorGetX(DirectX::XMVector3Dot(XMLoadFloat3(&mRight), DirectX::XMVector3Cross(XMLoadFloat3(&calLook), (projVec))));
			if (ccw > 0)	// ccw가 양수이면 방시계로 돌아야함
				degree = -degree;
			degree += 90.f;

			// cout << degree << endl;

			// pitch 각도 제한 두기
			if (degree < 90.f)
			{
				mLook = MathHelper::TransformNormal(mLook, xmmtxRotate);
				mUp = MathHelper::TransformNormal(mUp, xmmtxRotate);
				mRight = MathHelper::TransformNormal(mRight, xmmtxRotate);

				XMFLOAT3 r = { fPitch, 0.f, 0.f };
				XMVECTOR q = XMQuaternionRotationRollPitchYawFromVector(XMLoadFloat3(&r));

				XMStoreFloat3(&mPosition, XMVector3Rotate(XMLoadFloat3(&mPosition), q));
			}
			else if(degree > 170.f)
				degree = 170.f;
		}
		if (m_Owner && fYaw)
		{
			XMFLOAT3 xmf3Up = m_Owner->GetUp();
			XMMATRIX xmmtxRotate = DirectX::XMMatrixRotationAxis(XMLoadFloat3(&xmf3Up), (fYaw));
			mLook = MathHelper::TransformNormal(mLook, xmmtxRotate);
			mUp = MathHelper::TransformNormal(mUp, xmmtxRotate);
			mRight = MathHelper::TransformNormal(mRight, xmmtxRotate);
		}
		if (m_Owner && fRoll)
		{
			XMFLOAT3 xmf3Look = m_Owner->GetLook();
			XMMATRIX xmmtxRotate = DirectX::XMMatrixRotationAxis(XMLoadFloat3(&xmf3Look), (fRoll));
			XMFLOAT3 xmPos = m_Owner->GetPosition();

			mPosition = MathHelper::Subtract(mPosition, xmPos);
			mPosition = MathHelper::TransformCoord(mPosition, xmmtxRotate);
			mPosition = MathHelper::Add(mPosition, m_Owner->GetPosition());

			mLook = MathHelper::TransformNormal(mLook, xmmtxRotate);
			mUp = MathHelper::TransformNormal(mUp, xmmtxRotate);
			mRight = MathHelper::TransformNormal(mRight, xmmtxRotate);
		}
		break;
	}
	case CameraType::eFree:
	{
		mRotation.x += (fPitch);
		mRotation.y += (fYaw);
		mRotation.z += (fRoll);

		if (m_Owner && fPitch)
		{
			XMFLOAT3 xmf3Right = mRight;
			XMMATRIX xmmtxRotate = DirectX::XMMatrixRotationAxis(XMLoadFloat3(&xmf3Right), fPitch);
			mLook = MathHelper::TransformNormal(mLook, xmmtxRotate);
			mUp = MathHelper::TransformNormal(mUp, xmmtxRotate);
			mRight = MathHelper::TransformNormal(mRight, xmmtxRotate);
		}
		if (m_Owner && fYaw)
		{
			XMFLOAT3 xmf3Up = m_Owner->GetUp();
			XMMATRIX xmmtxRotate = DirectX::XMMatrixRotationAxis(XMLoadFloat3(&xmf3Up), (fYaw));
			mLook = MathHelper::TransformNormal(mLook, xmmtxRotate);
			mUp = MathHelper::TransformNormal(mUp, xmmtxRotate);
			mRight = MathHelper::TransformNormal(mRight, xmmtxRotate);
		}
		break;
	}
	}
	mViewDirty = true;
}

void Camera::Pitch(float angle)
{
	// Rotate up and look vector about the right vector.

	XMMATRIX R = DirectX::XMMatrixRotationAxis(XMLoadFloat3(&mRight), angle);

	XMStoreFloat3(&mUp, XMVector3TransformNormal(XMLoadFloat3(&mUp), R));
	XMStoreFloat3(&mLook, XMVector3TransformNormal(XMLoadFloat3(&mLook), R));

	mViewDirty = true;
}

void Camera::RotateY(float angle)
{
	// Rotate the basis vectors about the world y-axis.

	XMMATRIX R = DirectX::XMMatrixRotationY(angle);

	XMStoreFloat3(&mRight, XMVector3TransformNormal(XMLoadFloat3(&mRight), R));
	XMStoreFloat3(&mUp, XMVector3TransformNormal(XMLoadFloat3(&mUp), R));
	XMStoreFloat3(&mLook, XMVector3TransformNormal(XMLoadFloat3(&mLook), R));

	mViewDirty = true;
}

void Camera::UpdateViewMatrix()
{
	if (mViewDirty)
	{
		GenerateFrustum();

		XMVECTOR R = XMLoadFloat3(&mRight);
		XMVECTOR U = XMLoadFloat3(&mUp);
		XMVECTOR L = XMLoadFloat3(&mLook);
		XMVECTOR P = XMLoadFloat3(&mPosition);

		// Keep camera's axes orthogonal to each other and of unit length.
		L = XMVector3Normalize(L);
		U = XMVector3Normalize(XMVector3Cross(L, R));

		// U, L already ortho-normal, so no need to normalize cross product.
		R = XMVector3Cross(U, L);

		// Fill in the view matrix entries.
		float x = -XMVectorGetX(XMVector3Dot(P, R));
		float y = -XMVectorGetX(XMVector3Dot(P, U));
		float z = -XMVectorGetX(XMVector3Dot(P, L));

		XMStoreFloat3(&mRight, R);
		XMStoreFloat3(&mUp, U);
		XMStoreFloat3(&mLook, L);

		mView(0, 0) = mRight.x;
		mView(1, 0) = mRight.y;
		mView(2, 0) = mRight.z;
		mView(3, 0) = x;

		mView(0, 1) = mUp.x;
		mView(1, 1) = mUp.y;
		mView(2, 1) = mUp.z;
		mView(3, 1) = y;

		mView(0, 2) = mLook.x;
		mView(1, 2) = mLook.y;
		mView(2, 2) = mLook.z;
		mView(3, 2) = z;

		// 카메라 이동과 회전이 빈번하면 실수 연산의 오차 누적으로 카메라의 축들이 서로 직교하지 않을수있음
		// 카메라의 이동과 회전을 한 후 축들이 서로 직교하게 만들 필요가 있음
		mView(0, 3) = 0.0f;
		mView(1, 3) = 0.0f;
		mView(2, 3) = 0.0f;
		mView(3, 3) = 1.0f;

		mViewDirty = false;
	}
}

void Camera::GenerateFrustum()
{
	m_Frustum.CreateFromMatrix(m_Frustum, XMLoadFloat4x4(&mProj));
}

bool Camera::IsInFrustum(const XMMATRIX& invWorld, const BoundingBox& otherBounds)
{
	XMMATRIX view = XMLoadFloat4x4(&mView);
	XMMATRIX invView = XMMatrixInverse(&XMMatrixDeterminant(view), view);

	// View space to the object's local space.
	XMMATRIX viewToLocal = XMMatrixMultiply(invView, invWorld);

	// Transform the camera frustum from view space to the object's local space.
	BoundingFrustum localSpaceFrustum;
	m_Frustum.Transform(localSpaceFrustum, viewToLocal);

	// Perform the box/frustum intersection test in local space.
	return (localSpaceFrustum.Contains(otherBounds) != DirectX::DISJOINT);
}
