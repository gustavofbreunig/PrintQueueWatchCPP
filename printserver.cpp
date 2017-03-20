#include "printserver.h"

PrintServer::PrintServer()
{

}

std::vector<std::wstring> PrintServer::getPrinters()
{
    DWORD pcbNeeded, pcbReturned;

    //get memory needed
    EnumPrinters(PRINTER_ENUM_CONNECTIONS | PRINTER_ENUM_LOCAL, NULL, 2, NULL, 0, &pcbNeeded, &pcbReturned);

    //fill memory
    LPBYTE pPrinters = new BYTE[pcbNeeded];
    if (!EnumPrinters(PRINTER_ENUM_CONNECTIONS | PRINTER_ENUM_LOCAL, NULL, 2, pPrinters, pcbNeeded, &pcbNeeded, &pcbReturned))
    {
        std::string err("EnumPrinters error: " );
        err += std::to_string(GetLastError());
        throw std::runtime_error(err.c_str());
    }

    PRINTER_INFO_2* printers = (PRINTER_INFO_2*)pPrinters;
    std::vector<std::wstring> buffer;
    for (uint32_t i = 0; i < pcbReturned; i++)
    {
        std::wstring ws(printers[i].pPrinterName);
        buffer.push_back(ws);
    }

    delete[] pPrinters;
    return buffer;
}
