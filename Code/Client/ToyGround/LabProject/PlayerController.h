#pragma once
#include "Controller.h"

// Command ������� �߰�
class Character;
class PlayerController final : public Controller
{
public:
	explicit PlayerController(Character* player);
	virtual ~PlayerController() = default;

public:
	virtual void Update(const float deltaT) override;

private:
	virtual void HandleInput(const float deltaT) override;
	virtual void MouseCallback() override;

	virtual void OnKeyPressed() override;
	virtual void OnKeyReleased() override;
private:
	Character* m_Owner;

public:
	bool m_PausGame = false;
};