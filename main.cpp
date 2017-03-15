#include <iostream>
#include "printermonitor.h"
#include "printserver.h"

void JobEventFired(PrintJobEventArgs* args)
{
	std::cout << "event called: " << args->getEventName() << std::endl;
	std::cout << "Job ID: " << args->getPrintJob()->getJobId() << ", ";
	std::cout << "pages: " << args->getPrintJob()->getTotalPages();
	std::cout << ", pages printed: " << args->getPrintJob()->getPagesPrinted();
	std::cout << ", copies: " << args->getPrintJob()->getCopies() << std::endl;
	std::wcout << L"Document: " << args->getPrintJob()->getDocument() << std::endl;
	std::cout << std::endl;
}

int main(int argc, char* argv[])
{
	//create monitoring instance
    PrinterMonitor* pmon = new PrinterMonitor();

	//set events
    pmon->JobDeleted = &JobEventFired;
    pmon->JobAdded = &JobEventFired;
    pmon->JobSet = &JobEventFired;
    pmon->JobWritten = &JobEventFired;

	//add printers
    PrintServer pserver;
    std::vector<std::wstring> printers = pserver.getPrinters();
    for (const std::wstring &it: printers) //access by const reference
    {
		std::wcout << L"monitoring: " << it.c_str() << std::endl;
        pmon->AddPrinter(it);
    }
	
	std::cout << "Press any key to exit" << std::endl;
	//pause this thread;
	std::cin.get();

    for (const std::wstring &it: printers) //access by const reference
    {
        std::wcout << L"stop monitoring: " << it.c_str() << std::endl;
        pmon->RemovePrinter(it);
    }

	delete pmon;

	return 0;
}