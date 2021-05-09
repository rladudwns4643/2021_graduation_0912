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
	int m_SpawnLoaction;				// ������ġ
	std::string m_UserPlayerMeshName;	// �÷��̾� �޽��̸�

public:
	// ĳ���� ����
	bool m_IsMoving = false;

public:
	explicit Character(std::string type, std::string id);
	virtual ~Character();

	virtual void InitializeTransform() override;

	// ������Ʈ + ������Ʈ ������Ʈ
	void Update(const float deltaT);

public:

public:
	void SetCamera(Camera* myCamera, CameraType cameraType);
	void SetCamera(CameraType cameraType);
	void SetController();

	virtual bool	SetMesh(std::string meshName, std::string submeshName) override;
	void			SetMaterial(int materialIndex);

	void SetPosition(float posX, float posY, float posZ);
	void SetPosition(DirectX::XMFLOAT3 xmPos);

	void SetAnimationController(SkinnedModelInstance* skinnedModelInst);
	void SetAnimationPlayerState(AnimationController::PlayerState playerState);
	void SetAnimationKeyState(AnimationController::PlayerState keyState);

public:
	// �̵� & ȸ��
	bool			Move(DWORD dwDirection, float fDistance);
	virtual void	Move(DWORD dwDirection, float fDistance, bool bUpdateVelocity) override;
	virtual void	Move(const XMFLOAT3& xmf3Shift, bool bVelocity = false) override;

	virtual void	Rotate(float pitch, float yaw, float roll) override;

private:
	bool		RayMapTriangleIntersect(XMVECTOR orig, XMVECTOR dir, XMVECTOR v0, XMVECTOR v1, XMVECTOR v2, XMVECTOR& P);
	bool		RayObjTriangleIntersect(XMVECTOR orig, XMVECTOR dir, XMVECTOR v0, XMVECTOR v1, XMVECTOR v2, XMVECTOR& P);

public:
	// ĳ���� ������Ʈ��
	Camera									*m_MyCamera;
	std::unique_ptr<PlayerController>		m_PlayerController;
	std::unique_ptr<AnimationController>	m_AnimationController;
	std::map<std::string, CharacterParts*>	m_Parts;

public:
	bool			m_IsThirdCamera = true;
	float			m_HP = 1.f;		// ���� HP%

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
};

