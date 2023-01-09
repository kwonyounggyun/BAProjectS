#pragma once
#include "BACore.h"

struct BABufferUnit
{
public:
    BABufferUnit() :_capacity(sizeof(_buf)), _head(0), _tail(0) {}

    __int32 Write(char* buf, __int32 size);
    __int32 Read(char* buf, __int32 size);
    bool Peek(char* buf, __int32 size);
    __int32 GetWriteableSize() { return _capacity - _tail; }
    __int32 GetReadableSize() { return _tail - _head; }
    char* GetWritePosition() { return &_buf[_tail]; }
    char* GetReadPosition() { return &_buf[_head]; }

    char _buf[MAX_UNIT_BUFFER_SIZE + 1];
    __int32 _capacity;
    __int32 _head;
    __int32 _tail;
};

struct BABufferUnitNode : public BAMemoryPool<BABufferUnitNode, MAX_INIT_BUFFER_UNIT>
{
public:
    BABufferUnitNode() :_next(nullptr)
    {
        //std::cout << "생성" << std::endl;
    }
    ~BABufferUnitNode() {
        //std::cout << "해제" << std::endl;
    }


    std::shared_ptr<BABufferUnitNode> _next;
    BABufferUnit _buffer;
};

class BABufferContainer
{
public:
    BABufferContainer():_size(0)
    {
        _cur_read = _cur_write = _tail = std::make_shared<BABufferUnitNode>();
    }

    __int32 Write(void* buf, __int32 len);
    __int32 Read(void* buf, __int32 len);
    bool Peek(void* buf, __int32 len);
    bool Reserve(size_t length);
    bool PushNode();
    bool Readable(size_t size) { return _size >= (__int64)size; }
    bool Readable() { return _size > 0; }

protected:
    std::shared_ptr<BABufferUnitNode> _cur_read;
    std::shared_ptr<BABufferUnitNode> _cur_write;
    std::shared_ptr<BABufferUnitNode> _tail;

    __int64 _size;
};