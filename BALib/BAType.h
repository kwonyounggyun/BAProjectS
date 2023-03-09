#pragma once

class BVector3D
{
public:
	float x;
	float y;
	float z;

public:
	BVector3D() : x(0), y(0), z(0) {}
	BVector3D(float x, float y, float z) : x(x), y(y), z(z) {}
	~BVector3D() {}

	BVector3D operator-(const BVector3D& other) const
	{
		return BVector3D(x - other.x, y - other.y, z - other.z);
	}

	BVector3D operator+(const BVector3D& other) const
	{
		return BVector3D(x + other.x, y + other.y, z + other.z);
	}

	BVector3D operator*(float w) const
	{
		return BVector3D(x * w, y * w, z * w);
	}

	BVector3D operator/(float w) const
	{
		return BVector3D(x / w, y / w, z / w);
	}

	float DotProduct(const BVector3D& other) const
	{
		return x * other.x + y * other.y + z * other.z;
	}

	BVector3D CrossProduct(const BVector3D& other) const
	{

	}
};