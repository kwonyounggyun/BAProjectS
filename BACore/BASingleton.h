#pragma once

template<typename T>
class BASingleton
{
public:
	static T* const GetInstance()
	{
		static T _instance;
		
		return &_instance;
	}
};