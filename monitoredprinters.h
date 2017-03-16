#ifndef MONITOREDPRINTERS_H
#define MONITOREDPRINTERS_H
#include <map>
#include "printerinformation.h"

class MonitoredPrinters
{
private:
    std::map<std::wstring, PrinterInformation*> _PrinterList;

public:
    MonitoredPrinters();

    void Add(std::wstring DeviceName, PrinterInformation* _PrinterInformation);
    void Remove(std::wstring DeviceName);
};

#endif // MONITOREDPRINTERS_H
