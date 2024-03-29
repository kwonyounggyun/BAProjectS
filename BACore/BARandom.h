#pragma once
#include <random>


class BARandom
{
public:
	static int Random(int min, int max)
	{
		static std::random_device rd;
		static std::mt19937 gen(rd());

		std::uniform_int_distribution<int> dis(min, max);
		return dis(gen);
	}
};
