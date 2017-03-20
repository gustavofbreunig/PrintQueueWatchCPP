#ifndef PRINTJOBEVENTARGS_H
#define PRINTJOBEVENTARGS_H
#include "printjob.h"


enum class PrintJobEventTypes
{
    JobAddedEvent = 1,
    JobSetEvent = 2,
    JobWrittenEvent = 3,
    JobDeletedEvent = 4	
};

class PrintJobEventArgs
{
private:
    PrintJob* _Job;
    PrintJobEventTypes _EventType;

public:
    PrintJobEventArgs(PrintJob* Job, PrintJobEventTypes EventType);
    ~PrintJobEventArgs();
    PrintJob* getPrintJob();
    PrintJobEventTypes getEventType();
	std::string getEventName();

};

#endif // PRINTJOBEVENTARGS_H
