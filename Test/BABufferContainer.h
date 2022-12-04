#pragma once
#include <memory>

struct BABufferUnit
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

    void GetRemainBufInfo(__out char** buf, __out unsigned long& size);
};

class BABufferContainer
{
public:
    BABufferContainer()
    {
        _head = _tail = std::make_shared<BABufferUnitNode>();
    }

    int Write(char* buf, int len);
    void Write(const std::shared_ptr<BABufferUnitNode>& buffer_unit);
    int Read(char* buf, int len);
    bool Peek(char* buf, int len);
    bool CheckReadableSize(int size);
    std::shared_ptr<BABufferUnitNode>& GetTail() { return _tail; }

private:
    std::shared_ptr<BABufferUnitNode> _head;
    std::shared_ptr<BABufferUnitNode> _tail;
};