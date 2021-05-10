#pragma once
#include "GameObject.h"
#include "PlayerController.h"
#include "AnimationController.h"

class Camera;
class CharacterParts;
class Character : public GameObject
{
	friend class PlayerController;
	friend class CharacterParts;

public:
	int m_SpawnLoaction;				// 스폰위치
	std::string m_UserPlayerMeshName;	// 플레이어 메쉬이름
	std::string m_MapName;				// 캐릭터가 속해있는 맵이름

public:
	// 캐릭터 상태
	bool m_IsMoving = false;

public:
	explicit Character(std::string type, std::string id);
	virtual ~Character();

	virtual void InitializeTransform() override;

	// 업데이트 + 컴포넌트 업데이트
	void Update(const float deltaT);
	void WeaponUpdate();

public:

public:
	void SetMapName(std::string mapName);
	void SetCamera(Camera* myCamera, CameraType cameraType);
	void SetCamera(CameraType cameraType);
	void SetController();
	void SetParts(CharacterParts* parts);

	virtual bool	SetMesh(std::string meshName, std::string submeshName) override;
	void			SetMaterial(int materialIndex);

	void SetPosition(float posX, float posY, float posZ);
	void SetPosition(DirectX::XMFLOAT3 xmPos);

	void SetAnimationController(SkinnedModelInstance* skinnedModelInst);
	void SetAnimationPlayerState(AnimationController::PlayerState playerState);
	void SetAnimationKeyState(AnimationController::PlayerState keyState);

public:
	// 이동 & 회전
	bool			Move(DWORD dwDirection, float fDistance);
	virtual void	Move(DWORD dwDirection, float fDistance, bool bUpdateVelocity) override;
	virtual void	Move(const XMFLOAT3& xmf3Shift, bool bVelocity = false) override;

	virtual void	Rotate(float pitch, float yaw, float roll) override;

private:
	bool		RayMapTriangleIntersect(XMVECTOR orig, XMVECTOR dir, XMVECTOR v0, XMVECTOR v1, XMVECTOR v2, XMVECTOR& P);
	bool		RayObjTriangleIntersect(XMVECTOR orig, XMVECTOR dir, XMVECTOR v0, XMVECTOR v1, XMVECTOR v2, XMVECTOR& P);

public:
	// 캐릭터 컴포넌트들
	Camera									*m_MyCamera;
	std::unique_ptr<PlayerController>		m_PlayerController;
	std::unique_ptr<AnimationController>	m_AnimationController;
	std::map<std::string, CharacterParts*>	m_Parts;

	int m_PlayerID = 0;

public:
	bool			m_IsThirdCamera = true;
	float			m_HP = 1.f;		// 유저 HP%

private:
	// 캐릭터전용 메쉬정보
	GeometryMesh* m_CharacterGeo;
	D3D12_PRIMITIVE_TOPOLOGY	m_CharacterPrimitiveType;
	UINT						m_CharacterIndexCount;
	UINT						m_CharacterStartIndexLocation;
	int							m_CharacterBaseVertexLocation;
	BoundingBox					m_CharacterBounds;

	// 캐릭터전용 재질 인덱스
	UINT m_CharacterMaterialIndex;

	// 이동행렬
	DirectX::XMFLOAT3 m_Position;
	DirectX::XMFLOAT3 m_Right;
	DirectX::XMFLOAT3 m_Up;
	DirectX::XMFLOAT3 m_Look;
};

