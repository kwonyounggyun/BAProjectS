#pragma once

class Noncopyable
{
public:
	Noncopyable() {}
	virtual ~Noncopyable() = 0 {};

	Noncopyable& operator=(const Noncopyable&) = delete;
	Noncopyable(const Noncopyable&) = delete;
};