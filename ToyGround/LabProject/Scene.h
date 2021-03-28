#pragma once
#include "SceneController.h"

namespace Core
{
	extern Microsoft::WRL::ComPtr<ID3D12Device> g_Device;
	extern Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList> g_CommandList;

	extern int g_DisplayWidth;
	extern int g_DisplayHeight;

	extern bool g_InputSwitch;
	extern int g_Chating;
	extern WCHAR g_ChatBuf[256];	// �ϼ��� ���ڵ� ����
	extern WCHAR g_TempChatBuf[2];	// ���� ���� ���ڸ� �ӽ� ����
}

/* Scenes
@ �׸� ������Ʈ���� ���������� �����Ѵ�.
@ ������Ʈ ���� �� ��ġ
*/
class Scene abstract
{
protected:
	friend class SceneManager;

public:
	Scene();
	virtual ~Scene();

	virtual void Initialize() = 0;
	virtual void OnResize() = 0;

public:
	virtual bool	Enter() = 0;
	virtual void	Exit() = 0;

	virtual void	Update(const float& fDeltaTime) = 0;
	virtual void	Render() = 0;

protected:
	std::string m_SceneName;
	Controller* m_SceneController;
	DirectX::BoundingSphere m_SceneBounds;
};

