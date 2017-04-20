#include "monitoredprinters.h"

MonitoredPrinters::MonitoredPrinters()
{
}

void MonitoredPrinters::Add(std::wstring DeviceName, PrinterInformation* _PrinterInformation)
{
	if (_PrinterList.count(DeviceName) == 0)
	{
		_PrinterList[DeviceName] = _PrinterInformation;
		_PrinterInformation->SetMonitored(true);
	}
}

void MonitoredPrinters::Remove(std::wstring DeviceName)
{
	std::map<std::wstring, PrinterInformation*>::iterator it;
	it = _PrinterList.find(DeviceName);
	it->second->SetMonitored(false);
	delete it->second;
	_PrinterList.erase(it);
}
