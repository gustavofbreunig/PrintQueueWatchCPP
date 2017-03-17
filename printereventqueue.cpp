#include "printereventqueue.h"

PrinterEventQueue::PrinterEventQueue(std::function<void (PrintJobEventArgs *)> *JobEvent)
{
    this->JobEvent = JobEvent;

    int mutex_init = pthread_mutex_init(&_mutex, NULL);
    if (mutex_init)
    {
        std::string err("pthread_mutex_init failed, error code: ");
        err += std::to_string(mutex_init);
        throw std::runtime_error(err.c_str());
    }

    int cond_init = pthread_cond_init(&_cond, NULL);
    if (cond_init)
    {
        std::string err("pthread_cond_init failed, error code: ");
        err += std::to_string(cond_init);
        throw std::runtime_error(err.c_str());
    }
}

PrinterEventQueue::~PrinterEventQueue()
{
    int mutex_destroy = pthread_mutex_destroy(&_mutex);
    if (mutex_destroy)
    {
        std::cout << "pthread_mutex_destroy failed, error code: " << mutex_destroy << std::endl;
    }

    int cond_destroy = pthread_cond_destroy(&_cond);
    if (cond_destroy)
    {
        std::cout << "pthread_cond_destroy failed, error code: " << cond_destroy << std::endl;
    }
}

void PrinterEventQueue::AddJobEvent(PrintJobEventArgs JobEventArgs)
{
    pthread_mutex_lock(&_mutex);
    _queue.push(JobEventArgs);
    pthread_mutex_unlock(&_mutex);
}

void PrinterEventQueue::Awaken()
{
    pthread_mutex_lock(&_mutex);
    if (_queue.size() > 0)
    {
        pthread_mutex_unlock(&_mutex);
        pthread_cond_signal(&_cond);
    }
    else
    {
        pthread_mutex_unlock(&_mutex);
    }
}

void PrinterEventQueue::CancelConsumerThread()
{
    _Cancelled = true;
    pthread_cond_signal(&_cond);
}

void* PrinterEventQueue::staticStartThread(void *printerEventQueueClass)
{
    ((PrinterEventQueue*)printerEventQueueClass)->StartThread();
    return NULL;
}

void PrinterEventQueue::StartThread()
{
    while (!_Cancelled)
    {
        pthread_mutex_lock(&_mutex);
        pthread_cond_wait(&_cond, &_mutex);
        pthread_mutex_unlock(&_mutex);

        pthread_mutex_lock(&_mutex);
        std::function<void(PrintJobEventArgs *)> event = *JobEvent;
        while (_queue.size() > 0)
        {
            PrintJobEventArgs e = _queue.front();
            event(&e);
            _queue.pop();
        }
        pthread_mutex_unlock(&_mutex);
    }
}


