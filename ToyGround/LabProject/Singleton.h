#pragma once
#include "pch.h"

template <typename Type>
class TemplateSingleton
{
protected:
	TemplateSingleton() {};
	virtual ~TemplateSingleton() {};

private:
	static Type* m_pApp;

public:
	static Type* GetApp()
	{
		if (m_pApp == nullptr)
			m_pApp = new Type;
		return m_pApp;
	};

	static void DestroyApp()
	{
		SAFE_DELETE_PTR(m_pApp);
	};
};

template <typename Type> Type* TemplateSingleton<Type>::m_pApp = nullptr;