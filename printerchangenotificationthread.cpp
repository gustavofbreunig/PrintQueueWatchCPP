#include "printerchangenotificationthread.h"

PrinterChangeNotificationThread::PrinterChangeNotificationThread(HANDLE PrinterHandle, PrinterInformation* PrinterInformation)
{
    this->_PrinterHandle = PrinterHandle;
    this->_PrinterInformation = PrinterInformation;
}

void* PrinterChangeNotificationThread::staticStartThread(void *notificationThreadClass)
{
    ((PrinterChangeNotificationThread*) notificationThreadClass)->StartThread();
    return NULL;
}

//static variables need to be declared in one source file declaration
pthread_mutex_t PrinterChangeNotificationThread::_NotificationLock;

int PrinterChangeNotificationThread::InitializePthreadMutex()
{
    return pthread_mutex_init(&_NotificationLock, NULL);
}

int PrinterChangeNotificationThread::DestroyPthreadMutex()
{
    return pthread_mutex_destroy(&_NotificationLock);
}

void PrinterChangeNotificationThread::StartThread()
{
    PRINTER_NOTIFY_OPTIONS _PrinterNotifyOptions;
    PRINTER_NOTIFY_OPTIONS_TYPE pJobTypes[1];

    _PrinterNotifyOptions.Version = 2;
    _PrinterNotifyOptions.Count = 1;
    _PrinterNotifyOptions.pTypes = pJobTypes;

    WORD PrintJobFields[JOB_FIELDS_COUNT];
    PrintJobFields[0] = JOB_NOTIFY_FIELD_DOCUMENT;
    PrintJobFields[1] = JOB_NOTIFY_FIELD_STATUS;
    PrintJobFields[2] = JOB_NOTIFY_FIELD_MACHINE_NAME;
    PrintJobFields[3] = JOB_NOTIFY_FIELD_PORT_NAME;
    PrintJobFields[4] = JOB_NOTIFY_FIELD_USER_NAME;
    PrintJobFields[5] = JOB_NOTIFY_FIELD_NOTIFY_NAME;
    PrintJobFields[6] = JOB_NOTIFY_FIELD_DATATYPE;
    PrintJobFields[7] = JOB_NOTIFY_FIELD_PRINT_PROCESSOR;
    PrintJobFields[8] = JOB_NOTIFY_FIELD_PARAMETERS;
    PrintJobFields[9] = JOB_NOTIFY_FIELD_DRIVER_NAME;
    PrintJobFields[10] = JOB_NOTIFY_FIELD_DEVMODE;
    PrintJobFields[11] = JOB_NOTIFY_FIELD_STATUS_STRING;
    PrintJobFields[12] = JOB_NOTIFY_FIELD_SECURITY_DESCRIPTOR;
    PrintJobFields[13] = JOB_NOTIFY_FIELD_PRINTER_NAME;
    PrintJobFields[14] = JOB_NOTIFY_FIELD_PRIORITY;
    PrintJobFields[15] = JOB_NOTIFY_FIELD_POSITION;
    PrintJobFields[16] = JOB_NOTIFY_FIELD_SUBMITTED;
    PrintJobFields[17] = JOB_NOTIFY_FIELD_START_TIME;
    PrintJobFields[18] = JOB_NOTIFY_FIELD_UNTIL_TIME;
    PrintJobFields[19] = JOB_NOTIFY_FIELD_TIME;
    PrintJobFields[20] = JOB_NOTIFY_FIELD_TOTAL_PAGES;
    PrintJobFields[21] = JOB_NOTIFY_FIELD_PAGES_PRINTED;
    PrintJobFields[22] = JOB_NOTIFY_FIELD_TOTAL_BYTES;
    PrintJobFields[23] = JOB_NOTIFY_FIELD_BYTES_PRINTED;

    pJobTypes[0].Type = JOB_NOTIFY_TYPE;
    pJobTypes[0].Count = JOB_FIELDS_COUNT;
    pJobTypes[0].pFields = PrintJobFields;

    mhWait = FindFirstPrinterChangeNotification(_PrinterHandle, PRINTER_CHANGE_JOB, 0, &_PrinterNotifyOptions);

    if (mhWait == INVALID_HANDLE_VALUE)
    {
        _PrinterInformation->CancelWatching();
        std::string err("FindFirstPrinterChangeNotification failed, error: ");
        err += std::to_string(GetLastError());
        throw std::runtime_error(err.c_str());
    }

    while (!_PrinterInformation->isCancelled())
    {
        WaitForSingleObject(mhWait, INFINITE);

        if (_PrinterInformation->isCancelled())
        {
            std::wcout << L"Cancelled monitoring on " << _PrinterInformation->mPrinter_Info_2->pPrinterName << std::endl;
        }
        else
        {
            //Prevent this code being re-entrant...
            pthread_mutex_lock(&_NotificationLock);

            DWORD pdwChange;
            PRINTER_NOTIFY_INFO* info = new PRINTER_NOTIFY_INFO();

            _PrinterNotifyOptions.Flags = 0;

            if (FindNextPrinterChangeNotification(mhWait, &pdwChange, &_PrinterNotifyOptions, (LPVOID*)&info))
            {
                // If the flags indicate that there was insufficient space to store all
                // the changes we need to ask again
                if (info && info->Flags & PRINTER_NOTIFY_INFO_DISCARDED)
                {
                    _PrinterNotifyOptions.Flags = _PrinterNotifyOptions.Flags | PRINTER_NOTIFY_OPTIONS_REFRESH;
                    if (!FindNextPrinterChangeNotification(mhWait, &pdwChange, &_PrinterNotifyOptions,(LPVOID*) &info))
                    {
                        std::string err("FindNextPrinterChangeNotification failed to PRINTER_NOTIFY_OPTIONS_REFRESH, error: ");
                        err += std::to_string(GetLastError());
                        throw std::runtime_error(err.c_str());
                    }
                    else
                    {
                        _PrinterNotifyOptions.Flags = _PrinterNotifyOptions.Flags & (~PRINTER_NOTIFY_OPTIONS_REFRESH);
                    }
                }

                //list of printjobs changed in event (by id)
                std::set<uint32_t> colPrintJobs;
                if (info)
                {
                    for (uint32_t i = 0; i < info->Count; i++)
                    {
                        uint32_t jobId = info->aData[i].Id;
                        PrintJob* thisJob;

                        if (_PrinterInformation->PrintJobExists(jobId))
                        {
                            thisJob = _PrinterInformation->getItemByJobId(jobId);
                        }
                        else
                        {
                            // If this job is not on the printer job list, add it and populate with all info...
                            thisJob = new PrintJob(_PrinterHandle, jobId);
                            thisJob->RefreshJobInfo();

                            _PrinterInformation->addPrintJob(thisJob);
                        }
                        colPrintJobs.insert(info->aData[i].Id); //std::set never repeat itens

                        //some info change during job's life
                        if (info->aData[i].Field == JOB_NOTIFY_FIELD_PAGES_PRINTED)
                        {
                            thisJob->setPagesPrinted(info->aData[i].NotifyData.adwData[0]);
                        } else if (info->aData[i].Field == JOB_NOTIFY_FIELD_TOTAL_PAGES)
                        {
                            thisJob->setTotalPages(info->aData[i].NotifyData.adwData[0]);
                        } else if (info->aData[i].Field == JOB_NOTIFY_FIELD_TOTAL_BYTES)
                        {
                            thisJob->setJobSize(info->aData[i].NotifyData.adwData[0]);
                        } else if (info->aData[i].Field == JOB_NOTIFY_FIELD_STATUS_STRING)
                        {
                            thisJob->setStatusDescription((WCHAR*)info->aData[i].NotifyData.Data.pBuf);
                        } else if (info->aData[i].Field == JOB_NOTIFY_FIELD_STATUS)
                        {
                            thisJob->setStatus(info->aData[i].NotifyData.adwData[0]);
                        }

                    }

                    FreePrinterNotifyInfo(info);
                    delete info;
                }

                //Raise the appropriate event...
                if (pdwChange)
                {
                    //pdwChange set - changes have occured

                    std::set<uint32_t>::iterator it;
                    for (it = colPrintJobs.begin(); it != colPrintJobs.end(); it++)
                    {
                        PrintJob* thisJob = _PrinterInformation->getItemByJobId(*it);

                        if (pdwChange & PRINTER_CHANGE_ADD_JOB)
                        {
                            PrintJobEventArgs e(thisJob, PrintJobEventTypes::JobAddedEvent);
                            _PrinterInformation->EventQueue->AddJobEvent(e);
                        }

                        if (pdwChange & PRINTER_CHANGE_SET_JOB)
                        {
                            PrintJobEventArgs e(thisJob, PrintJobEventTypes::JobSetEvent);
                            _PrinterInformation->EventQueue->AddJobEvent(e);
                        }

                        if (pdwChange & PRINTER_CHANGE_WRITE_JOB)
                        {
                            PrintJobEventArgs e(thisJob, PrintJobEventTypes::JobWrittenEvent);
                            _PrinterInformation->EventQueue->AddJobEvent(e);
                        }

                        if (pdwChange & PRINTER_CHANGE_DELETE_JOB)
                        {
                            PrintJobEventArgs e(thisJob, PrintJobEventTypes::JobDeletedEvent);
                            _PrinterInformation->EventQueue->AddJobEvent(e);
                        }
                    }
                }

                _PrinterInformation->EventQueue->Awaken();

            }
            else
            {
                std::string err("FindNextPrinterChangeNotification failed, error: ");
                err += std::to_string(GetLastError());
                throw std::runtime_error(err.c_str());
            }

            pthread_mutex_unlock(&_NotificationLock);
        }
    }
}
