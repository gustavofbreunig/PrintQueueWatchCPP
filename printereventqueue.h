#ifndef PRINTEREVENTQUEUE_H
#define PRINTEREVENTQUEUE_H

#include <queue>
#include <string>
#include "pthread.h"
#include "windows.h"
#include "printjobeventargs.h"
#include <functional>
#include <iostream>

// Queues printer events and printjob events to be
// raised asynchronously thus allowing the printer monitor
// thread to get on with its business while existing events are
// being processed.
// adapted from (c) 2003 Merrion Computing Ltd

class PrinterEventQueue
{
private:
    std::queue<PrintJobEventArgs> _queue;
    pthread_mutex_t _mutex;
    pthread_cond_t _cond;
    bool _Cancelled = false;
    void StartThread();
    std::function<void(PrintJobEventArgs*)>* JobEvent;

public:
    PrinterEventQueue(std::function<void(PrintJobEventArgs*)>* JobEvent);
    ~PrinterEventQueue();
    void AddJobEvent(PrintJobEventArgs JobEventArgs);
    void Awaken();
    void CancelConsumerThread();
    static void* staticStartThread(void* printerEventQueueClass);
};

#endif // PRINTEREVENTQUEUE_H
