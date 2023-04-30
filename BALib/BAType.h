#pragma once
#include <cmath>
#include <float.h>

class BVector3D
{
public:
	double x;
	double y;
	double z;

public:
	BVector3D() : x(0), y(0), z(0) {}
	BVector3D(double x, double y, double z) : x(x), y(y), z(z) {}
	~BVector3D() {}

	BVector3D operator-(const BVector3D& other) const
	{
		return BVector3D(x - other.x, y - other.y, z - other.z);
	}

	BVector3D operator+(const BVector3D& other) const
	{
		return BVector3D(x + other.x, y + other.y, z + other.z);
	}

	BVector3D operator*(double w) const
	{
		return BVector3D(x * w, y * w, z * w);
	}

	BVector3D operator/(double w) const
	{
		return BVector3D(x / w, y / w, z / w);
	}

	double DotProduct(const BVector3D& other) const
	{
		return x * other.x + y * other.y + z * other.z;
	}

	BVector3D CrossProduct(const BVector3D& other) const
	{

	}

	bool operator==(const BVector3D& other)
	{
		if (abs(x - other.x) <= DBL_EPSILON && abs(y - other.y) <= DBL_EPSILON && abs(z - other.z) <= DBL_EPSILON)
			return true;

		return false;
	}
};