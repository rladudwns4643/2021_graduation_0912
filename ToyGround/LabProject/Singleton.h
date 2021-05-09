#pragma once
#include "pch.h"
// Template Singleton

template <typename Type>
class TemplateSingleton
{
protected:
	// 持失切 社瑚切 private
	TemplateSingleton() {};
	virtual ~TemplateSingleton() {};

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

private:
	static Type* m_pApp;
};

template <typename Type> Type* TemplateSingleton<Type>::m_pApp = nullptr;