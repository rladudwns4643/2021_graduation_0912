#pragma once
#include "Singleton.h"

class Scene;
class SceneManager : public TemplateSingleton<SceneManager>
{
private:
	friend class Service;
	friend class LoadingService;

	template<class NewScene>
	void CreateScene(SceneType sceneType, std::string sceneName)
	{
		Scene* scene = new NewScene;
		scene->m_SceneName = sceneName;
		scene->Initialize();
		m_Scenes[static_cast<int>(sceneType)] = scene;
	}

	void SendEvent(SceneType sceneType, int sEvent);
	void SendEventArgs(SceneType sceneType, int sEvent, int argsCount, ...);

	XMFLOAT3 argLookCallback = { 0.f,1.f,0.f };

public:
	explicit SceneManager();
	virtual ~SceneManager();

	virtual void InitializeScenes();
	virtual void OnResize();

public:
	/* ChangeScene
	@ ChangeScene: 다음 씬으로 넘어감
	@ ChangeScene(SceneType): 이동하고 싶은 씬으로 이동함
	*/
	void	ChangeScene();
	void	ChangeScene(SceneType sceneType);

	void	EnterScene(SceneType sceneType);
	void	ExitScene();

	void	UpdateScene(const float& deltaT);
	void	RenderScene();
	void	RenderUI();
	void	WriteShadow();

	Scene* GetCurScene() const;
	UINT	GetCurSceneIndex() const;

private:
	std::array<Scene*, static_cast<int>(SceneType::eCount)> m_Scenes;
	UINT m_CurScene;
	std::mutex m_MutexChangeScene;
};

