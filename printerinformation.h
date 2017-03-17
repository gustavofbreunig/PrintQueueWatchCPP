#ifndef PRINTERINFORMATION_H
#define PRINTERINFORMATION_H

#include <Windows.h>
#include "printerchangenotificationthread.h"
#include "printereventqueue.h"
#include "printjob.h"
#include <pthread.h>
#include <functional>

//forward declaration to break cycle
class PrinterChangeNotificationThread;

class PrinterInformation
{
private:
    HANDLE mhPrinter;
    PrinterChangeNotificationThread* _NotificationThread = nullptr;
    pthread_t _thread;
    pthread_t _eventConsumer;
    std::vector<PrintJob*> _PrintJobs;
    void InitPrinterInfo();
    bool _Cancelled = false;
    std::function<void(PrintJobEventArgs*)>* JobEvent;

public:
    PRINTER_INFO_2* mPrinter_Info_2;
    PrinterEventQueue* EventQueue;
    PrinterInformation(std::wstring DeviceName,
                       ACCESS_MASK DesiredAccess,
                       std::function<void(PrintJobEventArgs*)>* JobEvent);
    bool IsNetworkPrinter();
    void SetMonitored(bool value);
    ~PrinterInformation();
    void CancelWatching();
    bool isCancelled();
    void addPrintJob(PrintJob *job);
    PrintJob* getItemByJobId(uint32_t jobId);
    bool PrintJobExists(uint32_t jobId);

};

#endif // PRINTERINFORMATION_H
