#pragma once

class GeometryMesh;
class GameObject
{
protected:
	/* 중요 멤버변수 키워드*/
	// m_MeshName: 메시이름, 클라이언트에서 객체를 찾을때 필요한 키값이기도 함
	// m_InstID: 인스턴스 객체의 고유아이디, ObjectInfo의 인스턴스 객체를 관리하는 맵에서 객체의 인덱스를 찾을때 필요 (prop: servermeshID를 string으로 변형한값 / ui: uiName)
	// m_Index: ApplicationContext에 모든 객체의 정보를 담고있는 m_RItemsVec의 인덱스 정보
	// m_ServerMeshID: 서버에서 정의한 키값, typedef의 메시이름과 타입아이디가 정의되어있다. 
		// 클라이언트는 meshname(string)로 키값을 사용하고, 서버는 typeID(int)를 키값으로 사용한다.
		// 패킷으로는 typeID(int)값이 온다.

	std::string m_MeshName;
	std::string m_InstID;
	UINT		m_Index;

public:
	explicit GameObject(std::string meshName, std::string instID);
	virtual ~GameObject();

	std::string GetMeshName() const { return m_MeshName; }
	std::string	GetInstID() const { return m_InstID; }
	UINT		GetIndex() const { return m_Index; }

	virtual void InitializeTransform();
	virtual void Update(const float deltaT);
	virtual bool SetMesh(std::string meshName, std::string submeshName);

public:
	virtual void SetPosition(float posX, float posY, float posZ);
	virtual void SetPosition(DirectX::XMFLOAT3 xmPos);

	virtual void SetRight(const DirectX::XMFLOAT3& Right);
	virtual void SetUp(const DirectX::XMFLOAT3& Up);
	virtual void SetLook(const DirectX::XMFLOAT3& Look);
	virtual void SetMatrixByLook(float x, float y, float z);

	XMFLOAT3 GetPosition() const;
	XMFLOAT3 GetLook() const;
	XMFLOAT3 GetUp() const;
	XMFLOAT3 GetRight() const;

	virtual void MoveStrafe(float fDistance = 1.0f);
	virtual void MoveUp(float fDistance = 1.0f);
	virtual void MoveForward(float fDistance = 1.0f);

	virtual void Move(DWORD dwDirection, float fDistance, bool bUpdateVelocity);
	virtual void Move(const XMFLOAT3& xmf3Shift, bool bVelocity = false);

	virtual void Rotate(const DirectX::XMFLOAT3& axis, float angle);
	virtual void Rotate(const DirectX::XMFLOAT4& quaternion);
	virtual void Rotate(float pitch, float yaw, float roll);

	virtual void Scale(float x, float y, float z);

public:
	// 기타 특성값
	bool m_IsCulling = false;

	int m_ZLayer = -1;

	// 월드좌표 및 텍스쳐좌표
	DirectX::XMFLOAT4X4 m_World;
	DirectX::XMFLOAT4X4 m_TexTransform;
	XMFLOAT2 m_PositionRatio = { 0.f, 0.f }; // 화면비율
	XMFLOAT2 m_SizeRatio = { 0.f, 0.f }; // 화면비율


	// 메쉬, 바운딩박스정보
	GeometryMesh* m_Geo;
	D3D12_PRIMITIVE_TOPOLOGY m_PrimitiveType;
	UINT m_IndexCount;
	UINT m_StartIndexLocation;
	int m_BaseVertexLocation;
	//BoundingBox m_Bounds;

	// 재질정보
	UINT m_MaterialIndex;
	UINT m_OriginMatIndex;
};

