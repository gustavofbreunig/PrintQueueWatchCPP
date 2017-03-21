CC=g++
CFLAGS= -std=c++11 -Wall -O3 -DUNICODE -static-libgcc -static-libstdc++ -c

LDFLAGS= -lwinspool
SOURCES= main.cpp \
	monitoredprinters.cpp \
	printerchangenotificationthread.cpp \
	printereventqueue.cpp \
	printerinformation.cpp \
	printermonitor.cpp \
	printjob.cpp \
	printjobeventargs.cpp \
	printserver.cpp

OBJECTS=$(SOURCES:.cpp=.o)
EXECUTABLE=queuewatch.exe

all: $(SOURCES) $(EXECUTABLE)

$(EXECUTABLE): $(OBJECTS) 
    $(CC) $(OBJECTS) -o $@ $(LDFLAGS)
	
.cpp.o:
    $(CC) $(CFLAGS) $< -o $@

clean: 
	del *.o
	del $(EXECUTABLE)