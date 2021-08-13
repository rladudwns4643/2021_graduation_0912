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
	XMFLOAT4 m_SpawnLoaction;				// ������ġ
	std::string m_UserPlayerMeshName;	// �÷��̾� �޽��̸�
	std::string m_MapName;				// ĳ���Ͱ� �����ִ� ���̸�

public:
	// ĳ���� ����
	bool m_IsMoving = false;
	bool m_IsAiming = false;

public:
	explicit Character(std::string type, std::string id);
	virtual ~Character();

	virtual void InitializeTransform() override;
	bool ReleaseTransform();

	// ������Ʈ + ������Ʈ ������Ʈ
	void Update(const float deltaT);
	void WeaponUpdate();
	void UpdateStateUI();

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

	void SetIndexPos(float posX, float posY, float posZ);
	void SetIndexPos(DirectX::XMFLOAT3 xmPos);

	void SetAnimationController(SkinnedModelInstance* skinnedModelInst);
	void SetAnimationPlayerState(AnimationController::PlayerState playerState);
	void SetAnimationKeyState(AnimationController::PlayerState keyState);

	void SetLookToCameraLook();

public:
	// �̵� & ȸ��
	bool			Move(DWORD dwDirection, float fDistance);
	virtual void	Move(DWORD dwDirection, float fDistance, bool bUpdateVelocity) override;
	virtual void	Move(const XMFLOAT3& xmf3Shift, bool bVelocity = false) override;

	bool OnWater();

	virtual void	Rotate(float pitch, float yaw, float roll) override;

	// jump
	void Jump();
	void Falling();
	void OnGround();

	void Attack();
	void OnOffSkillMode();
public:
	// ĳ���� ������Ʈ��
	Camera									*m_MyCamera;
	std::unique_ptr<PlayerController>		m_PlayerController;
	std::unique_ptr<AnimationController>	m_AnimationController;
	std::map<std::string, CharacterParts*>	m_Parts;

	int m_PlayerID = 0;

public:
	int m_hp = MAX_HP;
	bool			m_IsThirdCamera = true;

private:
	// ĳ�������� �޽�����
	GeometryMesh* m_CharacterGeo;
	D3D12_PRIMITIVE_TOPOLOGY	m_CharacterPrimitiveType;
	UINT						m_CharacterIndexCount;
	UINT						m_CharacterStartIndexLocation;
	int							m_CharacterBaseVertexLocation;
	BoundingBox					m_CharacterBounds;

	// ĳ�������� ���� �ε���
	UINT m_CharacterMaterialIndex;

	// �̵����
	DirectX::XMFLOAT3 m_Position;
	DirectX::XMFLOAT3 m_Right;
	DirectX::XMFLOAT3 m_Up;
	DirectX::XMFLOAT3 m_Look;

	float m_tSpeed;

	// �ʿ����� ��ġ
	int m_IndexPosX;
	int m_IndexPosY;
	int m_IndexPosZ;

	// ĳ���� �ֺ� 27ĭ
	int shiftArr[81] = {
		 1,  1, -1,  0,  1, -1,  -1,  1, -1,
		 1,  1,  0,  0,  1,  0,  -1,  1,  0,
		 1,  1,  1,  0,  1,  1,  -1,  1,  1,

		 1,  0, -1,  0,  0, -1,  -1,  0, -1,
		 1,  0,  0,  0,  0,  0,  -1,  0,  0,
		 1,  0,  1,  0,  0,  1,  -1,  0,  1,

		 1, -1, -1,  0, -1, -1,  -1, -1, -1,
		 1, -1,  0,  0, -1,  0,  -1, -1,  0,
		 1, -1,  1,  0, -1,  1,  -1, -1,  1,
	};

	int shiftArrY[54] = {
		 1,  0, -1,  0,  0, -1,  -1,  0, -1,
		 1,  0,  0,  0,  0,  0,  -1,  0,  0,
		 1,  0,  1,  0,  0,  1,  -1,  0,  1,

		 1, -1, -1,  0, -1, -1,  -1, -1, -1,
		 1, -1,  0,  0, -1,  0,  -1, -1,  0,
		 1, -1,  1,  0, -1,  1,  -1, -1,  1,
	};

	// ���� ����
	int m_jumpCount = 1;
	bool m_isGround = true;
	DirectX::XMFLOAT3 m_jumpForce{0.f, 0.f, 0.f};

	// ���� ����
	int m_attackGauge = MAX_ATTACKGAUGE;
	XMFLOAT3 m_attackDirection;
	bool m_isSkillOn = false;
	int m_skillGauge = 0;
	int m_tempHp;
	int m_tempAttackGauge;
	int m_tempSkillGauge;
};

