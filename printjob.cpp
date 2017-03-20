#include "printjob.h"

PrintJob::PrintJob(HANDLE mhPrinter, uint32_t dwJobId)
{
    this->mhPrinter = mhPrinter;
    midJob = dwJobId;
}

PrintJob::~PrintJob()
{
    delete ji1;
    delete ji2;
}

void PrintJob::RefreshJobInfo()
{
    DWORD pcbNeeded = 0;

    if (ji2 == nullptr)
    {
        //Get the required buffer size into pcbNeeded
        if (!GetJob(mhPrinter, midJob, 2, (BYTE*)ji2, 0, &pcbNeeded))
        {
            if (GetLastError() != ERROR_INSUFFICIENT_BUFFER) //it's expected this error
            {
                std::string err("GetJob for JOB_INFO_2 failed on first call :");
                err += std::to_string(GetLastError());
                throw std::runtime_error(err.c_str());
            }
        }

        //Allocate a buffer the right size
        ji2 = (JOB_INFO_2*)new BYTE[pcbNeeded];

        //Populate the JOB_INFO_1 structure
        if (!GetJob(mhPrinter, midJob, 2, (BYTE*)ji2, pcbNeeded, &pcbNeeded))
        {
            std::string err("GetJob for JOB_INFO_2 failed on second call :");
            err += std::to_string(GetLastError());
            throw std::runtime_error(err.c_str());
        }
    }

    if (ji1 == nullptr)
    {
        //Get the required buffer size into pcbNeeded
        if (!GetJob(mhPrinter, midJob, 1, (BYTE*)ji1, 0, &pcbNeeded))
        {
            if (GetLastError() != ERROR_INSUFFICIENT_BUFFER) //it's expected this error
            {
                std::string err("GetJob for JOB_INFO_1 failed on first call :");
                err += std::to_string(GetLastError());
                throw std::runtime_error(err.c_str());
            }
        }

        //Allocate a buffer the right size
        ji1 = (JOB_INFO_1*)new BYTE[pcbNeeded];

        //Populate the JOB_INFO_1 structure
        if (!GetJob(mhPrinter, midJob, 1, (BYTE*)ji1, pcbNeeded, &pcbNeeded))
        {
            std::string err("GetJob for JOB_INFO_1 failed on second call :");
            err += std::to_string(GetLastError());
            throw std::runtime_error(err.c_str());
        }
    }


}

uint32_t PrintJob::getJobId()
{
    return midJob;
}

std::wstring PrintJob::getPrinterName()
{
    //The name of the print device that this job is queued against
    std::wstring name(ji1->pPrinterName);
    return name;
}

std::wstring PrintJob::getUserName()
{
    //The name of the user that sent the print job for printing
    std::wstring name(ji1->pUserName);
    return name;
}

std::wstring PrintJob::getMachineName()
{
    //The name of the workstation that sent the print job to print
    std::wstring machine(ji1->pMachineName);
    return machine;
}

std::wstring PrintJob::getDocument()
{
    //The document name being printed

    // This value is set by the application which sends the job to be printed.  Many
    // applications put the application name at the start of the document name to aid
    // identification

    std::wstring document(ji1->pDocument);
    return document;
}

std::wstring PrintJob::getStatusDescription()
{
    //The description of the current status of the print job
    if (ji1->pStatus)
    {
        std::wstring status(ji1->pStatus);
        return status;
    }
    else
    {
        return std::wstring(L"");
    }
}

void PrintJob::setStatusDescription(WCHAR *desc)
{
    delete ji1->pStatus;
    ji1->pStatus = new WCHAR[wcslen(desc) + 1];
    wcscpy(ji1->pStatus, desc);
}

std::wstring PrintJob::getDataType()
{
    std::wstring datatype(ji1->pDatatype);
    return datatype;
}

std::wstring PrintJob::getNotifyUserName()
{
    //The user to notify about the progress of this print job

    std::wstring user(ji2->pNotifyName);
    return user;
}

std::wstring PrintJob::getPrintProcessoName()
{
    // The name of the print processor
    //which is responsible for printing this job

    std::wstring processor(ji2->pPrintProcessor);
    return processor;
}

std::wstring PrintJob::getDriverName()
{
    // The name of the printer driver
    // that is responsible for producing this print job

    std::wstring name(ji2->pDriverName);
    return name;
}

std::wstring PrintJob::getParameters()
{
    //Extra driver specific parameters for this print job

    std::wstring param(ji2->pParameters);
    return param;
}

int32_t PrintJob::getPagesPrinted()
{
    //Gets the number of pages in each job as it is deleted
    return ji1->PagesPrinted;
}

void PrintJob::setPagesPrinted(int32_t pages)
{
    ji1->PagesPrinted = pages;
}

int32_t PrintJob::getPosition()
{
    //The position of the job in the print device job queue
    return ji1->Position;
}

int32_t PrintJob::getTotalPages()
{
    //Number of pages in each new job as it is added to the monitored printers
    return ji2->TotalPages;
}

void PrintJob::setTotalPages(int32_t pages)
{
    ji2->TotalPages = pages;
}

int32_t PrintJob::getPriority()
{
    // The priority of this print job.  Higher priority jobs will be processed ahead
    // of lower priority jobs

    return ji2->Priority;
}

int32_t PrintJob::getQueuedTime()
{
    //Specifies the total time, in milliseconds, that has elapsed since the job began printing
    return ji2->Time;
}

int32_t PrintJob::getJobSize()
{
    //Specifies the size, in bytes, of the job

    //While the job is being spooled this will contain the current size of the spool file

    return ji2->Size;
}

void PrintJob::setJobSize(int32_t size)
{
    ji2->Size = size;
}

int16_t PrintJob::getPaperKind()
{
    //The paper type that the job is intended to be printed on
    //This could be a standard paper size (A4, A5 etc) or custom paper size if the printer

    return ji2->pDevMode->dmPaperSize;
}

int16_t PrintJob::getPaperWidth()
{
    //The width of the selected paper (if PaperKind is PaperKind.Custom)
    //This value is measured in millimeters

    return ji2->pDevMode->dmPaperWidth;
}

int16_t PrintJob::getPaperLength()
{
    //The height of the selected paper (if PaperKind is PaperKind.Custom)
    //This value is measured in millimeters

    return ji2->pDevMode->dmPaperLength;
}

int16_t PrintJob::getPaperSource()
{
    return ji2->pDevMode->dmDefaultSource;
}

int16_t PrintJob::getPrinterResolution()
{
    return ji2->pDevMode->dmPrintQuality;
}

int16_t PrintJob::getCopies()
{
    //The number of copies of each page to print

    // Some applications (Microsoft Word) misreport the number of copies to the spooler which will
    // result in incorrect values being returned

    if (ji2->pDevMode->dmCopies > 0)
    {
        return ji2->pDevMode->dmCopies;
    }
    else
    {
        if (ji2->PagesPrinted > ji2->TotalPages)
        {
            return ji2->PagesPrinted / ji2->TotalPages;
        }
        else
        {
            return 1;
        }
    }
}

bool PrintJob::isLandscape()
{
    return ji2->pDevMode->dmOrientation == DMORIENT_LANDSCAPE;
}

bool PrintJob::isColor()
{
    return ji2->pDevMode->dmColor == DMCOLOR_COLOR;
}

bool PrintJob::isDuplex()
{
    return ji2->pDevMode->dmDuplex == DM_DUPLEX;
}

bool PrintJob::isDeleted()
{
    //True if the print job has been deleted
    return (ji1->Status & JOB_STATUS_DELETED) == JOB_STATUS_DELETED;
}

bool PrintJob::isDeleting()
{
    //True if the print job is being deleted
    return (ji1->Status & JOB_STATUS_DELETING) == JOB_STATUS_DELETING;
}

bool PrintJob::isPrinted()
{
    //True if the job has been printed

    // This will be true once the job has been completely sent to the printer.  This
    // does not mean that the physical print out has necessarily appeared.
    return (ji1->Status & JOB_STATUS_PRINTED) == JOB_STATUS_PRINTED;
}

bool PrintJob::isPrinting()
{
    //True if the print job is currently printing
    return (ji1->Status & JOB_STATUS_PRINTING) == JOB_STATUS_PRINTING;
}

bool PrintJob::isInError()
{
    //True if there is an error with this print job that prevents it from
    return (ji1->Status & JOB_STATUS_ERROR) == JOB_STATUS_ERROR;
}

bool PrintJob::isOffline()
{
    //True if the job is in error because the printer is off line
    return (ji1->Status & JOB_STATUS_OFFLINE) == JOB_STATUS_OFFLINE;
}

bool PrintJob::isPaperOut()
{
    //True if the job is in error because the printer has run out of paper
    return (ji1->Status & JOB_STATUS_PAPEROUT) == JOB_STATUS_PAPEROUT;
}

bool PrintJob::isUserInterventionRequired()
{
    //True if the print job is in error because the printer requires user intervention

    //This can be caused by a print job that requires manual paper feed
    return (ji1->Status & JOB_STATUS_USER_INTERVENTION) == JOB_STATUS_USER_INTERVENTION;
}

bool PrintJob::isSpooling()
{
    //True if the print job is spooling to a spool file

    return (ji1->Status & JOB_STATUS_SPOOLING) == JOB_STATUS_SPOOLING;
}

void PrintJob::setStatus(int32_t status)
{
    ji1->Status = status;
}

SYSTEMTIME PrintJob::getSubmitted()
{
    //Specifies the date and time at which the job was submitted for printing

    //for info about SYSTEMTIME struct, see https://msdn.microsoft.com/pt-br/library/windows/desktop/ms724950(v=vs.85).aspx
    return ji2->Submitted;
}


