#pragma once

template<class Handler>
class Singleton {
protected:
	Singleton() {};
	virtual ~Singleton() {};

public:
	static Handler* GetInstance() {
		if (m_instance == nullptr) m_instance = new Handler;
		return m_instance;
	}

	static void DeleteInstance() {
		delete m_instance;
		m_instance = nullptr;
	}
private:
	static Handler* m_instance;
};

template<class Handler> Handler* Singleton<Handler>::m_instance = nullptr;