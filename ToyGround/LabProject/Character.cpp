#include "pch.h"
#include "Character.h"
#include "Camera.h"
#include "ApplicationContext.h"
#include "AssertsReference.h"
#include "CommandContext.h"
#include "EnemyCommandCenter.h"
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

	m_jumpCount = 1;
	m_isGround = true;
	m_jumpForce.x = 0.f;
	m_jumpForce.y = 0.f;
	m_jumpForce.z = 0.f;

	m_isSkillOn = false;
	m_skillGauge = 0;
	m_attackGauge = MAX_ATTACKGAUGE;
	m_hp = MAX_HP;

	m_tempHp = m_hp;
	m_tempAttackGauge = m_attackGauge;
	m_tempSkillGauge = m_skillGauge;

	SetIndexPos(m_Position);
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

bool Character::ReleaseTransform()
{
	m_Geo = m_CharacterGeo;
	m_IndexCount = m_CharacterIndexCount;
	m_StartIndexLocation = m_CharacterStartIndexLocation;
	m_BaseVertexLocation = m_CharacterBaseVertexLocation;
	m_PrimitiveType = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
	m_Bounds = m_CharacterBounds;
	m_MaterialIndex = m_CharacterMaterialIndex;

	return true;
}

void Character::Update(const float deltaT)
{
	if (m_PlayerID == 100)
	{
		m_AnimationController->Update(deltaT);
		return;
	}

#ifdef DEBUG_CLIENT
	UpdateStateUI();

	m_AnimationController->Update(deltaT);

	if (m_PlayerController)
		m_PlayerController->Update(deltaT);
		
	Falling();

#elif DEBUG_SERVER
	m_AnimationController->Update(deltaT);

	if (NetCore::GetApp()->GetBattleID() == m_PlayerID)
	{
		UpdateStateUI();
		if (m_PlayerController)
			m_PlayerController->Update(deltaT);
		Falling();
	}
#endif

	//cout << m_PlayerID << ": " << m_AnimationController->m_KeyState << endl;
//	WeaponUpdate();
}

void Character::WeaponUpdate()
{
	//f (m_PlayerID == 0)
	//
	//	GameObject* leftWeapon = AppContext->FindObject<GameObject>(OBJECT_MESH_STR_REVOLVER, std::to_string(1));
	//	XMFLOAT3 fprePos = GetPosition();
	//	XMFLOAT3 fPos = MathHelper::TransformCoord(fprePos, m_AnimationController->m_CopySkinnedModelInst->FinalTransforms[test]);
	//	//
	//	//XMMATRIX xmPreWorld = XMLoadFloat4x4(&m_World);
	//	//XMMATRIX xmFinalTransform = XMLoadFloat4x4(&m_AnimationController->m_CopySkinnedModelInst->FinalTransforms[test]);
	//	//XMMATRIX xmWorld = XMMatrixMultiply(xmFinalTransform, xmPreWorld);
	//	//
	//	DirectX::XM
	//	cout << test << endl;
	//	test++;
	//	test %= 39;
	//	//XMFLOAT4X4 wmfWorld;
	//	//XMStoreFloat4x4(&wmfWorld, XMMatrixTranspose(xmWorld));
	//	leftWeapon->SetPosition(fPos);
	//

	//GameObject* RightWeapon = AppContext->FindObject<GameObject>(OBJECT_MESH_STR_REVOLVER, std::to_string(2));
	//RightWeapon->SetPosition(m_AnimationController->m_CopySkinnedModelInst->FinalTransforms[19]._41,
	//	m_AnimationController->m_CopySkinnedModelInst->FinalTransforms[19]._42,
	//	m_AnimationController->m_CopySkinnedModelInst->FinalTransforms[19]._43);
}

void Character::UpdateStateUI()
{
	if (m_attackGauge < MAX_ATTACKGAUGE)
	{
		m_attackGauge += 3;
		if (m_attackGauge >= MAX_ATTACKGAUGE)
			m_attackGauge = MAX_ATTACKGAUGE;
	}

	if (m_tempHp < m_hp)
	{
		m_tempHp += 20;
		if (m_tempHp >= m_hp)
			m_tempHp = m_hp;
		AppContext->UpdateStateUI2D(OBJECT_TYPE_UI2D, OBJECT_NAME_GAMEPLAY_HEALTH, m_tempHp);
	}
	else if (m_tempHp > m_hp)
	{
		m_tempHp -= 20;
		if (m_tempHp <= m_hp)
			m_tempHp = m_hp;
		AppContext->UpdateStateUI2D(OBJECT_TYPE_UI2D, OBJECT_NAME_GAMEPLAY_HEALTH, m_tempHp);
	}

	if (m_tempAttackGauge < m_attackGauge)
	{
		m_tempAttackGauge += 5;
		if (m_tempAttackGauge >= m_attackGauge)
			m_tempAttackGauge = m_attackGauge;
		AppContext->UpdateStateUI2D(OBJECT_TYPE_UI2D, OBJECT_NAME_GAMEPLAY_ATTACK_GAUGE, m_tempAttackGauge);
	}
	else if (m_tempAttackGauge > m_attackGauge)
	{
		m_tempAttackGauge = m_attackGauge;
		AppContext->UpdateStateUI2D(OBJECT_TYPE_UI2D, OBJECT_NAME_GAMEPLAY_ATTACK_GAUGE, m_tempAttackGauge);
	}

	if (m_tempSkillGauge < m_skillGauge)
	{
		m_tempSkillGauge += 2;
		if (m_tempSkillGauge > m_skillGauge)
			m_tempSkillGauge = m_skillGauge;
		AppContext->UpdateStateUI2D(OBJECT_TYPE_UI2D, OBJECT_NAME_GAMEPLAY_SKILL_GAUGE, m_tempSkillGauge);
	}
	else if (m_tempSkillGauge > m_skillGauge)
	{
		m_tempSkillGauge = m_skillGauge;
		AppContext->UpdateStateUI2D(OBJECT_TYPE_UI2D, OBJECT_NAME_GAMEPLAY_SKILL_GAUGE, m_tempSkillGauge);
	}
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
		m_MyCamera->SetOffset({ m_Bounds.Extents.x + m_Bounds.Extents.x * 0.8f, m_Bounds.Extents.y * 1.2f, -m_Bounds.Extents.x - m_Bounds.Extents.y * 2.f });
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

#ifdef DEBUG_CLIENT
		if (m_PlayerID == 1)
		{
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
		}
		else
		{
			if (EnemyCommandCenter::GetApp()->m_StartAttackAnim == false)
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
		}


#elif DEBUG_SERVER
		if (m_PlayerID == NetCore::GetApp()->GetBattleID() == m_PlayerID)
		{
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
	}
		else
		{
			if (EnemyCommandCenter::GetApp()->m_StartAttackAnim == false)
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
		}
#endif
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
	SetIndexPos(posX, posY, posZ);
	// Attack_Box
	if (m_MyCamera)
	{
		const std::map<std::string, UINT>& info = AppContext->m_RItemsMap[OBJECT_MESH_STR_ATTACK_BOX]->GetInstanceKeyMap();
		for (auto& i : info) {
			AppContext->m_RItemsVec[i.second]->SetPosition(posX, posY + 90.f, posZ);
		}
	}

	XMFLOAT3 prePos = { m_World._41, m_World._42, m_World._43 };
	
	if (m_PlayerID == NetCore::GetApp()->GetBattleID()) {
		if (MathHelper::Distance(m_packetPosition, GetPosition()) > 10.f) {
			m_packetPosition = GetPosition();
			Service::GetApp()->AddEvent(EVENT_GAME_MAKE_MOVE, 2, m_packetPosition, static_cast<int>(m_AnimationController->m_PlayerState));
		}
	}

	m_World._41 = posX;
	m_World._42 = posY;
	m_World._43 = posZ;

	XMFLOAT3 shift = { posX - prePos.x, posY - prePos.y, posZ - prePos.z };

	if (m_MyCamera) m_MyCamera->Move(shift);

}

void Character::SetPosition(DirectX::XMFLOAT3 xmPos)
{
	SetPosition(xmPos.x, xmPos.y, xmPos.z);
}

void Character::SetIndexPos(float posX, float posY, float posZ)
{
	m_IndexPosX = (int)((posX + (STD_CUBE_SIZE / 2)) / STD_CUBE_SIZE + (MAP_WIDTH_BLOCK_NUM / 2));
	m_IndexPosY = (int)(posY / STD_CUBE_SIZE) + 1;
	m_IndexPosZ = (int)((posZ + (STD_CUBE_SIZE / 2)) / STD_CUBE_SIZE + (MAP_DEPTH_BLOCK_NUM / 2));

	// cout << "index posY: " << m_IndexPosY << endl;
}

void Character::SetIndexPos(DirectX::XMFLOAT3 xmPos)
{
	SetIndexPos(xmPos.x, xmPos.y, xmPos.z);
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
	m_attackDirection = dir;

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
	
	// 물 위 인지 체크
	if (OnWater())
		tfDistance *= 0.5;

	m_tSpeed = tfDistance;
	//cout << "Speed: " << m_tSpeed << endl;

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
	XMFLOAT3 prePos = GetPosition();
	XMFLOAT3 pos = MathHelper::Add(GetPosition(), xmf3Shift);

	//cout << "x: " << prePos.x - pos.x << ", z: " << prePos.z - pos.z << endl;

	// 벽충돌 계산
	if (pos.x >= (MAP_WIDTH_BLOCK_NUM / 2) * STD_CUBE_SIZE)
		pos.x = (MAP_WIDTH_BLOCK_NUM / 2) * STD_CUBE_SIZE;
	if (pos.x <= -((MAP_WIDTH_BLOCK_NUM / 2) * STD_CUBE_SIZE))
		pos.x = -((MAP_WIDTH_BLOCK_NUM / 2) * STD_CUBE_SIZE);
	if (pos.z >= (MAP_DEPTH_BLOCK_NUM / 2) * STD_CUBE_SIZE)
		pos.z = (MAP_DEPTH_BLOCK_NUM / 2) * STD_CUBE_SIZE;
	if (pos.z <= -((MAP_DEPTH_BLOCK_NUM / 2) * STD_CUBE_SIZE))
		pos.z = -((MAP_DEPTH_BLOCK_NUM / 2) * STD_CUBE_SIZE);

	int preIndexPosX = m_IndexPosX;
	int preIndexPosY = m_IndexPosY;
	int preIndexPosZ = m_IndexPosZ;


	//if (preIndexPosX != m_IndexPosX || preIndexPosY != m_IndexPosY || preIndexPosZ != m_IndexPosZ)
	//{
	//		cout << "X: " << m_IndexPosX << " Y: " << m_IndexPosY << " Z: " << m_IndexPosZ << endl;
	//}

	// x, z 충돌검사
	SetIndexPos(pos);
	bool checkColl = false;
	Map* originMap = AppContext->m_Maps[m_MapName];
	for (auto& p : originMap->mapInfoVector)
	{
		for (int i = 0; i < 81; i += 3)
		{
			int aX = m_IndexPosX + shiftArr[i];
			int aY = m_IndexPosY + shiftArr[i + 1];
			int aZ = m_IndexPosZ + shiftArr[i + 2];

			if (aX < 0 || aX >= MAP_WIDTH_BLOCK_NUM
				|| aY <= 0 || aY >= MAP_HEIGHT_BLOCK_NUM
				|| aZ < 0 || aZ >= MAP_DEPTH_BLOCK_NUM)
				continue;

			if (p.typeID == AppContext->m_MapArray[aY][aZ][aX] && p.colWithChar)
			{
				GameObject* obj = AppContext->FindObject<GameObject>(p.meshName, std::to_string(p.typeID));

				XMFLOAT3 objPos = obj->GetPosition();
				XMFLOAT3 objMin(objPos.x + obj->m_Bounds.Center.x - (obj->m_Bounds.Extents.x / 2),
					objPos.y + obj->m_Bounds.Center.y,
					objPos.z + obj->m_Bounds.Center.z - (obj->m_Bounds.Extents.z / 2));
				XMFLOAT3 playerMin(pos.x + m_Bounds.Center.x - (m_Bounds.Extents.x / 2),
					pos.y + m_Bounds.Center.y,
					pos.z + m_Bounds.Center.z - (m_Bounds.Extents.z / 2));
				XMFLOAT3 objMax(objPos.x + obj->m_Bounds.Center.x + (obj->m_Bounds.Extents.x / 2),
					objPos.y + obj->m_Bounds.Center.y + obj->m_Bounds.Extents.y,
					objPos.z + obj->m_Bounds.Center.z + (obj->m_Bounds.Extents.z / 2));
				XMFLOAT3 playerMax(pos.x + m_Bounds.Center.x + (m_Bounds.Extents.x / 2),
					pos.y + m_Bounds.Center.y + m_Bounds.Extents.y,
					pos.z + m_Bounds.Center.z + (m_Bounds.Extents.z / 2));

				if (objMin.x < playerMax.x && objMax.x > playerMin.x &&
					objMin.y < playerMax.y && objMax.y > playerMin.y &&
					objMin.z < playerMax.z && objMax.z > playerMin.z)
				{
					XMFLOAT3 overlapMaxVertex{ min(objMax.x, playerMax.x), min(objMax.y, playerMax.y), min(objMax.z, playerMax.z) };
					XMFLOAT3 overlapMinVertex{ max(objMin.x, playerMin.x), max(objMin.y, playerMin.y), max(objMin.z, playerMin.z) };
					XMFLOAT3 overlapDepth{
					   overlapMaxVertex.x - overlapMinVertex.x,
					   overlapMaxVertex.y - overlapMinVertex.y,
					   overlapMaxVertex.z - overlapMinVertex.z
					};
					
					if (overlapDepth.x < overlapDepth.z)
					{
						if (objPos.x > pos.x)
							overlapDepth.x *= -1;
						overlapDepth.z = 0;
					}

					else
					{
						if (objPos.z > pos.z)
							overlapDepth.z *= -1;
						overlapDepth.x = 0;
					}
					overlapDepth.y = 0;

					pos = MathHelper::Add(pos, overlapDepth);
				}
			}
		}
	}

	// 보석 충돌
	for (int i=0; i<MAX_GEM_COUNT;++i)
	{
		GameObject* obj = AppContext->FindObject<GameObject>(OBJECT_MESH_STR_GEM, to_string(OBJECT_START_INDEX_GEM + i));
		if (obj->m_IsVisible == true)
		{
			XMFLOAT3 objPos = obj->GetPosition();
			XMFLOAT3 objMin(objPos.x + obj->m_Bounds.Center.x - (obj->m_Bounds.Extents.x / 2),
				objPos.y + obj->m_Bounds.Center.y,
				objPos.z + obj->m_Bounds.Center.z - (obj->m_Bounds.Extents.z / 2));
			XMFLOAT3 playerMin(pos.x + m_Bounds.Center.x - (m_Bounds.Extents.x / 2),
				pos.y + m_Bounds.Center.y,
				pos.z + m_Bounds.Center.z - (m_Bounds.Extents.z / 2));
			XMFLOAT3 objMax(objPos.x + obj->m_Bounds.Center.x + (obj->m_Bounds.Extents.x / 2),
				objPos.y + obj->m_Bounds.Center.y + obj->m_Bounds.Extents.y,
				objPos.z + obj->m_Bounds.Center.z + (obj->m_Bounds.Extents.z / 2));
			XMFLOAT3 playerMax(pos.x + m_Bounds.Center.x + (m_Bounds.Extents.x / 2),
				pos.y + m_Bounds.Center.y + m_Bounds.Extents.y,
				pos.z + m_Bounds.Center.z + (m_Bounds.Extents.z / 2));

			if (objMin.x <= playerMax.x && objMax.x >= playerMin.x &&
				objMin.y <= playerMax.y && objMax.y >= playerMin.y &&
				objMin.z <= playerMax.z && objMax.z >= playerMin.z)
			{
				cout << "collison : " << to_string(OBJECT_START_INDEX_GEM + i) << endl;
				AppContext->HiddenGem(i, false);
				Service::GetApp()->AddEvent(EVENT_GAME_GET_COIN, 1, i);
				break;
			}
		}
	}

#ifdef DEBUG_CLIENT
	SetPosition(pos.x, pos.y, pos.z);
#elif DEBUG_SERVER
	SetPosition(pos.x, pos.y, pos.z);
	//Service::GetApp()->AddEvent(EVENT_GAME_MAKE_MOVE, 2, GetPosition(), static_cast<int>(m_AnimationController->m_PlayerState));
#endif
}

void Character::Jump()
{
	if (m_jumpCount < 1 || !m_isGround)
		return;
	
	m_jumpCount = 0;
	m_isGround = false;
	m_jumpForce.y = 55.f;
}

void Character::Falling()
{
	m_jumpForce.y -= GRAVITY;
	XMFLOAT3 prePos = GetPosition();
	XMFLOAT3 pos = MathHelper::Add(prePos, m_jumpForce);

	// 바닥
	if (pos.y < 0.0f)
	{
		pos.y = 0.0f;
		OnGround();
	}

	// y축 충돌검사
	float yGap = m_jumpForce.y;
	float tyPos = pos.y;
	float jumpColl = 0.f;
	SetIndexPos(prePos);
	//SetIndexPos(pos);
	Map* originMap = AppContext->m_Maps[m_MapName];
	for (auto& p : originMap->mapInfoVector)
	{
		for (int i = 0; i < 54; i += 3)
		{
			int aX = m_IndexPosX + shiftArrY[i];
			int aY = m_IndexPosY + shiftArrY[i + 1];
			int aZ = m_IndexPosZ + shiftArrY[i + 2];

			if (aX < 0 || aX >= MAP_WIDTH_BLOCK_NUM
				|| aY <= 0 || aY >= MAP_HEIGHT_BLOCK_NUM
				|| aZ < 0 || aZ >= MAP_DEPTH_BLOCK_NUM)
				continue;

			if (p.typeID == AppContext->m_MapArray[aY][aZ][aX] && p.colWithChar)
			{
				GameObject* obj = AppContext->FindObject<GameObject>(p.meshName, std::to_string(p.typeID));

				XMFLOAT3 objPos = obj->GetPosition();
				XMFLOAT3 objMin(objPos.x + obj->m_Bounds.Center.x - (obj->m_Bounds.Extents.x / 2),
					objPos.y + obj->m_Bounds.Center.y,
					objPos.z + obj->m_Bounds.Center.z - (obj->m_Bounds.Extents.z / 2));
				XMFLOAT3 playerMin(pos.x + m_Bounds.Center.x - (m_Bounds.Extents.x / 2),
					pos.y + m_Bounds.Center.y,
					pos.z + m_Bounds.Center.z - (m_Bounds.Extents.z / 2));
				XMFLOAT3 objMax(objPos.x + obj->m_Bounds.Center.x + (obj->m_Bounds.Extents.x / 2),
					objPos.y + obj->m_Bounds.Center.y + obj->m_Bounds.Extents.y,
					objPos.z + obj->m_Bounds.Center.z + (obj->m_Bounds.Extents.z / 2));
				XMFLOAT3 playerMax(pos.x + m_Bounds.Center.x + (m_Bounds.Extents.x / 2),
					pos.y + m_Bounds.Center.y + m_Bounds.Extents.y,
					pos.z + m_Bounds.Center.z + (m_Bounds.Extents.z / 2));

				if (objMin.x < playerMax.x && objMax.x > playerMin.x &&
					objMin.y < playerMax.y && objMax.y > playerMin.y &&
					objMin.z < playerMax.z && objMax.z > playerMin.z)
				{
					XMFLOAT3 overlapMaxVertex{ min(objMax.x, playerMax.x), min(objMax.y, playerMax.y), min(objMax.z, playerMax.z) };
					XMFLOAT3 overlapMinVertex{ max(objMin.x, playerMin.x), max(objMin.y, playerMin.y), max(objMin.z, playerMin.z) };
					XMFLOAT3 overlapDepth{
					   overlapMaxVertex.x - overlapMinVertex.x,
					   overlapMaxVertex.y - overlapMinVertex.y,
					   overlapMaxVertex.z - overlapMinVertex.z
					};
					if (yGap < overlapDepth.y 
						&& p.meshName != OBJECT_MESH_STR_TREE_01
						&& p.meshName != OBJECT_MESH_STR_TREE_02)
					{
						yGap = overlapDepth.y;
						tyPos = objMax.y;
					}
				}
			}
			if (shiftArrY[i + 1] == 0)
			{
				aY += 1;
				if (aY <= 0 || aY >= MAP_HEIGHT_BLOCK_NUM)
					continue;
				if (p.typeID == AppContext->m_MapArray[aY][aZ][aX] && p.colWithChar)
				{
					GameObject* obj = AppContext->FindObject<GameObject>(p.meshName, std::to_string(p.typeID));

					XMFLOAT3 objPos = obj->GetPosition();
					XMFLOAT3 objMin(objPos.x + obj->m_Bounds.Center.x - (obj->m_Bounds.Extents.x / 2),
						objPos.y + obj->m_Bounds.Center.y,
						objPos.z + obj->m_Bounds.Center.z - (obj->m_Bounds.Extents.z / 2));
					XMFLOAT3 playerMin(pos.x + m_Bounds.Center.x - (m_Bounds.Extents.x / 2),
						pos.y + m_Bounds.Center.y,
						pos.z + m_Bounds.Center.z - (m_Bounds.Extents.z / 2));
					XMFLOAT3 objMax(objPos.x + obj->m_Bounds.Center.x + (obj->m_Bounds.Extents.x / 2),
						objPos.y + obj->m_Bounds.Center.y + obj->m_Bounds.Extents.y,
						objPos.z + obj->m_Bounds.Center.z + (obj->m_Bounds.Extents.z / 2));
					XMFLOAT3 playerMax(pos.x + m_Bounds.Center.x + (m_Bounds.Extents.x / 2),
						pos.y + m_Bounds.Center.y + m_Bounds.Extents.y,
						pos.z + m_Bounds.Center.z + (m_Bounds.Extents.z / 2));

					if (objMin.x < playerMax.x && objMax.x > playerMin.x &&
						objMin.y < playerMax.y && objMax.y > playerMin.y &&
						objMin.z < playerMax.z && objMax.z > playerMin.z)
					{
						XMFLOAT3 overlapMaxVertex{ min(objMax.x, playerMax.x), min(objMax.y, playerMax.y), min(objMax.z, playerMax.z) };
						XMFLOAT3 overlapMinVertex{ max(objMin.x, playerMin.x), max(objMin.y, playerMin.y), max(objMin.z, playerMin.z) };
						XMFLOAT3 overlapDepth{
						   overlapMaxVertex.x - overlapMinVertex.x,
						   overlapMaxVertex.y - overlapMinVertex.y,
						   overlapMaxVertex.z - overlapMinVertex.z
						};
						if (jumpColl < overlapDepth.y)
						{
							jumpColl = overlapDepth.y;
						}
					}
				}
			}
		}
	}
	// cout << "x: " << m_IndexPosX << ", y: " << m_IndexPosY << ", z: " << m_IndexPosZ << endl;
	if (m_jumpForce.y != yGap)
		OnGround();
	pos.y = tyPos;

	if (jumpColl > 0.f)
	{
		pos.y -= jumpColl;
		m_jumpForce.y = 0;
	}

	SetPosition(pos.x, pos.y, pos.z);
}

void Character::OnGround()
{
	m_jumpCount = 1;
	m_isGround = true;
	m_jumpForce.y = 0;
}

void Character::Attack()
{
#ifdef DEBUG_SERVER
	if (m_isSkillOn == true) {
		Service::GetApp()->AddEvent(EVENT_GAME_REQUEST_BULLET, 2, m_attackDirection, 2);
		m_isSkillOn = false;
	}
	else {
		Service::GetApp()->AddEvent(EVENT_GAME_REQUEST_BULLET, 2, m_attackDirection, 1);
		Service::GetApp()->AddEvent(EVENT_GAME_REQUEST_BULLET, 2, m_attackDirection, 1);
		Service::GetApp()->AddEvent(EVENT_GAME_REQUEST_BULLET, 2, m_attackDirection, 1);
	}
#elif DEBUG_CLIENT
	if(m_isSkillOn == true)
	{
		m_skillGauge = 0;
		int bIndex = 0;
		for (int i = 0; i < MAX_SKILL_BULLET_COUNT; ++i)
		{
			if (AppContext->m_AtiveSkillBulletCheck[i] == false)
			{
				bIndex = i;
				AppContext->m_AtiveSkillBullet[AppContext->m_AtiveSkillBulletCnt++] = bIndex;
				AppContext->m_AtiveSkillBulletCheck[i] = true;
				break;
			}
		}
		XMFLOAT3 bStartPos = GetPosition();
		bStartPos.y += 90.f;
		//cout << "AtiveBulletCnt: " << AppContext->m_AtiveBulletCnt << endl;
		//cout << "Position x: " << bStartPos.x << ", y: " << bStartPos.y << ", z: " << bStartPos.z << endl;
		AppContext->DisplayBullet(bIndex, bStartPos, m_attackDirection, m_PlayerID, 2);
		m_isSkillOn = false;
	}
	else
	{
		if(m_skillGauge <MAX_SKILLGAUGE)	
			m_skillGauge += ONE_HIT_CHARGE_SKILLGAUGE;
		m_attackGauge -= 100;
		int bIndex = 0;
		for (int i = 0; i < MAX_BULLET_COUNT; ++i)
		{
			if (AppContext->m_ActiveBulletCheck[i] == false)
			{
				bIndex = i;
				AppContext->m_ActiveBullet[AppContext->m_ActiveBulletCnt++] = bIndex;
				AppContext->m_ActiveBulletCheck[i] = true;
				break;
			}
		}
		XMFLOAT3 bStartPos = GetPosition();
		bStartPos.y += 90.f;
		//cout << "AtiveBulletCnt: " << AppContext->m_AtiveBulletCnt << endl;
		//cout << "Position x: " << bStartPos.x << ", y: " << bStartPos.y << ", z: " << bStartPos.z << endl;
		AppContext->DisplayBullet(bIndex, bStartPos, m_attackDirection, m_PlayerID, 1);
	}
#endif
}

void Character::OnOffSkillMode()
{
	//cout << "SkillGauge: " << m_skillGauge << endl;
	if (m_isSkillOn == true)
	{
		cout << "SkillOff" << endl;
		m_isSkillOn = false;
		return;
	}
	if (m_skillGauge >= MAX_SKILLGAUGE)
	{
		cout << "SkillOn" << endl;
		m_isSkillOn = true;
	}
}

bool Character::OnWater()
{
	XMFLOAT3 pos = GetPosition();
	if (pos.y > 0)
		return false;
	SetIndexPos(pos);
	for (int i = 0; i < MAP_WATER_NUM; ++i)
	{
		if (AppContext->m_WaterMap[i].y == m_IndexPosX
			&& AppContext->m_WaterMap[i].x == m_IndexPosZ)
			return true;
	}
	return false;
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