#pragma once
#include "GameObject.h"

/* Chararcter
@ Players (Master / Student)
@ Controller / Animation Parts(GameObject*)
@ Bone(ContantBuffer)가 필요함
@ Character는 CharacterParts를 관리하는 매니저 역할
*/
class Camera;
class Character : public GameObject
{

public:
	int m_SpawnLoaction;				/* 스폰위치 */
	std::string m_UserPlayerMeshName;	/* 유저플레이어 메쉬이름 */

public:
	// 캐릭터 상태
	bool m_IsMoving = false;

public:
	explicit Character(std::string type, std::string id);
	virtual ~Character();

	virtual void InitializeTransform() override;

	// 업데이트 + 컴포넌트 업데이트
	void Update(const float deltaT);
	void CameraUpdate(const float delteT);

public:
	// 캐릭터 게임 컨텐츠 기능
	bool Transform(std::string meshName, std::string submeshName, UINT matIndex);
	bool ReleaseTransform();

public:
	void SetCamera(Camera* myCamera, CameraType cameraType);
	void SetCamera(CameraType cameraType);

	virtual bool	SetMesh(std::string meshName, std::string submeshName) override;
	void			SetMaterial(int materialIndex);

	void SetPosition(float posX, float posY, float posZ);
	void SetPosition(DirectX::XMFLOAT3 xmPos);


public:
	// 이동 & 회전
	bool			Move(DWORD dwDirection, float fDistance);
	virtual void	Move(DWORD dwDirection, float fDistance, bool bUpdateVelocity) override;
	virtual void	Move(const XMFLOAT3& xmf3Shift, bool bVelocity = false) override;

	virtual void	Rotate(float pitch, float yaw, float roll) override;

private:
	XMFLOAT3	GetThunderBoltDirection(float screenX, float screenY);

	void		PropIntersect(float screenX, float screenY, XMVECTOR& orign, XMVECTOR& direction, string& objName, string& insID);
	float		CharacterIntersect(Character* character, std::string MeshName, float distance, float screenX, float screenY, XMVECTOR& orign, XMVECTOR& direction, string& objName, string& insID, float wizardDis);

	bool		RayMapTriangleIntersect(XMVECTOR orig, XMVECTOR dir, XMVECTOR v0, XMVECTOR v1, XMVECTOR v2, XMVECTOR& P);
	bool		RayObjTriangleIntersect(XMVECTOR orig, XMVECTOR dir, XMVECTOR v0, XMVECTOR v1, XMVECTOR v2, XMVECTOR& P);

public:
	// 캐릭터 컴포넌트들
	Camera* m_MyCamera;

public:
	float m_HP = 1.f;		// 유저 HP%

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

