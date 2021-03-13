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

class TOY_GROUND : public IGameApp, public TemplateSingleton<TOY_GROUND>
{
public:
	virtual void Startup(void) override;
	virtual void Cleanup(void) override;
	virtual void Update(float deltaT) override;
	virtual void RenderScene(void) override;
	virtual void RenderUI(void) override;

	void OnResize();
	
private:
	void BuildAsserts();
	void BuildCharactersAndParticles();

public:
	Camera* m_pCamera;
	std::map<std::string, std::unique_ptr<Light>> m_pLights;

private:
private:
	SceneManager					*m_pSceneManager;
	ApplicationContext				*m_pAppContext;
	AssertsReference				*m_pAssetsRef;
};

