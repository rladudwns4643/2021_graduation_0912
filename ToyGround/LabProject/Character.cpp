#include "pch.h"
#include "Character.h"
#include "Camera.h"
#include "ApplicationContext.h"
#include "AssertsReference.h"
#include "CommandContext.h"

Character::Character(std::string type, std::string id) : GameObject(type, id), m_MyCamera(nullptr),
{
	m_Position = { 0,0,0 };
	m_Right = { 1,0,0 };
	m_Up = { 0,1,0 };
	m_Look = { 0,0,1 x};
}

Character::~Character()
{
}

void Character::CameraUpdate(const float deltaT)
{
	if (!m_MyCamera || m_MyCamera == NULL)
		return;

	for (auto& p : originMap->mapInfoVector)
	{
		GameObject* obj = AppContext->FindObject<GameObject>(p.meshName, std::to_string(p.typeID));	// proptype, typeid
		XMFLOAT3 objPos = obj->GetPosition();
		if (objPos.y > 1500)
			continue;
		XMFLOAT3 cameraPos = this->m_MyCamera->GetPosition3f();
		XMFLOAT3 objBoundPos3 = obj->m_Geo->DrawArgs[p.meshName].Bounds.Center;

		DirectX::XMStoreFloat3(&objBoundPos3, XMVector3Transform(XMLoadFloat3(&objBoundPos3), XMLoadFloat4x4(&obj->m_World)));

		BoundingBox bb;
		// bb.Center = { tmp._41, tmp._42, tmp._43 };
		bb.Center = { objBoundPos3.x, objBoundPos3.y, objBoundPos3.z };
		bb.Extents = obj->m_Geo->DrawArgs[p.meshName].Bounds.Extents;

		// if(bb.Center.y -)

		XMFLOAT3 corners[8] = {};
		obj->m_Geo->DrawArgs[p.meshName].Bounds.GetCorners(corners);

		for (int i = 0; i < 8; ++i)
		{
			XMStoreFloat3(&corners[i], XMVector3Transform(XMLoadFloat3(&corners[i]), XMLoadFloat4x4(&obj->m_World)));
		}

		// cout << "앞왼위" << endl;
		m_MyCamera->IntersectsObject(XMLoadFloat3(&corners[6]), XMLoadFloat3(&corners[4]), XMLoadFloat3(&corners[7]));

		// cout << "앞오아" << endl;
		m_MyCamera->IntersectsObject(XMLoadFloat3(&corners[6]), XMLoadFloat3(&corners[5]), XMLoadFloat3(&corners[4]));

		// cout << "왼왼위" << endl;
		m_MyCamera->IntersectsObject(XMLoadFloat3(&corners[7]), XMLoadFloat3(&corners[0]), XMLoadFloat3(&corners[3]));

		// cout << "왼오아" << endl;
		m_MyCamera->IntersectsObject(XMLoadFloat3(&corners[7]), XMLoadFloat3(&corners[4]), XMLoadFloat3(&corners[0]));

		// cout << "오왼위" << endl;
		m_MyCamera->IntersectsObject(XMLoadFloat3(&corners[6]), XMLoadFloat3(&corners[2]), XMLoadFloat3(&corners[5]));

		// cout << "오오아" << endl;
		m_MyCamera->IntersectsObject(XMLoadFloat3(&corners[2]), XMLoadFloat3(&corners[1]), XMLoadFloat3(&corners[5]));

		// cout << "뒤왼위" << endl;
		m_MyCamera->IntersectsObject(XMLoadFloat3(&corners[3]), XMLoadFloat3(&corners[1]), XMLoadFloat3(&corners[2]));

		// cout << "뒤오아" << endl;
		m_MyCamera->IntersectsObject(XMLoadFloat3(&corners[3]), XMLoadFloat3(&corners[0]), XMLoadFloat3(&corners[1]));

		// cout << "위왼위" << endl;
		m_MyCamera->IntersectsObject(XMLoadFloat3(&corners[2]), XMLoadFloat3(&corners[7]), XMLoadFloat3(&corners[3]));

		// cout << "위오아" << endl;
		m_MyCamera->IntersectsObject(XMLoadFloat3(&corners[2]), XMLoadFloat3(&corners[6]), XMLoadFloat3(&corners[7]));

	}
}

void Character::SoundUpdate(const float deltaT)
{
	switch (m_AnimationController->m_PlayerState)
	{
	case AnimationController::PlayerState::STATE_FORWARD:
	case AnimationController::PlayerState::STATE_BACKWARD:
	case AnimationController::PlayerState::STATE_LEFT_STRAFE:
	case AnimationController::PlayerState::STATE_RIGHT_STRAFE:
		m_RunningSoundDeltaTime += deltaT;
		if (m_RunningSoundCoolTime < m_RunningSoundDeltaTime)
		{
			// if(내 플레이어가 아닐떄)
			float distanceRate = 0.f;
			if (m_MeshName != m_UserPlayerMeshName &&
				(m_MeshName == CHARACTER_BAIRD || m_MeshName == CHARACTER_DRUID || m_MeshName == CHARACTER_FEMALE_PEASANT || m_MeshName == CHARACTER_MALE_PEASANT || m_MeshName == CHARACTER_SORCERER))// m_UserPlayerMeshName != "")
			{
				// 내 플레이어와 현재 플레이어의 거리비율을 구해서 volume에 넣어준다.
				XMFLOAT3 userPos = AppContext->FindObject<Character>(m_UserPlayerMeshName, m_UserPlayerMeshName)->GetPosition();
				XMFLOAT3 myPos = { m_World._41,  m_World._42,  m_World._43 };
				// 두 벡터사이의 거리구하기
				distanceRate = pow((myPos.x - userPos.x), 2) + pow((myPos.y - userPos.y), 2) + pow((myPos.z - userPos.z), 2);
				distanceRate = sqrt(distanceRate);
				distanceRate /= 40'00; //  이건 뭐해주는 거지
				SoundManager::GetApp()->PlaySoundOnce(L"EFF_Player_Footstep.wav", SoundManager::CHANNEL_ID::PLAYER_RUNNING, 2.5f - distanceRate);
				m_RunningSoundDeltaTime = 0;
			}
		}
		break;
	}
}

void Character::BlurUpdate(const float deltaT)
{
	if (m_CharacterSwitchBlur)
	{
		m_BlurTotalTime += deltaT;
		if (m_BlurLifeTime < m_BlurTotalTime)
		{
			m_BlurTotalTime = 0.f;
			ControlBlurEffect(false);
		}
	}
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

	// Animation & State
	m_AnimationController->Update(deltaT);

	// Sound
	SoundUpdate(deltaT);

	// Blur
	BlurUpdate(deltaT);

	// 카메라 변환
	if (m_MyCamera != NULL)
	{
		CameraType cType = m_MyCamera->GetCameraType();

		if (cType == CameraType::Third)
		{
			XMFLOAT3 target = GetPosition();
			target.y += 200;
			m_MyCamera->Update(target, deltaT);
			CameraUpdate(deltaT);
		}
		else if (cType == CameraType::First)
		{
			XMFLOAT3 target = GetPosition();
			// target.y += 200;
			m_MyCamera->Update(target, deltaT);
			//CameraUpdate(deltaT);
		}
	}
}

bool Character::Transform(std::string meshName, std::string submeshName, UINT matIndex)
{
	if (this == nullptr) return false;

	if (GameObject::SetMesh(meshName, submeshName))
	{
		m_MaterialIndex = matIndex;
		m_TransformType = TransformType::Prop;

		// 변신했을때 카메라 오프셋 조절
		if (m_MyCamera)
		{
			if (m_MyCamera->GetCameraType() == CameraType::Third)
			{
				XMFLOAT3 extent = m_Geo->DrawArgs[meshName].Bounds.Extents;
				if (m_Bounds.Extents.x > 100.f)
					m_MyCamera->SetOffset(XMFLOAT3(extent.x * 0.35f, extent.y * 1.3f,
						-extent.x * 2.3f - extent.y * 1.5f));
				else
					m_MyCamera->SetOffset({ extent.x + extent.x * 0.05f,
						extent.y * 2.f,
						-extent.x * 2 * 2.5f - extent.y * 2.f });

				CameraUpdate(0.f);
			}
			else if (m_MyCamera->GetCameraType() == CameraType::First)
			{
				XMFLOAT3 extent = m_Geo->DrawArgs[meshName].Bounds.Extents;
				m_MyCamera->SetOffset(XMFLOAT3(extent.x * 0.5f, extent.y * 1.3f, extent.z * 1.8f));
				m_MyCamera->Rotate(InputHandler::g_MouseChangebleY, InputHandler::g_MouseChangebleX, 0.f);
			}

		}

		return true;
	};
	return false;
}

void Character::TransformParticle(bool isCapture)
{
	// 변신시 파티클 on
	AppContext->DisplayParticle(PARTICLE_NAME_SMOKE, m_MeshName, GetPosition(), true, isCapture);
}

void Character::PenartyParticle(bool onoff)
{
	if (onoff)
		AppContext->DisplayParticle(PARTICLE_NAME_PENALTY, m_MeshName, GetPosition(), true);
	else
		AppContext->HiddenParticle(PARTICLE_NAME_PENALTY, m_MeshName);
}

void Character::ControlBlurEffect(bool onoff)
{
	m_CharacterSwitchBlur = onoff;
	GraphicsContext::GetApp()->OnBlurEffect(onoff);

	if (onoff)
		AppContext->DisplayUI2D(OBJECT_NAME_CRUSH_MIRROR, OBJECT_NAME_CRUSH_MIRROR, XMFLOAT2(0.f, 0.f), XMFLOAT2(Core::g_DisplayWidth * 10, Core::g_DisplayHeight * 10), false, AnchorType::Center);
	else
		AppContext->HiddenUI2D(OBJECT_NAME_CRUSH_MIRROR, OBJECT_NAME_CRUSH_MIRROR);
}

bool Character::ReleaseTransform()
{
	m_TransformType = TransformType::Character;
	m_TransformMeshName = m_MeshName;

	// 메쉬, 머테리얼 정보 리셋
	m_Geo = m_CharacterGeo;
	m_IndexCount = m_CharacterIndexCount;
	m_StartIndexLocation = m_CharacterStartIndexLocation;
	m_BaseVertexLocation = m_CharacterBaseVertexLocation;
	m_PrimitiveType = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
	m_Bounds = m_CharacterBounds;
	m_MaterialIndex = m_CharacterMaterialIndex;

	// 다시 카메라 오프셋 리셋
	if (m_MyCamera)
	{
		if (m_MyCamera->GetCameraType() == CameraType::Third)
		{
			XMFLOAT3 extent = m_Bounds.Extents;
			if (m_Bounds.Extents.x > 100.f)
				m_MyCamera->SetOffset(XMFLOAT3(extent.x * 0.35f, extent.y * 1.3f,
					-extent.x * 2.3f - extent.y * 1.5f));
			else
				m_MyCamera->SetOffset({ extent.x + extent.x * 0.05f,
					extent.y * 2.f,
					-extent.x * 2 * 2.5f - extent.y * 2.f });

			CameraUpdate(0.f);
		}
		else if (m_MyCamera->GetCameraType() == CameraType::First)
		{
			XMFLOAT3 extent = m_Bounds.Extents;
			m_MyCamera->SetOffset(XMFLOAT3(extent.x * 0.5f, extent.y * 1.3f, extent.z * 1.8f));
			m_MyCamera->Rotate(InputHandler::g_MouseChangebleY, InputHandler::g_MouseChangebleX, 0.f);
		}
	}

	return true;
}



void Character::SetCamera(Camera* myCamera, CameraType cameraType)
{
	m_MyCamera = myCamera;

	SetCamera(cameraType);
}

void Character::SetCamera(CameraType cameraType)
{
	if (m_MyCamera == nullptr) return;
	m_MyCamera->SetCamera(cameraType, this);

	switch (cameraType)
	{
	case CameraType::First:
		m_MyCamera->SetTimeLag(0.f);
		m_MyCamera->SetOffset(XMFLOAT3(m_Bounds.Extents.x * 0.5f, m_Bounds.Extents.y * 1.3f, m_Bounds.Extents.z * 1.5f));
		m_MyCamera->SetRotation({ 0.f, 0.f, 0.f });

		m_MyCamera->Rotate(InputHandler::g_MouseChangebleY, InputHandler::g_MouseChangebleX, 0.f);
		break;
	case CameraType::Third:
		m_MyCamera->SetTimeLag(0.0f);
		if (m_Bounds.Extents.x > 100.f)
			m_MyCamera->SetOffset(XMFLOAT3(m_Bounds.Extents.x * 0.35f, m_Bounds.Extents.y * 1.3f, -m_Bounds.Extents.x * 2.3f - m_Bounds.Extents.y * 1.5f));
		else
			m_MyCamera->SetOffset({ m_Bounds.Extents.x + m_Bounds.Extents.x * 0.05f, m_Bounds.Extents.y * 2.f, -m_Bounds.Extents.x * 2 * 2.5f - m_Bounds.Extents.y * 2.f });
		m_MyCamera->SetRotation({ 0.f, 0.f, 0.f });
		break;
	case CameraType::Free:
		m_MyCamera->SetTimeLag(0.f);
		m_MyCamera->SetOffset(XMFLOAT3(0.0f, 200.f, -150.f));
		break;
	}
}

void Character::SetController(std::string mapName)
{
	// 커서지우고 에임그려주기
	m_PlayerController = make_unique<PlayerController>(this);
	m_PlayerController->SetMapName(mapName);
}

void Character::SetParts(CharacterParts* parts)
{
	m_Parts[parts->m_PartsName] = parts;
}

void Character::SetParticle(std::string particleName, std::string instID)
{
	Particle* particle = AppContext->FindObject<Particle>(particleName, instID);
	if (!particle) return;

	particle->SetOwner(this);
	m_Particles[particleName] = particle;
}

void Character::SetBlurLifeTime(float blurLifeTime)
{
	if (m_PlayerRole == ROLE_STUDENT)
		m_BlurLifeTime = blurLifeTime;
	else
		m_BlurLifeTime = 0;
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

#ifdef DEBUG_CLIENT
		Move(DIR_FORWARD, fDistance, true);
#elif DEBUG_SERVER
#endif

		XMFLOAT3 dir;
		DirectX::XMStoreFloat3(&dir, direction);
		dir.y = 0.f;
		direction = DirectX::XMVector3Normalize(XMLoadFloat3(&dir));

		XMFLOAT3 cLook = m_Look;
		cLook.y = 0.f;
		cLook = MathHelper::Normalize(cLook);

		float cosValue = DirectX::XMVectorGetX(DirectX::XMVector3Dot(direction, XMLoadFloat3(&cLook)));

		XMFLOAT3 cUp = m_Up;
		// cUp.y = 0.f;
		cUp = MathHelper::Normalize(cUp);

		float ccw = DirectX::XMVectorGetX(DirectX::XMVector3Dot(XMLoadFloat3(&cUp), DirectX::XMVector3Cross(direction, XMLoadFloat3(&cLook))));


		float acosValue = 0.f;

		if (cosValue <= -1.f)
			acosValue = 3.141592;
		else if (cosValue > 1.f)
			acosValue = 0.f;
		else
			acosValue = acos(cosValue);

		float degree = XMConvertToDegrees(acosValue);

		if (ccw > 0)	// ccw가 양수이면 방시계로 돌아야함
			degree = -degree;
		degree *= 0.2f;

		bool isChange = false;

		if (fabs(degree) > 1.f)
		{
			Rotate(0.f, XMConvertToRadians(degree), 0.f);
			isChange = true;
		}
		degree = 0.f;


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

void Character::SetMapName(std::string mapName)
{
	m_MapName = mapName;
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

XMFLOAT3 Character::CalculateThunderBoltDirectionUsingMap(float screenX, float screenY)
{
	if (!m_MyCamera)
		return{};

	// test bb --> 나중에 멤버 변수로 하나 만들어두면 좋을듯
	BoundingBox bb;
	bb.Center = { 2500.f, 0.f, 2500.f };
	bb.Extents = { 5000.f, 5000.f, 5000.f };

	// 화면 좌표 --> 투영 좌표
	XMFLOAT4X4 matProjection = m_MyCamera->GetProj4x4f();

	float vx = (+2.0f * screenX / Core::g_DisplayWidth - 1.0f) / matProjection._11;
	float vy = (-2.0f * screenY / Core::g_DisplayHeight + 1.0f) / matProjection._22;

	XMVECTOR origin = DirectX::XMVectorSet(0.0f, 0.0f, 0.0f, 1.0f);
	XMVECTOR direction = DirectX::XMVectorSet(vx, vy, 1.0f, 0.0f);

	// 투영 좌표 --> 카메라 좌표 -->	월드 좌표 --> 로컬 좌표
	XMFLOAT4X4 matView = m_MyCamera->GetView4x4f();
	XMMATRIX matInvView = XMMatrixInverse(&XMMatrixDeterminant(XMLoadFloat4x4(&matView)), XMLoadFloat4x4(&matView));

	// test world matrix --> bb랑 같이 나중에 변경해야함
	XMMATRIX bbWorld = XMMatrixSet(
		bb.Extents.x, 0.f, 0.f, 0.f,
		0.f, bb.Extents.y, 0.f, 0.f,
		0.f, 0.f, bb.Extents.z, 0.f,
		bb.Center.x, bb.Center.y, bb.Center.z, 1.f
	);
	// XMMATRIX W = XMLoadFloat4x4(&bbWorld);
	XMMATRIX invWorld = XMMatrixInverse(&XMMatrixDeterminant(bbWorld), bbWorld);

	// Tranform ray to vi space of Mesh.
	XMMATRIX toLocal = XMMatrixMultiply(matInvView, invWorld);

	// origin, direction 모두 로컬 좌표가 됨
	origin = DirectX::XMVector3TransformCoord(origin, toLocal);
	direction = DirectX::XMVector3TransformNormal(direction, toLocal);
	// Make the ray direction unit length for the intersection tests.
	direction = DirectX::XMVector3Normalize(direction);


	// obj의 바운딩 박스의 각 꼭지점
	XMFLOAT3 corners[8] = {};
	bb.GetCorners(corners);

	XMVECTOR P = XMVectorZero();
	XMVECTOR tmpP = XMVectorZero();
	// 앞
	if (RayMapTriangleIntersect(origin, direction, XMLoadFloat3(&corners[0]), XMLoadFloat3(&corners[3]), XMLoadFloat3(&corners[2]), tmpP))
		P = tmpP;
	else if (RayMapTriangleIntersect(origin, direction, XMLoadFloat3(&corners[0]), XMLoadFloat3(&corners[2]), XMLoadFloat3(&corners[1]), tmpP))
		P = tmpP;
	// 왼
	else if (RayMapTriangleIntersect(origin, direction, XMLoadFloat3(&corners[4]), XMLoadFloat3(&corners[7]), XMLoadFloat3(&corners[3]), tmpP))
		P = tmpP;
	else if (RayMapTriangleIntersect(origin, direction, XMLoadFloat3(&corners[4]), XMLoadFloat3(&corners[3]), XMLoadFloat3(&corners[0]), tmpP))
		P = tmpP;
	// 오른
	else if (RayMapTriangleIntersect(origin, direction, XMLoadFloat3(&corners[1]), XMLoadFloat3(&corners[2]), XMLoadFloat3(&corners[6]), tmpP))
		P = tmpP;
	else if (RayMapTriangleIntersect(origin, direction, XMLoadFloat3(&corners[1]), XMLoadFloat3(&corners[6]), XMLoadFloat3(&corners[5]), tmpP))
		P = tmpP;
	// // // 뒤
	else if (RayMapTriangleIntersect(origin, direction, XMLoadFloat3(&corners[5]), XMLoadFloat3(&corners[6]), XMLoadFloat3(&corners[7]), tmpP))
		P = tmpP;
	else if (RayMapTriangleIntersect(origin, direction, XMLoadFloat3(&corners[5]), XMLoadFloat3(&corners[7]), XMLoadFloat3(&corners[4]), tmpP))
		P = tmpP;
	// 바닥
	// else if (RayCoveredMapTriangleIntersect(origin, direction, XMLoadFloat3(&corners[4]), XMLoadFloat3(&corners[0]), XMLoadFloat3(&corners[1]), tmpP))
	// 	P = tmpP;
	// else if (RayCoveredMapTriangleIntersect(origin, direction, XMLoadFloat3(&corners[4]), XMLoadFloat3(&corners[1]), XMLoadFloat3(&corners[5]), tmpP))
	// 	P = tmpP;
	// 위
	else if (RayMapTriangleIntersect(origin, direction, XMLoadFloat3(&corners[3]), XMLoadFloat3(&corners[7]), XMLoadFloat3(&corners[6]), tmpP))
		P = tmpP;
	else if (RayMapTriangleIntersect(origin, direction, XMLoadFloat3(&corners[3]), XMLoadFloat3(&corners[6]), XMLoadFloat3(&corners[2]), tmpP))
		P = tmpP;
	else
	{
		cout << "thunder bolt direction: " << direction.m128_f32[0] << ", " << direction.m128_f32[1] << ", " << direction.m128_f32[2] << endl;
	}

	XMFLOAT3 pos = {};
	XMStoreFloat3(&pos, P);

	// P: Local -> World coordinate
	P = XMVector3Transform(P, bbWorld);
	XMStoreFloat3(&pos, P);


	// test 캐릭터 좌표
	XMFLOAT3 CharPos = { m_World._41, m_World._42, m_World._43 };
	CharPos.x += m_Look.x * 30.f;
	CharPos.y += 120.f;
	CharPos.z += m_Look.z * 30.f;

	P = XMVectorSubtract(P, XMLoadFloat3(&CharPos));
	P = XMVector3Normalize(P);
	XMStoreFloat3(&pos, P);

	return pos;
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

XMFLOAT3 Character::CalculateThunderBoltDirectionUsingObj(XMVECTOR orign, XMVECTOR direction, string objName, string insID)
{
	XMVECTOR P = XMVectorZero();
	XMVECTOR tmpP = XMVectorZero();


	GameObject* obj = AppContext->FindObject<GameObject>(objName, insID);	// proptype, typeid
	if (!obj)
	{
		cout << "CalculateThunderBoltDirectionUsingObj - obj NULL!" << endl;
		return {};
	}
	BoundingBox bb = obj->m_Bounds;
	// cout << obj->GetMeshName() << endl;


	XMFLOAT3 corners[8] = {};
	bb.GetCorners(corners);

	// 뒤
	if (RayObjTriangleIntersect(orign, direction, XMLoadFloat3(&corners[4]), XMLoadFloat3(&corners[7]), XMLoadFloat3(&corners[6]), tmpP))
		P = tmpP;
	// cout << "l point ================ " << P.m128_f32[0] << ", " << P.m128_f32[1] << ", " << P.m128_f32[2] << endl;
	else if (RayObjTriangleIntersect(orign, direction, XMLoadFloat3(&corners[4]), XMLoadFloat3(&corners[6]), XMLoadFloat3(&corners[5]), tmpP))
		P = tmpP;
	// 앞
	else if (RayObjTriangleIntersect(orign, direction, XMLoadFloat3(&corners[1]), XMLoadFloat3(&corners[2]), XMLoadFloat3(&corners[3]), tmpP))
		P = tmpP;
	else if (RayObjTriangleIntersect(orign, direction, XMLoadFloat3(&corners[1]), XMLoadFloat3(&corners[3]), XMLoadFloat3(&corners[0]), tmpP))
		P = tmpP;
	// 왼
	else if (RayObjTriangleIntersect(orign, direction, XMLoadFloat3(&corners[0]), XMLoadFloat3(&corners[3]), XMLoadFloat3(&corners[7]), tmpP))
		P = tmpP;
	else if (RayObjTriangleIntersect(orign, direction, XMLoadFloat3(&corners[0]), XMLoadFloat3(&corners[7]), XMLoadFloat3(&corners[4]), tmpP))
		P = tmpP;
	// 오
	else if (RayObjTriangleIntersect(orign, direction, XMLoadFloat3(&corners[5]), XMLoadFloat3(&corners[6]), XMLoadFloat3(&corners[2]), tmpP))
		P = tmpP;
	else if (RayObjTriangleIntersect(orign, direction, XMLoadFloat3(&corners[5]), XMLoadFloat3(&corners[2]), XMLoadFloat3(&corners[1]), tmpP))
		P = tmpP;
	// 위
	else if (RayObjTriangleIntersect(orign, direction, XMLoadFloat3(&corners[7]), XMLoadFloat3(&corners[3]), XMLoadFloat3(&corners[2]), tmpP))
		P = tmpP;
	else if (RayObjTriangleIntersect(orign, direction, XMLoadFloat3(&corners[7]), XMLoadFloat3(&corners[2]), XMLoadFloat3(&corners[6]), tmpP))
		P = tmpP;
	// 밑
	else if (RayObjTriangleIntersect(orign, direction, XMLoadFloat3(&corners[0]), XMLoadFloat3(&corners[4]), XMLoadFloat3(&corners[5]), tmpP))
		P = tmpP;
	else if (RayObjTriangleIntersect(orign, direction, XMLoadFloat3(&corners[0]), XMLoadFloat3(&corners[5]), XMLoadFloat3(&corners[1]), tmpP))
		P = tmpP;


	XMFLOAT3 pos = {};
	P = XMVector3Transform(P, XMLoadFloat4x4(&obj->m_World));

	// 캐릭터 좌표
	XMFLOAT3 CharPos = { m_World._41, m_World._42, m_World._43 };
	CharPos.x += m_Look.x * 30.f;
	CharPos.y += 120.f;
	CharPos.z += m_Look.z * 30.f;

	P = XMVectorSubtract(P, XMLoadFloat3(&CharPos));
	P = XMVector3Normalize(P);
	DirectX::XMStoreFloat3(&pos, P);


	return pos;
}

void Character::PropIntersect(float screenX, float screenY, XMVECTOR& orign, XMVECTOR& dir, string& objName, string& insID)
{
	float distance = 0.f;

	Map* originMap = AppContext->m_Maps[m_MapName];
	if (!originMap)
	{
		cout << "맵이름: " << m_MapName << endl << "오류! 맵이름이 잘못되었습니다." << endl;
		return;
	}

	// 프롭 체크
	float tmin = 0.0f;
	XMVECTOR charPos = XMLoadFloat3(&XMFLOAT3{ m_World._41, m_World._42, m_World._43 });
	XMVECTOR CharToCameraDis = XMVector3Length(XMVectorSubtract(charPos, m_MyCamera->GetPosition()));
	float CharToCameraDistance = XMVectorGetX(CharToCameraDis);

	for (auto& p : originMap->mapInfoVector)
	{
		GameObject* obj = AppContext->FindObject<GameObject>(p.meshName, std::to_string(p.typeID));	// proptype, typeid
		XMFLOAT4X4 matProjection = m_MyCamera->GetProj4x4f();

		float vx = (+2.0f * screenX / Core::g_DisplayWidth - 1.0f) / matProjection._11;
		float vy = (-2.0f * screenY / Core::g_DisplayHeight + 1.0f) / matProjection._22;

		XMFLOAT4X4 matView = m_MyCamera->GetView4x4f();
		XMMATRIX matInvView = XMMatrixInverse(&XMMatrixDeterminant(XMLoadFloat4x4(&matView)), XMLoadFloat4x4(&matView));

		XMVECTOR original = XMVectorSet(0.0f, 0.0f, 0.0f, 1.0f);
		XMVECTOR direction = XMVectorSet(vx, vy, 1.0f, 0.0f);

		XMMATRIX W = XMLoadFloat4x4(&obj->m_World);
		XMMATRIX invWorld = XMMatrixInverse(&XMMatrixDeterminant(W), W);

		// Tranform ray to vi space of Mesh.
		XMMATRIX toLocal = XMMatrixMultiply(matInvView, invWorld);

		original = XMVector3TransformCoord(original, toLocal);
		direction = XMVector3TransformNormal(direction, toLocal);

		// Make the ray direction unit length for the intersection tests.
		direction = XMVector3Normalize(direction);

		if (obj->m_Geo->DrawArgs[p.meshName].Bounds.Intersects(original, direction, tmin) && tmin > CharToCameraDistance)
		{
			if (distance == 0.f)
			{
				distance = tmin;
				objName = obj->GetMeshName();
				orign = original;
				dir = direction;
				insID = obj->GetInstID();
			}
			else
			{
				if (distance > tmin)
				{
					distance = tmin;
					objName = obj->GetMeshName();
					orign = original;
					dir = direction;
					insID = obj->GetInstID();
				}
			}
		}
	}


	float tmp = INFINITE;
	float cameraToWizardDis;
	Character* character = AppContext->FindObject<Character>(CHARACTER_WIZARD, CHARACTER_WIZARD);
	if (character->m_IsVisible)
	{
		tmp = CharacterIntersect(character, CHARACTER_WIZARD, INFINITE, screenX, screenY, orign, dir, objName, insID, 0.f);
		if (tmp != INFINITE && (distance > tmp))
		{
			cameraToWizardDis = tmp;
		}
	}


	character = AppContext->FindObject<Character>(CHARACTER_DRUID, CHARACTER_DRUID);
	if (character->m_IsVisible)
	{
		tmp = CharacterIntersect(character, CHARACTER_DRUID, distance, screenX, screenY, orign, dir, objName, insID, cameraToWizardDis);
		if (tmp != INFINITE && (distance > tmp))
		{
			distance = tmp;
		}
	}

	character = AppContext->FindObject<Character>(CHARACTER_BAIRD, CHARACTER_BAIRD);
	if (character->m_IsVisible)
	{
		tmp = CharacterIntersect(character, CHARACTER_BAIRD, distance, screenX, screenY, orign, dir, objName, insID, cameraToWizardDis);
		if (tmp != INFINITE && (distance > tmp))
		{
			distance = tmp;
		}
	}

	character = AppContext->FindObject<Character>(CHARACTER_FEMALE_PEASANT, CHARACTER_FEMALE_PEASANT);
	if (character->m_IsVisible)
	{
		tmp = CharacterIntersect(character, CHARACTER_FEMALE_PEASANT, distance, screenX, screenY, orign, dir, objName, insID, cameraToWizardDis);
		if (tmp != INFINITE && (distance > tmp))
		{
			distance = tmp;
		}
	}

	character = AppContext->FindObject<Character>(CHARACTER_MALE_PEASANT, CHARACTER_MALE_PEASANT);
	if (character->m_IsVisible)
	{
		tmp = CharacterIntersect(character, CHARACTER_MALE_PEASANT, distance, screenX, screenY, orign, dir, objName, insID, cameraToWizardDis);
		if (tmp != INFINITE && (distance > tmp))
		{
			distance = tmp;
		}
	}

	character = AppContext->FindObject<Character>(CHARACTER_SORCERER, CHARACTER_SORCERER);
	if (character->m_IsVisible)
	{
		tmp = CharacterIntersect(character, CHARACTER_SORCERER, distance, screenX, screenY, orign, dir, objName, insID, cameraToWizardDis);
		if (tmp != INFINITE && (distance > tmp))
		{
			distance = tmp;
		}
	}
}


float Character::CharacterIntersect(Character* character, std::string MeshName, float distance, float screenX, float screenY, XMVECTOR& orign, XMVECTOR& dir, string& objName, string& insID, float wizardDis)
{
	if (!(character->GetPosition().x == 0.f) && !(character->GetPosition().z == 0.f))
	{
		XMFLOAT4X4 matProjection = m_MyCamera->GetProj4x4f();

		float vx = (+2.0f * screenX / Core::g_DisplayWidth - 1.0f) / matProjection._11;
		float vy = (-2.0f * screenY / Core::g_DisplayHeight + 1.0f) / matProjection._22;

		XMFLOAT4X4 matView = m_MyCamera->GetView4x4f();
		XMMATRIX matInvView = XMMatrixInverse(&XMMatrixDeterminant(XMLoadFloat4x4(&matView)), XMLoadFloat4x4(&matView));

		XMVECTOR original = XMVectorSet(0.0f, 0.0f, 0.0f, 1.0f);
		XMVECTOR direction = XMVectorSet(vx, vy, 1.0f, 0.0f);

		XMMATRIX W = XMLoadFloat4x4(&character->m_World);
		XMMATRIX invWorld = XMMatrixInverse(&XMMatrixDeterminant(W), W);

		// Tranform ray to vi space of Mesh.
		XMMATRIX toLocal = XMMatrixMultiply(matInvView, invWorld);

		original = XMVector3TransformCoord(original, toLocal);
		direction = XMVector3TransformNormal(direction, toLocal);

		// Make the ray direction unit length for the intersection tests.
		direction = XMVector3Normalize(direction);

		float tmin = INFINITE;
		if (character->m_Bounds.Intersects(original, direction, tmin))
		{
			if ((MeshName != CHARACTER_WIZARD) && (tmin > wizardDis) && (distance > tmin))
			{
				objName = MeshName;
				orign = original;
				dir = direction;
				insID = character->GetInstID();
				return tmin;
			}
		}
	}

	return INFINITE;
}

XMFLOAT3 Character::GetThunderBoltDirection(float screenX, float screenY)
{
	XMVECTOR ResultOriginal;
	XMVECTOR ResultDirection;
	string objectName = "";
	string insID = "";

	XMFLOAT3 resultDirection = {};

	PropIntersect(screenX, screenY, ResultOriginal, ResultDirection, objectName, insID);

	// cout << "--------------- " << objectName << endl;
	if (objectName == "")
		return CalculateThunderBoltDirectionUsingMap(screenX, screenY);
	else
		return CalculateThunderBoltDirectionUsingObj(ResultOriginal, ResultDirection, objectName, insID);

	// return XMFLOAT3();
}
