#pragma once

/*
* �̰� �ܼ��� ������ ���ϰ� ������ ���� ���� ������ ������ ���ø��̴�. �������� ����������.
*/
template<typename K, typename S>
class SocketManager
{
private:
	std::map<K, S*> map_data;
	using const_iter = std::map<K, S*>::const_iterator;
	using find_func = bool(*)(const_iter&);

public:
	bool insert(K key, S* value)
	{
		auto result = map_data.insert(std::make_pair(key, value));
		return result.second;
	}

	bool erase(K key)
	{
		map_data.erase(key);
		return true;
	}

	S* find(K key)
	{
		auto iter = map_data.find(key);
		if (iter == map_data.end())
			return nullptr;

		return iter->second;
	}

	S* find_if(find_func find_)
	{
		auto iter = std::find_if(map_data.begin(), map_data.end(), find_);
		if (iter == map_data.end())
			return nullptr;

		return iter->second;
	}
};

