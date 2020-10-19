#pragma once
#include "pch.h"
class Pipe {
private:
    int allocatedSize;    // total bytes allocated
    int bytesInBuffer;    // actual bytes received
    char* buf;
    HANDLE pipe;

    // ... other stuff as needed
public:
    Pipe() {
        allocatedSize = BUF_SIZE;
        buf = malloc(allocatedSize);
    }
    ~Pipe() { free(buf); }

    void Connect(DWORD pid, int robotID) {
        sprintf_s(robotPipeName, "\\\\.\\pipe\\CC-%X-robot-%X", pid, robotID);
        pipe = CreateFile(robotPipeName, GENERIC_READ | GENERIC_WRITE,
            0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
    };

    void Send(char* buf) {
        BOOL sent = WriteFile(pipe, buf, allocatedSize, &bytesInBuffer, NULL);
        if (sent == FAILURE) {
            printf("Error %d sending message to Robot\n", GetLastError());
            exit(-1);
        }
    };

    void Read(void);   // reads one full message
};

void Pipe::Read(void)
{
    BOOL read = ReadFile(pipe, buf, allocatedSize, &bytesInBuffer, NULL);	// need error handling
    if (read == FAILURE) {
        printf("Error %d receiving message from Robot\n", GetLastError());
        exit(-1);
    }
    if (bytesInBuffer == allocatedSize)
    {
        PeekNamedPipe(pipe, buf, allocatedSize, &remainder); // need error handling
        if (remainder > 0)
        {
            allocatedSize += remainder;    // alternative: double the size
            buf = realloc(buf, allocatedSize);
            ReadFile(pipe, buf + bytesInBuffer, remainder, &bytesInBuffer); // need error handling
            bytesInBuffer = allocatedSize;
        }
    }
}



