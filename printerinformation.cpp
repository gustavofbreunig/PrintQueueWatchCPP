#include "printerinformation.h"

void PrinterInformation::SetMonitored(bool value)
{
	if (value)
	{
		if (_NotificationThread == NULL)
		{
			_Cancelled = false;
			//create the eventqueue
			EventQueue = new PrinterEventQueue(JobEvent);
			//create the eventqueue consumer thread
			pthread_create(&_eventConsumer, NULL, PrinterEventQueue::staticStartThread, EventQueue);

			_NotificationThread = new PrinterChangeNotificationThread(mhPrinter, this);
			//create the notification thread consumer
			pthread_create(&_thread, NULL, PrinterChangeNotificationThread::staticStartThread, _NotificationThread);
		}
	}
	else
	{
		if (!_Cancelled)
		{
			_Cancelled = true;
			//Stop monitoring the printer
			if (!FindClosePrinterChangeNotification(_NotificationThread->mhWait))
			{
				std::string err("FindClosePrinterChangeNotification() failed: error ");
				err += std::to_string(GetLastError());
				err += " on printer ";
				char printerName[wcslen(mPrinter_Info_2->pPrinterName) + 1];
				wcstombs(printerName, mPrinter_Info_2->pPrinterName, wcslen(mPrinter_Info_2->pPrinterName));
				err += printerName;
				throw std::runtime_error(err.c_str());
			}

			//Wait for the monitoring thread to terminate
			pthread_join(_thread, NULL);

			EventQueue->CancelConsumerThread();
			//wait for the eventqueue consumer to terminate
			pthread_join(_eventConsumer, NULL);

			delete EventQueue;
			delete _NotificationThread;
			std::wcout << L"All resources released in " << mPrinter_Info_2->pPrinterName << std::endl;
		}
	}
}

void PrinterInformation::InitPrinterInfo()
{
	DWORD BytesWritten = 0;
	BYTE* ptBuf = (BYTE*)malloc(1);
	//pega o tamanho de buffer necessario na primeira chamada
	if (!GetPrinter(mhPrinter, 2, ptBuf, 1, &BytesWritten))
	{
		free(ptBuf); //já sei quantos bytes preciso, aloco memória novamente
		ptBuf = (BYTE*)malloc(BytesWritten);
		if (GetPrinter(mhPrinter, 2, ptBuf, BytesWritten, &BytesWritten))
		{
			mPrinter_Info_2 = (PRINTER_INFO_2*)ptBuf;
		}
		else
		{
			std::string err("InitPrinterInfo error: ");
			err += std::to_string(GetLastError());
			throw std::runtime_error(err.c_str());
		}
	}
	else
	{
		std::string err("InitPrinterInfo error: ");
		err += std::to_string(GetLastError());
		throw std::runtime_error(err.c_str());
	}
}

PrinterInformation::PrinterInformation(std::wstring DeviceName, ACCESS_MASK DesiredAccess, std::function<void (PrintJobEventArgs *)> *JobEvent)
{
	this->JobEvent = JobEvent;

	WCHAR* dev = &DeviceName[0];

	PRINTER_DEFAULTS def;
	def.DesiredAccess = DesiredAccess;
	def.pDatatype = 0;
	def.pDevMode = 0;

	if (OpenPrinter(dev, &mhPrinter, &def))
	{
		InitPrinterInfo();
	}
	else
	{
		def.DesiredAccess = PRINTER_ALL_ACCESS;
		if (OpenPrinter(dev, &mhPrinter, &def))
		{
			InitPrinterInfo();
		}
		else if (OpenPrinter(dev, &mhPrinter, NULL))
		{
			InitPrinterInfo();
		}
		else
		{
			std::string err("OpenPrinter() failed for printer ");
			char devName[DeviceName.length() + 1];
			std::wcstombs(devName, DeviceName.c_str(), DeviceName.length() + 1);
			err += devName;
			err += ", error: ";
			err += std::to_string(GetLastError());
			throw std::runtime_error(err.c_str());
		}
	}
}

bool PrinterInformation::IsNetworkPrinter()
{
	return mPrinter_Info_2->Attributes & PRINTER_ATTRIBUTE_NETWORK;
}

PrinterInformation::~PrinterInformation()
{
	ClosePrinter(mhPrinter);
	free(mPrinter_Info_2);
}

void PrinterInformation::CancelWatching()
{
	_Cancelled = true;
}

bool PrinterInformation::isCancelled()
{
	return _Cancelled;
}

void PrinterInformation::addPrintJob(PrintJob* job)
{
	_PrintJobs.push_back(job);
}

PrintJob* PrinterInformation::getItemByJobId(uint32_t jobId)
{
	int i = 0;
	std::vector<PrintJob*>::iterator iter;

	for (iter = _PrintJobs.begin(); iter < _PrintJobs.end(); iter++)
	{
		PrintJob* pJob = *iter;
		if (pJob->getJobId() == jobId)
		{
			break;
		}
		i++;
	}

	return _PrintJobs.at(i);
}

bool PrinterInformation::PrintJobExists(uint32_t jobId)
{
	std::vector<PrintJob*>::iterator iter;
	for (iter = _PrintJobs.begin(); iter < _PrintJobs.end(); iter++)
	{
		PrintJob* pJob = *iter;
		if (pJob->getJobId() == jobId)
		{
			return true;
		}
	}

	return false;
}
