#pragma once

class GeometryMesh;
class GameObject
{
protected:
	/* �߿� ������� Ű����*/
	// m_MeshName: �޽��̸�, Ŭ���̾�Ʈ���� ��ü�� ã���� �ʿ��� Ű���̱⵵ ��
	// m_InstID: �ν��Ͻ� ��ü�� �������̵�, ObjectInfo�� �ν��Ͻ� ��ü�� �����ϴ� �ʿ��� ��ü�� �ε����� ã���� �ʿ� (prop: servermeshID�� string���� �����Ѱ� / ui: uiName)
	// m_Index: ApplicationContext�� ��� ��ü�� ������ ����ִ� m_RItemsVec�� �ε��� ����
	// m_ServerMeshID: �������� ������ Ű��, typedef�� �޽��̸��� Ÿ�Ծ��̵� ���ǵǾ��ִ�. 
		// Ŭ���̾�Ʈ�� meshname(string)�� Ű���� ����ϰ�, ������ typeID(int)�� Ű������ ����Ѵ�.
		// ��Ŷ���δ� typeID(int)���� �´�.

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
	// ��Ÿ Ư����
	bool m_IsCulling = false;

	int m_ZLayer = -1;

	// ������ǥ �� �ؽ�����ǥ
	DirectX::XMFLOAT4X4 m_World;
	DirectX::XMFLOAT4X4 m_TexTransform;
	XMFLOAT2 m_PositionRatio = { 0.f, 0.f }; // ȭ�����
	XMFLOAT2 m_SizeRatio = { 0.f, 0.f }; // ȭ�����


	// �޽�, �ٿ���ڽ�����
	GeometryMesh* m_Geo;
	D3D12_PRIMITIVE_TOPOLOGY m_PrimitiveType;
	UINT m_IndexCount;
	UINT m_StartIndexLocation;
	int m_BaseVertexLocation;
	//BoundingBox m_Bounds;

	// ��������
	UINT m_MaterialIndex;
	UINT m_OriginMatIndex;
};

