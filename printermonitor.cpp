#include "printermonitor.h"

PrinterMonitor::PrinterMonitor()
{
    JobEvent = [this](PrintJobEventArgs* args) {
        //check the event and if the function is callable
        if (this->JobAdded && args->getEventType() == PrintJobEventTypes::JobAddedEvent)
        {
            this->JobAdded(args);
        } else if (this->JobSet && args->getEventType() == PrintJobEventTypes::JobSetEvent)
        {
            this->JobSet(args);
        } else if (this->JobWritten && args->getEventType() == PrintJobEventTypes::JobWrittenEvent)
        {
            this->JobWritten(args);
        } else if (this->JobDeleted && args->getEventType() == PrintJobEventTypes::JobDeletedEvent)
        {
            this->JobDeleted(args);
        }
    };
}

PrinterMonitor::~PrinterMonitor()
{
    PrinterChangeNotificationThread::DestroyPthreadMutex();
    delete _MonitoredPrinters;
}

void PrinterMonitor::RemovePrinter(std::wstring DeviceName)
{
    _MonitoredPrinters->Remove(DeviceName);
}

void PrinterMonitor::AddPrinter(std::wstring DeviceName)
{
    if (_MonitoredPrinters == NULL)
    {
        _MonitoredPrinters = new MonitoredPrinters();
        //initialize mutex inside thread class
        int mutex_printer_change_error = PrinterChangeNotificationThread::InitializePthreadMutex();
        if (mutex_printer_change_error)
        {
            std::string err("pthred_mutex_init error: ");
            err += std::to_string(mutex_printer_change_error);
            throw std::runtime_error(err.c_str());
        }
    }

    //is a network printer?
    PrinterInformation piTest(DeviceName, PRINTER_ACCESS_USE | READ_CONTROL, NULL);

    if (piTest.IsNetworkPrinter())
    {
        _MonitoredPrinters->Add(DeviceName, new PrinterInformation(DeviceName, PRINTER_ALL_ACCESS | SERVER_ALL_ACCESS | READ_CONTROL, &JobEvent));
    }
    else
    {
        _MonitoredPrinters->Add(DeviceName, new PrinterInformation(DeviceName, PRINTER_ALL_ACCESS | READ_CONTROL, &JobEvent));
    }
}
