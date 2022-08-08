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
    // NetworkInterface을(를) 통해 상속됨
    virtual bool Initialize() override;
    virtual void StartNetwork() override;
};

