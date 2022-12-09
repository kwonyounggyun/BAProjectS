#pragma once
#include <memory>

struct BABufferUnit
{
public:
    BABufferUnit() :_capacity(sizeof(_buf)), _head(0), _tail(0) {}

    INT32 Write(UINT8* buf, INT32 size);
    INT32 Read(UINT8* buf, INT32 size);
    BOOL Peek(UINT8* buf, INT32 size);
    INT32 GetWriteableSize() { return _capacity - _tail; }
    INT32 GetReadableSize() { return _tail - _head; }
    UINT8* GetWritePosition() { return &_buf[_tail]; }
    UINT8* GetReadPosition() { return &_buf[_head]; }

private:
    UINT8 _buf[MAX_UNIT_BUFFER_SIZE + 1];
    INT32 _capacity;
    INT32 _head;
    INT32 _tail;
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
    BABufferContainer():size(0)
    {
        _cur_read = _cur_write = _tail = std::make_shared<BABufferUnitNode>();
    }

    INT32 Write(UINT8* buf, INT32 len);
    INT32 Read(UINT8* buf, INT32 len);
    BOOL Peek(UINT8* buf, INT32 len);
    BOOL Reserve(INT32 length);
    BOOL PushNode();

protected:
    std::shared_ptr<BABufferUnitNode> _cur_read;
    std::shared_ptr<BABufferUnitNode> _cur_write;
    std::shared_ptr<BABufferUnitNode> _tail;

    UINT64 size;
};