#ifndef PRINTJOB_H
#define PRINTJOB_H

#include "windows.h"
#include "stdint.h"
#include <string>
#include <stdexcept>

class PrintJob
{
private:
    HANDLE mhPrinter;
    uint32_t midJob = 0;
    JOB_INFO_1* ji1 = nullptr;
    JOB_INFO_2* ji2 = nullptr;

public:
    PrintJob(HANDLE mhPrinter, uint32_t dwJobId);
    ~PrintJob();
    void RefreshJobInfo();
    uint32_t getJobId();
    std::wstring getPrinterName();
    std::wstring getUserName();
    std::wstring getMachineName();
    std::wstring getDocument();
    std::wstring getStatusDescription();
    void setStatusDescription(WCHAR* desc);
    std::wstring getDataType();
    std::wstring getNotifyUserName();
    std::wstring getPrintProcessoName();
    std::wstring getDriverName();
    std::wstring getParameters();
    int32_t getPagesPrinted();
    void setPagesPrinted(int32_t pages);
    int32_t getPosition();
    int32_t getTotalPages();
    void setTotalPages(int32_t pages);
    int32_t getPriority();
    int32_t getQueuedTime();
    int32_t getJobSize();
    void setJobSize(int32_t size);
    int16_t getPaperKind();
    int16_t getPaperWidth();
    int16_t getPaperLength();
    int16_t getPaperSource();
    int16_t getPrinterResolution();
    int16_t getCopies();
    bool isLandscape();
    bool isColor();
    bool isDuplex();
    bool isDeleted();
    bool isDeleting();
    bool isPrinted();
    bool isPrinting();
    bool isInError();
    bool isOffline();
    bool isPaperOut();
    bool isUserInterventionRequired();
    bool isSpooling();
    void setStatus(int32_t status);
    SYSTEMTIME getSubmitted();
};

#endif // PRINTJOB_H
