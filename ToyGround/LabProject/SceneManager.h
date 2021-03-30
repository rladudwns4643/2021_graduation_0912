#pragma once
#include "Singleton.h"

class Scene;
class SceneManager : public TemplateSingleton<SceneManager>
{
private:
	template<class NewScene>
	void CreateScene(SceneType sceneType, std::string sceneName)
	{
		Scene* scene = new NewScene;
		scene->m_SceneName = sceneName;
		scene->Initialize();
		m_Scenes[static_cast<int>(sceneType)] = scene;
	}

public:
	explicit SceneManager();
	virtual ~SceneManager();

	virtual void InitializeScenes();
	virtual void OnResize();

	void	ChangeScene();
	void	ChangeScene(SceneType sceneType);

	void	EnterScene(SceneType sceneType);
	void	ExitScene();

	void	UpdateScene(const float& deltaT);
	void	RenderScene();

	Scene* GetCurScene() const;
	UINT   GetCurSceneIndex() const;

private:
	std::array<Scene*, static_cast<int>(SceneType::eCount)> m_Scenes;
	UINT m_CurScene;
	std::mutex m_MutexChangeScene;
};
