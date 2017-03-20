#ifndef PRINTERMONITOR_H
#define PRINTERMONITOR_H

#include "printerinformation.h"
#include "windows.h"
#include "monitoredprinters.h"
#include "printerchangenotificationthread.h"
#include <functional>
#include "printjobeventargs.h"

class PrinterMonitor
{
private:
    MonitoredPrinters* _MonitoredPrinters = nullptr;
    std::function<void(PrintJobEventArgs*)> JobEvent;

public:
    PrinterMonitor();
    ~PrinterMonitor();

    //Raised when a job is added to one of the print spool queues being monitored
    std::function<void(PrintJobEventArgs*)> JobAdded;

    //Raised when a job is removed from one of the print spool queues being monitored
    std::function<void(PrintJobEventArgs*)> JobDeleted;

    //Raised when a job is written to on one of the print spool queues being monitored
    std::function<void(PrintJobEventArgs*)> JobWritten;

    //Raised when a job's properties are changed in one of the print spool queues being monitored
    std::function<void(PrintJobEventArgs*)> JobSet;

    void AddPrinter(std::wstring DeviceName);
    void RemovePrinter(std::wstring DeviceName);
};

#endif // PRINTERMONITOR_H
