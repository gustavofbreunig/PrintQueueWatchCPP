#ifndef PRINTERCHANGENOTIFICATIONTHREAD_H
#define PRINTERCHANGENOTIFICATIONTHREAD_H

#include "windows.h"
#include "printerinformation.h"
#include <pthread.h>
#include <set>
#include "printjobeventargs.h"
#include <exception>
#include <iostream>

#define JOB_FIELDS_COUNT 24

//forward declaration to break cycle
class PrinterInformation;

class PrinterChangeNotificationThread
{
private:
    HANDLE _PrinterHandle;
    PrinterInformation* _PrinterInformation;
    static pthread_mutex_t _NotificationLock;
    void StartThread();

public:
    HANDLE mhWait;
    PrinterChangeNotificationThread(HANDLE PrinterHandle, PrinterInformation* PrinterInformation);
    static int InitializePthreadMutex();
    static int DestroyPthreadMutex();
    static void* staticStartThread(void* notificationThreadClass);
};

#endif // PRINTERCHANGENOTIFICATIONTHREAD_H
