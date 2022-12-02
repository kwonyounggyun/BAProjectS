#pragma once
#include <memory>

struct BABufferUnit : public BAMemoryPool<BABufferUnit, MAX_USER * 2>
{
public:
    BABufferUnit() :_capacity(sizeof(_buf)), _head(0), _tail(0) {}
    char _buf[MAX_UNIT_BUFFER_SIZE];
    int _capacity;
    int _head;
    int _tail;

    int Write(char* buf, int size);
    int Read(char* buf, int size);
    bool Peek(char* buf, int size);
    int GetWriteableSize() { return _capacity - _tail; }
    int GetReadableSize() { return _tail - _head; }
};

struct BABufferUnitNode
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
    BABufferContainer()
    {
        _head = _tail = std::make_shared<BABufferUnitNode>();
    }

    int Write(char* buf, int len);
    int Read(char* buf, int len);
    bool Peek(char* buf, int len);
    bool CheckReadableSize(int size);

private:
    std::shared_ptr<BABufferUnitNode> _head;
    std::shared_ptr<BABufferUnitNode> _tail;
};