#pragma once

class IGameApp abstract
{
	bool isDone = false;

public:
	virtual void Startup(void) = 0;
	virtual void Cleanup(void) = 0;

	virtual bool IsDone();

	virtual void Update(const float deltaT) = 0;

	virtual void RenderScene(void) = 0;

	virtual void RenderUI(void) {};

	void		Exit();
};