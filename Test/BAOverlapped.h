#pragma once
#include "IOverlapped.h"
#include "BAMemoryPool.h"

class BARead:public IOverlapped, BAMemoryPool<BARead>
{

};

class BAWrite :public IOverlapped, BAMemoryPool<BAWrite>
{

};

class BAAccept :public IOverlapped, BAMemoryPool<BAAccept>
{

};