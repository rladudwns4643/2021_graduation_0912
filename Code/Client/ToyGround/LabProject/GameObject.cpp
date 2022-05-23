#include "pch.h"
#include "GameObject.h"
#include "AssertsReference.h"

static unsigned int s_currentIndex = 0;

GameObject::GameObject(std::string meshName, std::string instID) :
	m_MeshName(meshName),
	m_InstID(instID),
	m_Index(s_currentIndex++),
	m_SkinnedModelInst(nullptr)
{
	m_World = MathHelper::Identity4x4();
	m_TexTransform = MathHelper::Identity4x4();
}

GameObject::~GameObject()
{
}

void GameObject::InitializeTransform()
{
	m_World = MathHelper::Identity4x4();
	m_TexTransform = MathHelper::Identity4x4();
}

void GameObject::Update(const float deltaT)
{
	XMMATRIX bworld = XMLoadFloat4x4(&m_World);
}

bool GameObject::SetMesh(std::string meshName, std::string submeshName)
{
	if (!AssertsReference::GetApp()->m_GeometryMesh.count(meshName)) return false;
	if (this == nullptr) return false;

	m_Geo = AssertsReference::GetApp()->m_GeometryMesh[meshName].get();
	m_IndexCount = m_Geo->DrawArgs[submeshName].IndexCount;
	m_StartIndexLocation = m_Geo->DrawArgs[submeshName].StartIndexLocation;
	m_BaseVertexLocation = m_Geo->DrawArgs[submeshName].BaseVertexLocation;
	m_PrimitiveType = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
	m_Bounds = m_Geo->DrawArgs[meshName].Bounds;

	return true;
}
bool GameObject::SetBBMesh(std::string submeshName)
{
	if (!AssertsReference::GetApp()->m_BoundingBoxMesh->DrawArgs.count(submeshName)) return false;
	if (this == nullptr) return false;

	m_Bb = AssertsReference::GetApp()->m_BoundingBoxMesh.get();
	m_IndexCountBb = m_Bb->DrawArgs[submeshName].IndexCount;
	m_StartIndexLocationBb = m_Bb->DrawArgs[submeshName].StartIndexLocation;
	m_BaseVertexLocationBb = m_Bb->DrawArgs[submeshName].BaseVertexLocation;
	m_PrimitiveTypeBb = D3D_PRIMITIVE_TOPOLOGY_LINESTRIP;

	return true;
}


void GameObject::SetPosition(float posX, float posY, float posZ)
{
	m_World._41 = posX;
	m_World._42 = posY;
	m_World._43 = posZ;
}

void GameObject::SetPosition(DirectX::XMFLOAT3 xmPos)
{
	SetPosition(xmPos.x, xmPos.y, xmPos.z);
}

void GameObject::SetIndexPos(float posX, float posY, float posZ)
{
	m_IndexPosX = (int)((posX + (STD_CUBE_SIZE / 2)) / STD_CUBE_SIZE + (MAP_WIDTH_BLOCK_NUM / 2));
	m_IndexPosY = (int)(posY / STD_CUBE_SIZE);
	m_IndexPosZ = (int)((posZ + (STD_CUBE_SIZE / 2)) / STD_CUBE_SIZE + (MAP_DEPTH_BLOCK_NUM / 2));
}

void GameObject::SetIndexPos(DirectX::XMFLOAT3 xmPos)
{
	SetIndexPos(xmPos.x, xmPos.y, xmPos.z);
}
void GameObject::SetRight(const DirectX::XMFLOAT3& Right)
{
	m_World._11 = Right.x;
	m_World._12 = Right.y;
	m_World._13 = Right.z;
}

void GameObject::SetUp(const DirectX::XMFLOAT3& Up)
{
	m_World._21 = Up.x;
	m_World._22 = Up.y;
	m_World._23 = Up.z;
}

void GameObject::SetLook(const DirectX::XMFLOAT3& Look)
{
	m_World._31 = Look.x;
	m_World._32 = Look.y;
	m_World._33 = Look.z;
}

void GameObject::SetMatrixByLook(float x, float y, float z)
{
	XMFLOAT3 look{ x,y,z };
	XMFLOAT3 up = GetUp();
	XMFLOAT3 right = MathHelper::CrossProduct(up, look);	//������ up look �ƴϸ� look up �� �� �ϳ�

	m_World._11 = right.x;
	m_World._12 = right.y;
	m_World._13 = right.z;

	m_World._31 = x;
	m_World._32 = y;
	m_World._33 = z;
}

void GameObject::SetWorldMatrix(DirectX::XMFLOAT4X4 world)
{
	m_World = world;
}

DirectX::XMFLOAT4X4 GameObject::GetWorldMatrix()
{
	return m_World;
}

XMFLOAT3 GameObject::GetPosition() const
{
	return XMFLOAT3(m_World._41, m_World._42, m_World._43);
}

XMFLOAT3 GameObject::GetLook() const
{
	XMFLOAT3 look = { m_World._31, m_World._32, m_World._33 };
	return MathHelper::Normalize(look);
}

XMFLOAT3 GameObject::GetUp() const
{
	XMFLOAT3 up = { m_World._21, m_World._22, m_World._23 };
	return MathHelper::Normalize(up);
}

XMFLOAT3 GameObject::GetRight() const
{
	XMFLOAT3 right = { m_World._11, m_World._12, m_World._13 };
	return MathHelper::Normalize(right);
}

void GameObject::MoveStrafe(float fDistance)
{
	XMFLOAT3 xmf3Position = GetPosition();
	XMFLOAT3 xmf3Right = GetRight();
	xmf3Position = MathHelper::Add(xmf3Position, xmf3Right, fDistance);
	SetPosition(xmf3Position);
}

void GameObject::MoveUp(float fDistance)
{
	XMFLOAT3 xmf3Position = GetPosition();
	XMFLOAT3 xmf3Up = GetUp();
	xmf3Position = MathHelper::Add(xmf3Position, xmf3Up, fDistance);
	SetPosition(xmf3Position);
}

void GameObject::MoveForward(float fDistance)
{
	XMFLOAT3 xmf3Position = GetPosition();
	XMFLOAT3 xmf3Look = GetLook();
	xmf3Position = MathHelper::Add(xmf3Position, xmf3Look, fDistance);
	SetPosition(xmf3Position);
}

void GameObject::Move(DWORD dwDirection, float fDistance, bool bUpdateVelocity)
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

void GameObject::Move(const XMFLOAT3& xmf3Shift, bool bVelocity)
{
	XMFLOAT3 pos = MathHelper::Add(GetPosition(), xmf3Shift);
	SetPosition(pos);
}

void GameObject::Rotate(const DirectX::XMFLOAT3& axis, float angle)
{
	XMMATRIX RotMat = XMMatrixRotationAxis(XMLoadFloat3(&axis), XMConvertToRadians(angle));
	XMMATRIX World = XMLoadFloat4x4(&m_World) * RotMat;
	XMStoreFloat4x4(&m_World, World);
}

void GameObject::Rotate(const DirectX::XMFLOAT4& quaternion)
{
	XMMATRIX RotMat = XMMatrixRotationQuaternion(XMLoadFloat4(&quaternion));
	XMMATRIX World = XMLoadFloat4x4(&m_World) * RotMat;
	XMStoreFloat4x4(&m_World, World);
}

void GameObject::Rotate(float pitch, float yaw, float roll)
{
	XMMATRIX R = XMMatrixRotationRollPitchYaw(
		MathHelper::XMConvertToRadians(pitch),
		MathHelper::XMConvertToRadians(yaw),
		MathHelper::XMConvertToRadians(roll));
	XMStoreFloat4x4(&m_World, XMLoadFloat4x4(&m_World) * R);
}

void GameObject::Scale(float x, float y, float z)
{
	m_World._11 *= x;
	m_World._22 *= y;
	m_World._33 *= z;
}

void GameObject::Update()
{
	XMFLOAT3 pos = GetPosition();
	pos = MathHelper::Add(pos, m_Speed);
	SetPosition(pos);
}