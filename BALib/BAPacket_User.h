#pragma once

enum { _request_us_move = 1 };
class request_us_move 
{
public:
	BVector3D _move;
	BVector3D _forward;

	int GetSize()
	{
		return sizeof(*this);
	}
};

enum { _reply_su_move = 2 };
class reply_su_move
{
public:
	BVector3D _pos;
	
	int GetSize()
	{
		return sizeof(*this);
	}
};

class Request_US_Jump
{
	int GetSize()
	{
		return sizeof(*this);
	}
};