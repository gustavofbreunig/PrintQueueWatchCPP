#ifndef PRINTSERVER_H
#define PRINTSERVER_H
#include <vector>
#include <string>
#include <stdexcept>
#include "windows.h"

class PrintServer
{
public:
    PrintServer();
    std::vector<std::wstring> getPrinters();
};

#endif // PRINTSERVER_H
