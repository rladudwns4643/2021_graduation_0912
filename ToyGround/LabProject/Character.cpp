#include "pch.h"
#include "Character.h"
#include "Camera.h"
#include "ApplicationContext.h"
#include "AssertsReference.h"
#include "CommandContext.h"

Character::Character(std::string type, std::string id) :
	GameObject(type, id),
	m_PlayerController(nullptr),
	m_MyCamera(nullptr)
{
	m_Position = { 0,0,0 };
	m_Right = { 1,0,0 };
	m_Up = { 0,1,0 };
	m_Look = { 0,0,1 };
}

Character::~Character()
{
}

void Character::InitializeTransform()
{
	GameObject::InitializeTransform();

	m_Position = { 0,0,0 };
	m_Right = { 1,0,0 };
	m_Up = { 0,1,0 };
	m_Look = { 0,0,1 };
}

void Character::Update(const float deltaT)
{
	if (m_PlayerController)
		m_PlayerController->Update(deltaT);

	m_AnimationController->Update(deltaT);

	// 카메라 변환
	if (m_MyCamera != NULL)
	{
		CameraType cType = m_MyCamera->GetCameraType();

		if (cType == CameraType::eThird)
		{
			XMFLOAT3 target = GetPosition();
			target.y += 200;
			m_MyCamera->Update(target, deltaT);
			//CameraUpdate(deltaT);
		}
		else if (cType == CameraType::eFirst)
		{
			XMFLOAT3 target = GetPosition();
			// target.y += 200;
			m_MyCamera->Update(target, deltaT);
			//CameraUpdate(deltaT);
		}
	}
}

void Character::SetCamera(Camera* myCamera, CameraType cameraType)
{
	m_MyCamera = myCamera;

	SetCamera(cameraType);
}

void Character::SetCamera(CameraType cameraType)
{
//	if (m_MyCamera == nullptr) return;
	m_MyCamera->SetCamera(cameraType, this);

	switch (cameraType)
	{
	case CameraType::eFirst:
	{
		m_MyCamera->SetTimeLag(0.f);
		//m_MyCamera->SetOffset(XMFLOAT3(0.0f, STD_CUBE_SIZE * 0.7, 0.0f));
		m_MyCamera->SetOffset(XMFLOAT3(0.0f, 160.f, 20.0f));

		XMVECTOR direction = {};
		XMFLOAT3 dir;
		DirectX::XMStoreFloat3(&dir, m_MyCamera->GetLook());
		dir.y = 0.f;
		direction = DirectX::XMVector3Normalize(XMLoadFloat3(&dir));

		XMFLOAT3 cLook = m_Look;
		cLook.y = 0.f;
		cLook = MathHelper::Normalize(cLook);

		float cosValue = DirectX::XMVectorGetX(DirectX::XMVector3Dot(direction, XMLoadFloat3(&cLook)));

		XMFLOAT3 cUp = m_Up;
		cUp.y = 0.f;
		//cUp = MathHelper::Normalize(cUp);

		float ccw = DirectX::XMVectorGetX(DirectX::XMVector3Dot(XMLoadFloat3(&cUp), DirectX::XMVector3Cross(direction, XMLoadFloat3(&cLook))));

		float acosValue = 0.f;

		acosValue = acos(cosValue);

		float degree = XMConvertToDegrees(acosValue);

		if (ccw > 0)	// ccw가 양수이면 반시계로 돌아야함
			degree = -degree;
		degree *= 0.9f;

		if (fabs(degree) > 1.f)
			Rotate(0.f, XMConvertToRadians(degree), 0.f);

		//m_MyCamera->SetRotation({ 0.f, 0.f, 0.f });
		//m_MyCamera->Rotate(InputHandler::g_MouseChangebleY, InputHandler::g_MouseChangebleX, 0.f);
		break;
	}
	case CameraType::eThird:
		m_MyCamera->SetTimeLag(0.0f);
//		m_MyCamera->SetOffset(XMFLOAT3(0.0f, STD_CUBE_SIZE * 1.5f, -STD_CUBE_SIZE * 4.5f));
		if (m_Bounds.Extents.x > 100.f)
			m_MyCamera->SetOffset(XMFLOAT3(m_Bounds.Extents.x * 0.35f, m_Bounds.Extents.y * 1.3f, -m_Bounds.Extents.x * 2.3f - m_Bounds.Extents.y * 1.5f));
		else
			m_MyCamera->SetOffset({ m_Bounds.Extents.x + m_Bounds.Extents.x * 0.05f, m_Bounds.Extents.y * 2.f, -m_Bounds.Extents.x * 2 * 2.5f - m_Bounds.Extents.y * 2.f });
		
		m_MyCamera->SetRotation({ 0.f, 0.f, 0.f });
		break;
	case CameraType::eFree:
		m_MyCamera->SetTimeLag(0.f);
		m_MyCamera->SetOffset(XMFLOAT3(0.0f, 0.0f, 0.0f));
		break;
	}
}

void Character::SetController()
{
	// 커서지우고
	m_PlayerController = make_unique<PlayerController>(this);
}

bool Character::Move(DWORD dwDirection, float fDistance)
{
	XMVECTOR look = m_MyCamera->GetLook();
	XMVECTOR right = m_MyCamera->GetRight();

	XMVECTOR direction = {};
	DWORD MoveBit = 3;
	DWORD strafeBit = 12;

	if (dwDirection)
	{
		//if (dwDirection == 15) // 모든 키가 눌렸을 경우
		//	return false;
		//if ((dwDirection ^ MoveBit) == 0 || (dwDirection ^ MoveBit) == 15)	// ws가 동시에 눌렸을 경우
		//	return false;
		//if ((dwDirection ^ strafeBit) == 0 || (dwDirection ^ strafeBit) == 15)	// ad가 동시에 눌렸을 경우
		//	return false;
		//
		//
		//if (dwDirection & DIR_FORWARD)			direction = direction + look;
		//if (dwDirection & DIR_BACKWARD)			direction = direction - look;
		//
		//if (dwDirection & DIR_RIGHT)			direction = direction + right;
		//if (dwDirection & DIR_LEFT)				direction = direction - right;
		//
		//XMFLOAT3 dir;
		//DirectX::XMStoreFloat3(&dir, direction);
		//dir.y = 0.f;
		//direction = DirectX::XMVector3Normalize(XMLoadFloat3(&dir));
		//
		//XMFLOAT3 cLook = m_Look;
		//cLook.y = 0.f;
		//cLook = MathHelper::Normalize(cLook);
		//
		//float cosValue = DirectX::XMVectorGetX(DirectX::XMVector3Dot(direction, XMLoadFloat3(&cLook)));
		//
		//XMFLOAT3 cUp = m_Up;
		////cUp.y = 0.f;
		//cUp = MathHelper::Normalize(cUp);
		//
		//float ccw = DirectX::XMVectorGetX(DirectX::XMVector3Dot(XMLoadFloat3(&cUp), DirectX::XMVector3Cross(direction, XMLoadFloat3(&cLook))));
		//
		//float acosValue = 0.f;
		//
		//acosValue = acos(cosValue);
		//
		//float degree = XMConvertToDegrees(acosValue);
		//
		//if (ccw > 0)	// ccw가 양수이면 반시계로 돌아야함
		//	degree = -degree;
		//degree *= 0.9f;
		//
		//bool isChange = false;
		//
		//if (m_MyCamera->GetCameraType() == CameraType::eThird)
		//{
		//	if (fabs(degree) > 1.f)
		//	{
		//		Rotate(0.f, XMConvertToRadians(degree), 0.f);
		//		isChange = true;
		//	}
		//	else
		//		Move(DIR_FORWARD, fDistance, true);
		//	degree = 0.f;
		//}
		//else
		//{
		//	Move(dwDirection, fDistance, true);
		//}
		Move(dwDirection, fDistance, true);
		return true;
		//return isChange;
	}
	return false;
}

bool Character::SetMesh(std::string meshName, std::string submeshName)
{
	if (GameObject::SetMesh(meshName, submeshName))
	{
		m_CharacterGeo = AssertsReference::GetApp()->m_GeometryMesh[meshName].get();
		m_CharacterPrimitiveType = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
		m_CharacterIndexCount = m_Geo->DrawArgs[submeshName].IndexCount;
		m_CharacterStartIndexLocation = m_Geo->DrawArgs[submeshName].StartIndexLocation;
		m_CharacterBaseVertexLocation = m_Geo->DrawArgs[submeshName].BaseVertexLocation;
		m_CharacterBounds = m_Geo->DrawArgs[meshName].Bounds;

		return true;
	}

	return false;
}

void Character::SetMaterial(int materialIndex)
{
	if (materialIndex == -1) return;

	m_MaterialIndex = materialIndex;
	m_CharacterMaterialIndex = materialIndex;
}

void Character::SetPosition(float posX, float posY, float posZ)
{
	XMFLOAT3 prePos = { m_World._41, m_World._42, m_World._43 };

	m_World._41 = posX;
	m_World._42 = posY;
	m_World._43 = posZ;

	XMFLOAT3 shift = { posX - prePos.x, posY - prePos.y , posZ - prePos.z };

	// FreeType일때
	if (m_MyCamera) m_MyCamera->Move(shift);
}

void Character::SetPosition(DirectX::XMFLOAT3 xmPos)
{
	SetPosition(xmPos.x, xmPos.y, xmPos.z);
}

void Character::SetAnimationController(SkinnedModelInstance* skinnedModelInst)
{
	if (!skinnedModelInst) return;

	m_SkinnedModelInst = skinnedModelInst;
	m_AnimationController = std::make_unique<AnimationController>(this, m_SkinnedModelInst);
}

void Character::SetAnimationPlayerState(AnimationController::PlayerState playerState)
{
	m_AnimationController->m_PlayerState = playerState;
}

void Character::SetAnimationKeyState(AnimationController::PlayerState keyState)
{
	m_AnimationController->m_KeyState = keyState;
}

void Character::Move(DWORD dwDirection, float fDistance, bool bUpdateVelocity)
{
	XMFLOAT3 look = GetLook();
	XMFLOAT3 right = GetRight();
	XMFLOAT3 up = GetUp();
	if (dwDirection)
	{
		XMFLOAT3 xmf3Shift = XMFLOAT3(0, 0, 0);
		if (dwDirection & DIR_FORWARD) xmf3Shift = MathHelper::Add(xmf3Shift, look, fDistance);
		if (dwDirection & DIR_BACKWARD) xmf3Shift = MathHelper::Add(xmf3Shift, look, -fDistance);
		if (dwDirection & DIR_RIGHT) xmf3Shift = MathHelper::Add(xmf3Shift, right, fDistance);
		if (dwDirection & DIR_LEFT) xmf3Shift = MathHelper::Add(xmf3Shift, right, -fDistance);
		if (dwDirection & DIR_UP) xmf3Shift = MathHelper::Add(xmf3Shift, up, fDistance);
		if (dwDirection & DIR_DOWN) xmf3Shift = MathHelper::Add(xmf3Shift, up, -fDistance);

		Move(xmf3Shift, bUpdateVelocity);
	}
}

void Character::Move(const XMFLOAT3& xmf3Shift, bool bVelocity)
{
	XMFLOAT3 pos = MathHelper::Add(GetPosition(), xmf3Shift);
	SetPosition(pos.x, pos.y, pos.z);
	if (m_MyCamera) m_MyCamera->Move(xmf3Shift);
}

void Character::Rotate(float pitch, float yaw, float roll)
{
	if (pitch)
	{
		XMFLOAT3 xmf3Right = this->m_Right;
		XMMATRIX xmmtxRotate = XMMatrixRotationAxis(XMLoadFloat3(&xmf3Right), (pitch));
		m_Look = MathHelper::TransformNormal(m_Look, xmmtxRotate);
		m_Right = MathHelper::TransformNormal(m_Right, xmmtxRotate);
		m_Up = MathHelper::TransformNormal(m_Up, xmmtxRotate);
	}
	if (yaw)
	{
		XMFLOAT3 xmf3Up = this->m_Up;
		XMMATRIX xmmtxRotate = XMMatrixRotationAxis(XMLoadFloat3(&xmf3Up), (yaw));
		m_Look = MathHelper::TransformNormal(m_Look, xmmtxRotate);
		m_Right = MathHelper::TransformNormal(m_Right, xmmtxRotate);

	}

	m_Look = MathHelper::Normalize(m_Look);
	m_Right = MathHelper::CrossProduct(m_Up, m_Look, true);
	m_Up = MathHelper::CrossProduct(m_Look, m_Right, true);

	m_World._11 = m_Right.x; m_World._12 = m_Right.y; m_World._13 = m_Right.z;
	m_World._31 = m_Look.x; m_World._32 = m_Look.y; m_World._33 = m_Look.z;
}

bool Character::RayMapTriangleIntersect(XMVECTOR orig, XMVECTOR dir, XMVECTOR v0, XMVECTOR v1, XMVECTOR v2, XMVECTOR& P)
{
	XMVECTOR v0v1 = DirectX::XMVectorSubtract(v1, v0);
	XMVECTOR v0v2 = DirectX::XMVectorSubtract(v2, v0);

	XMVECTOR N = DirectX::XMVector3Cross(v0v1, v0v2);	// 시계 방향 순서로
	// float area2 = DirectX::XMVectorGetX(DirectX::XMVector3Length(N));

	// Step1 : finding P

	// check if ray and plane are parallel?
	float NdotrayDirection = DirectX::XMVectorGetX(DirectX::XMVector3Dot(N, dir));
	if (fabs(NdotrayDirection) < FLT_EPSILON)
		return false;

	// compute d parameter using equation 2
	// Ax + By + Cz + D = 0
	// (A, B, C) --> plane Normal
	float d = DirectX::XMVectorGetX(DirectX::XMVector3Dot(N, v0));

	// compute t 
	float t = (DirectX::XMVectorGetX(DirectX::XMVector3Dot(N, orig)) + d) / NdotrayDirection;
	// check if the triangle is in behind the ray
	if (t < 0)
		return false;

	// compute the intersection point using equation 1
	P = orig + t * dir;	// Local Pos

	// Step 2: inside-outside test
	XMVECTOR C;

	// edge 0
	XMVECTOR e0 = v1 - v0;
	XMVECTOR vp0 = P - v0;
	C = XMVector3Cross(vp0, e0);
	float tmp = XMVectorGetX(XMVector3Dot(N, C));
	if (XMVectorGetX(XMVector3Dot(N, C)) > 0)
		return false;

	// edge 1
	XMVECTOR e1 = v2 - v1;
	XMVECTOR vp1 = P - v1;
	C = XMVector3Cross(vp1, e1);
	tmp = XMVectorGetX(XMVector3Dot(N, C));
	if (XMVectorGetX(XMVector3Dot(N, C)) > 0)
		return false;

	// edge 2
	XMVECTOR e2 = v0 - v2;
	XMVECTOR vp2 = P - v2;
	C = XMVector3Cross(vp2, e2);
	tmp = XMVectorGetX(XMVector3Dot(N, C));
	if (XMVectorGetX(XMVector3Dot(N, C)) > 0)
		return false;


	return true;

}

bool Character::RayObjTriangleIntersect(XMVECTOR orig, XMVECTOR dir, XMVECTOR v0, XMVECTOR v1, XMVECTOR v2, XMVECTOR& P)
{
	XMVECTOR v0v1 = DirectX::XMVectorSubtract(v1, v0);
	XMVECTOR v0v2 = DirectX::XMVectorSubtract(v2, v0);

	XMVECTOR N = DirectX::XMVector3Cross(v0v1, v0v2);	// 시계 방향 순서로
	// float area2 = DirectX::XMVectorGetX(DirectX::XMVector3Length(N));

	// Step1 : finding P

	// check if ray and plane are parallel?
	float NdotrayDirection = DirectX::XMVectorGetX(DirectX::XMVector3Dot(N, dir));
	if (fabs(NdotrayDirection) < FLT_EPSILON)
		return false;

	// compute d parameter using equation 2
	// Ax + By + Cz + D = 0
	// (A, B, C) --> plane Normal
	float d = -DirectX::XMVectorGetX(DirectX::XMVector3Dot(N, v0));

	// compute t 
	float t = -(DirectX::XMVectorGetX(DirectX::XMVector3Dot(N, orig)) + d) / NdotrayDirection;
	// check if the triangle is in behind the ray
	// if (t < 0)
	// 	return false;

	// compute the intersection point using equation 1
	P = orig + t * dir;	// Local Pos

	// Step 2: inside-outside test
	XMVECTOR C;

	// edge 0
	XMVECTOR e0 = v1 - v0;
	XMVECTOR vp0 = P - v0;
	C = XMVector3Cross(vp0, e0);
	float tmp = XMVectorGetX(XMVector3Dot(N, C));
	if (XMVectorGetX(XMVector3Dot(N, C)) > 0)
		return false;

	// edge 1
	XMVECTOR e1 = v2 - v1;
	XMVECTOR vp1 = P - v1;
	C = XMVector3Cross(vp1, e1);
	tmp = XMVectorGetX(XMVector3Dot(N, C));
	if (XMVectorGetX(XMVector3Dot(N, C)) > 0)
		return false;

	// edge 2
	XMVECTOR e2 = v0 - v2;
	XMVECTOR vp2 = P - v2;
	C = XMVector3Cross(vp2, e2);
	tmp = XMVectorGetX(XMVector3Dot(N, C));
	if (XMVectorGetX(XMVector3Dot(N, C)) > 0)
		return false;


	return true;
}