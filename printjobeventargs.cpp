#include "printjobeventargs.h"

PrintJobEventArgs::PrintJobEventArgs(PrintJob *Job, PrintJobEventTypes EventType)
{
	_Job = Job;
	_EventType = EventType;
}

PrintJobEventArgs::~PrintJobEventArgs()
{

}

PrintJob* PrintJobEventArgs::getPrintJob()
{
	return _Job;
}

PrintJobEventTypes PrintJobEventArgs::getEventType()
{
	return _EventType;
}

std::string PrintJobEventArgs::getEventName()
{
	switch ((int)_EventType)
	{
	case (int)PrintJobEventTypes::JobAddedEvent:
		return std::string("JobAdded");
		break;
	case (int)PrintJobEventTypes::JobSetEvent:
		return std::string("JobSet");
		break;
	case (int)PrintJobEventTypes::JobWrittenEvent:
		return std::string("JobWritten");
		break;
	case (int)PrintJobEventTypes::JobDeletedEvent:
		return std::string("JobDeleted");
		break;
	}

	return NULL;
}