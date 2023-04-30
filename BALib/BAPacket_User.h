#pragma once
#include "BAType.h"

#pragma pack(push, 1)
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
	int id;
	BVector3D _pos;
	
	int GetSize()
	{
		return sizeof(*this);
	}
};

enum { _inform_player_data = 3 };
class inform_player_data
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
#pragma pack(pop)