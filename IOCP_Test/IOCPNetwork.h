#pragma once
#include "NetworkInterface.h"

class IOCPNetwork : NetworkInterface
{
public:
    IOCPNetwork();
    ~IOCPNetwork();

    bool CreaeteWorkerThread();
    void WorkerThread();
    
public:
    // NetworkInterface��(��) ���� ��ӵ�
    virtual bool Initialize() override;
    virtual void StartNetwork() override;
};

