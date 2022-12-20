#pragma once

template<typename T, typename ... types>
class BASingleton
{
private:
	static T* _instance;

public:
	static const T* GetInstance()
	{
		if (_instance == nullptr)
			_instance = new T(types);

		return _instance;
	}
};