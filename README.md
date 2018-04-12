# PrintQueueWatchCPP

![demo](https://raw.githubusercontent.com/gustavofbreunig/PrintQueueWatchCPP/master/demo.gif "Demo")


This project is based on PrintQueueWatch (https://printqueuewatch.codeplex.com) made by Duncan Jones (http://www.merrioncomputing.com/)
for printer monitoring, it can log every print job in Windows printers.

The original project was written in VB.NET, but for compatibilty purposes, I needed something that don't relies on .NET Framework, so, I rewrote
the basic functionality in C++.

Compatible OS:
* Windows XP/Vista/Seven/10

Dependencies:
* POSIX Threads

Supported Compilers:
* GCC for windows (mingw-w64: https://mingw-w64.org) (but every C++11 compiler should work)

How to use it: 
Just add source files (.cpp and .h) to your project and build, use as in main.cpp
For a functional example:
```
make
```

Execute the .exe, then print something.