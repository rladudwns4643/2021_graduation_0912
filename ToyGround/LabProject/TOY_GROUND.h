#pragma once
#include "IGameApp.h"
#include "Singleton.h"
#include "Camera.h"

namespace Core
{
	extern HWND													g_hMainWnd;
	extern HWND													g_hVideoWnd;

	extern Microsoft::WRL::ComPtr<ID3D12Device>					g_pDevice;
	extern Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList>	g_pCommandList;

	extern int													g_iDisplayWidth;
	extern int													g_iDisplayHeight;
}

class SceneManager;
class ApplicationContext;
class AssertsReference;
class CommandCenter;

class TOY_GROUND : public IGameApp, public TemplateSingleton<TOY_GROUND>
{
public:
	virtual void Startup(void) override;
	virtual void Cleanup(void) override;
	virtual void Update(float deltaT) override;
	virtual void RenderScene(void) override;

	void OnResize();
	
private:
	void BuildAsserts();
	void BuildCharacters();

public:
	Camera* m_Camera = NULL;
	std::map<std::string, std::unique_ptr<Light>> m_pLights;

private:
	SceneManager					*m_pSceneManager = NULL;
	ApplicationContext				*m_pAppContext = NULL;
	AssertsReference				*m_pAssetsRef = NULL;
	CommandCenter					*m_CommandCenter = NULL;
};

