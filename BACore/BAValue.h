#pragma once
using BA_VALUE_COUNT_TYPE = __int64;
using BA_VALUE_PIECE_TYPE = __int64;
using BA_VALUE_DECIMAL_TYPE = double;

const BA_VALUE_COUNT_TYPE MAX_LINE = 0x7FFFFFFF;
const BA_VALUE_PIECE_TYPE MAX_VALUE = 0x7FFFFFFF;
const BA_VALUE_DECIMAL_TYPE MAX_DECIMAL = (BA_VALUE_DECIMAL_TYPE)MAX_LINE * MAX_VALUE;

class BAValue
{
private:
	BA_VALUE_COUNT_TYPE _count;
	BA_VALUE_PIECE_TYPE _piece;

	BA_VALUE_DECIMAL_TYPE GetValue() const
	{
		return (BA_VALUE_DECIMAL_TYPE)_count * MAX_VALUE + _piece;
	}

public:
	BAValue(BA_VALUE_DECIMAL_TYPE value)
	{
		if (value > MAX_DECIMAL)
			value = MAX_DECIMAL;
		else if (value < -1 * MAX_DECIMAL)
			value = -1 * MAX_DECIMAL;

		_count = (BA_VALUE_COUNT_TYPE)(value / MAX_VALUE);
		_piece = (BA_VALUE_PIECE_TYPE)(value - ((BA_VALUE_DECIMAL_TYPE)_count * MAX_VALUE));
	}

	~BAValue() {}

	BAValue operator+(BA_VALUE_DECIMAL_TYPE value)
	{
		return BAValue(GetValue() + value);
	}

	BAValue operator+(const BAValue& other)
	{
		return *this + other.GetValue();
	}

	BAValue operator-(BA_VALUE_DECIMAL_TYPE value)
	{
		return BAValue(GetValue() - value);
	}

	BAValue operator-(const BAValue& other)
	{
		return *this - other.GetValue();
	}

	BA_VALUE_DECIMAL_TYPE operator/(BA_VALUE_DECIMAL_TYPE value)
	{
		return GetValue() / value;
	}

	BA_VALUE_DECIMAL_TYPE operator/(const BAValue& other)
	{
		return *this / other.GetValue();
	}

	BAValue operator*(BA_VALUE_DECIMAL_TYPE value)
	{
		return GetValue() * value;
	}

	BAValue& operator=(const BAValue& other)
	{
		_count = other._count;
		_piece = other._piece;

		return *this;
	}

	bool operator<(BA_VALUE_DECIMAL_TYPE value)
	{
		return GetValue() < value;
	}

	bool operator<(const BAValue& other)
	{
		return *this < other.GetValue();
	}

	bool operator>(BA_VALUE_DECIMAL_TYPE value)
	{
		return GetValue() > value;
	}

	bool operator>(const BAValue& other)
	{
		return *this > other.GetValue();
	}

	bool operator==(BA_VALUE_DECIMAL_TYPE value)
	{
		return (GetValue() - value) <= DBL_EPSILON;
	}

	bool operator==(const BAValue& other)
	{
		return *this == other.GetValue();
	}
};