#include "pch.h"
#include "Character.h"
#include "Camera.h"
#include "ApplicationContext.h"
#include "AssertsReference.h"
#include "CommandContext.h"
#include "CommandCenter.h"
#include "netCore.h"
#include "Service.h"
#include "Map.h"

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

	//WeaponUpdate();
}

void Character::WeaponUpdate()
{
	Map* originMap = AppContext->m_Maps[m_MapName];

	//if (m_PlayerID == 0)
	//{
	//	GameObject* leftWeapon = AppContext->FindObject<GameObject>(OBJECT_MESH_STR_REVOLVER, std::to_string(1));
	//	leftWeapon->SetPosition(m_AnimationController->m_CopySkinnedModelInst->FinalTransforms[9]._41,
	//							m_AnimationController->m_CopySkinnedModelInst->FinalTransforms[9]._42,
	//							m_AnimationController->m_CopySkinnedModelInst->FinalTransforms[9]._43);
	//	leftWeapon->
	//	GameObject* RightWeapon = AppContext->FindObject<GameObject>(OBJECT_MESH_STR_REVOLVER, std::to_string(2));
	//	RightWeapon->SetPosition(m_AnimationController->m_CopySkinnedModelInst->FinalTransforms[19]._41,
	//							m_AnimationController->m_CopySkinnedModelInst->FinalTransforms[19]._42,
	//							m_AnimationController->m_CopySkinnedModelInst->FinalTransforms[19]._43);
	//}
	//else
	//{
	//	GameObject* leftWeapon = AppContext->FindObject<GameObject>(OBJECT_MESH_STR_REVOLVER, std::to_string(3));
	//	leftWeapon->SetPosition(m_AnimationController->m_CopySkinnedModelInst->FinalTransforms[9]._41,
	//		m_AnimationController->m_CopySkinnedModelInst->FinalTransforms[9]._42,
	//		m_AnimationController->m_CopySkinnedModelInst->FinalTransforms[9]._43);
	//	GameObject* RightWeapon = AppContext->FindObject<GameObject>(OBJECT_MESH_STR_REVOLVER, std::to_string(4));
	//	RightWeapon->SetPosition(m_AnimationController->m_CopySkinnedModelInst->FinalTransforms[19]._41,
	//		m_AnimationController->m_CopySkinnedModelInst->FinalTransforms[19]._42,
	//		m_AnimationController->m_CopySkinnedModelInst->FinalTransforms[19]._43);
	//}
}

void Character::SetMapName(std::string mapName)
{
	m_MapName = mapName;
}

void Character::SetCamera(Camera* myCamera, CameraType cameraType)
{
	m_MyCamera = myCamera;

	SetCamera(cameraType);
}

void Character::SetCamera(CameraType cameraType)
{
	m_MyCamera->SetCamera(cameraType, this);

	switch (cameraType)
	{
	case CameraType::eThird:
		m_MyCamera->SetTimeLag(0.0f);
		
		m_MyCamera->SetOffset({ m_Bounds.Extents.x + m_Bounds.Extents.x * 0.5f, m_Bounds.Extents.y * 1.2f, -m_Bounds.Extents.x - m_Bounds.Extents.y * 2.f });
		
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

void Character::SetParts(CharacterParts* parts)
{
	//m_Parts[parts->m_PartsName] = parts;
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
		if (dwDirection == 15) // 모든 키가 눌렸을 경우
			return false;
		if ((dwDirection ^ MoveBit) == 0 || (dwDirection ^ MoveBit) == 15)	// ws가 동시에 눌렸을 경우
			return false;
		if ((dwDirection ^ strafeBit) == 0 || (dwDirection ^ strafeBit) == 15)	// ad가 동시에 눌렸을 경우
			return false;

		if (dwDirection & DIR_FORWARD)			direction = direction + look;
		if (dwDirection & DIR_BACKWARD)			direction = direction - look;

		if (dwDirection & DIR_RIGHT)			direction = direction + right;
		if (dwDirection & DIR_LEFT)				direction = direction - right;

		XMFLOAT3 dir;
		DirectX::XMStoreFloat3(&dir, direction);
		dir.y = 0.f;
		direction = DirectX::XMVector3Normalize(XMLoadFloat3(&dir));
		
		XMFLOAT3 cLook = m_Look;
		cLook.y = 0.f;
		cLook = MathHelper::Normalize(cLook);
		
		float cosValue = DirectX::XMVectorGetX(DirectX::XMVector3Dot(direction, XMLoadFloat3(&cLook)));
		
		XMFLOAT3 cUp = m_Up;
		cUp = MathHelper::Normalize(cUp);
		
		float ccw = DirectX::XMVectorGetX(DirectX::XMVector3Dot(XMLoadFloat3(&cUp), DirectX::XMVector3Cross(direction, XMLoadFloat3(&cLook))));
		
		float acosValue = 0.f;
		
		acosValue = acos(cosValue);
		
		float degree = XMConvertToDegrees(acosValue);
		
		if (ccw > 0)	// ccw가 양수이면 반시계로 돌아야함
			degree = -degree;
		
		bool isChange = false;
		
		if (CommandCenter::GetApp()->m_StartAttackAnim == false)
		{
			if (fabs(degree) > 1.f)
			{
				Rotate(0.f, XMConvertToRadians(degree), 0.f);
				isChange = true;
			}
			Move(DIR_FORWARD, fDistance, true);
			degree = 0.f;
		}
		else
			Move(dwDirection, fDistance, true);
		return isChange;
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
	// Attack_Box
	const std::map<std::string, UINT>& info = AppContext->m_RItemsMap[OBJECT_MESH_STR_ATTACK_BOX]->GetInstanceKeyMap();
	for (auto& i : info) AppContext->m_RItemsVec[i.second]->SetPosition(posX, posY + 90.f, posZ);

	XMFLOAT3 prePos = { m_World._41, m_World._42, m_World._43 };

	m_World._41 = posX;
	m_World._42 = posY;
	m_World._43 = posZ;

	XMFLOAT3 shift = { posX - prePos.x, posY - prePos.y, posZ - prePos.z };
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

void Character::SetLookToCameraLook()
{
	XMVECTOR direction = m_MyCamera->GetLook();

	XMFLOAT3 dir;
	DirectX::XMStoreFloat3(&dir, direction);
	dir.y = 0.f;
	direction = DirectX::XMVector3Normalize(XMLoadFloat3(&dir));

	XMFLOAT3 cLook = m_Look;
	cLook.y = 0.f;
	cLook = MathHelper::Normalize(cLook);

	float cosValue = DirectX::XMVectorGetX(DirectX::XMVector3Dot(direction, XMLoadFloat3(&cLook)));

	XMFLOAT3 cUp = m_Up;
	cUp = MathHelper::Normalize(cUp);

	float ccw = DirectX::XMVectorGetX(DirectX::XMVector3Dot(XMLoadFloat3(&cUp), DirectX::XMVector3Cross(direction, XMLoadFloat3(&cLook))));

	float acosValue = 0.f;

	acosValue = acos(cosValue);

	float degree = XMConvertToDegrees(acosValue);

	if (ccw > 0)	// ccw가 양수이면 반시계로 돌아야함
		degree = -degree;
	degree *= 0.9f;

	if (fabs(degree) > 1.f)
		Rotate(0.f, XMConvertToRadians(degree), 0.f);
	degree = 0.f;
}

void Character::Move(DWORD dwDirection, float fDistance, bool bUpdateVelocity)
{
	XMFLOAT3 look = GetLook();
	XMFLOAT3 right = GetRight();
	XMFLOAT3 up = GetUp();
	
	// 대각선 체크
	float tfDistance = fDistance;
	int checkFB = 0, checkRL = 0;
	if (dwDirection & DIR_FORWARD) checkFB++;
	if (dwDirection & DIR_BACKWARD) checkFB--;
	if (dwDirection & DIR_RIGHT) checkRL++;
	if (dwDirection & DIR_LEFT) checkRL--;
	if (checkFB != 0 && checkRL != 0)
		tfDistance = tfDistance / sqrt(2);

	if (dwDirection)
	{
		XMFLOAT3 xmf3Shift = XMFLOAT3(0, 0, 0);
		if (dwDirection & DIR_FORWARD) xmf3Shift = MathHelper::Add(xmf3Shift, look, tfDistance);
		if (dwDirection & DIR_BACKWARD) xmf3Shift = MathHelper::Add(xmf3Shift, look, -tfDistance);
		if (dwDirection & DIR_RIGHT) xmf3Shift = MathHelper::Add(xmf3Shift, right, tfDistance);
		if (dwDirection & DIR_LEFT) xmf3Shift = MathHelper::Add(xmf3Shift, right, -tfDistance);
		if (dwDirection & DIR_UP) xmf3Shift = MathHelper::Add(xmf3Shift, up, tfDistance);
		if (dwDirection & DIR_DOWN) xmf3Shift = MathHelper::Add(xmf3Shift, up, -tfDistance);

		Move(xmf3Shift, bUpdateVelocity);
	}
}

void Character::Move(const XMFLOAT3& xmf3Shift, bool bVelocity)
{
	XMFLOAT3 pos = MathHelper::Add(GetPosition(), xmf3Shift);

#ifdef DEBUG_CLIENT
	SetPosition(pos.x, pos.y, pos.z);
#elif DEBUG_SERVER
	SetPosition(pos.x, pos.y, pos.z);
	Service::GetApp()->AddEvent(EVENT_GAME_MAKE_MOVE, 1, GetPosition());
#endif
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

	XMFLOAT3 t_look{ m_Look };
	Service::GetApp()->AddEvent(EVENT_GAME_MAKE_MOUSE, 1, t_look);
}