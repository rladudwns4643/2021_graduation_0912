#pragma once
#include "Controller.h"

class TitleScene;
class LobbyScene;
class GameplayScene;

class TitleController final : public Controller
{
public:
	explicit TitleController(TitleScene* myScene);
	virtual ~TitleController() = default;

public:
	virtual void Update(const float deltaT) override;

private:
	virtual void HandleInput(const float deltaT) override;
	virtual void MouseCallback() override;

private:
	TitleScene* m_MyScene;
	std::string m_PickedUIName;
	std::string m_PickedUIObjectName;
};


class LobbyController final : public Controller
{
public:
	explicit LobbyController(LobbyScene* myScene);
	virtual ~LobbyController() = default;

public:
	virtual void Update(const float deltaT) override;

private:
	virtual void HandleInput(const float deltaT) override;
	virtual void MouseCallback() override;

private:
	LobbyScene* m_MyScene;
	std::string m_PickedUIName;
	std::string m_PickedUIObjectName;


};

class GameplayController final : public Controller
{
public:
	explicit GameplayController(GameplayScene* myScene);
	virtual ~GameplayController() = default;

public:
	virtual void Update(const float deltaT) override;

private:
	virtual void HandleInput(const float deltaT) override;
	virtual void MouseCallback() override;

private:
	GameplayScene* m_MyScene;
};