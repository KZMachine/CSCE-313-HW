// Karim Zaher
// CSCE 313 - 201
// Fall 2020

#include "pch.h"

int main(int argc, char *argv[])
{
    // ----- SETUP ----- //
    if (argc != 4) {
        printf("Usage: file_name planet_number cave_number num_threads");
        exit(-1);
    }
    int planetNumber = std::stoi(argv[1]);
    int caveNumber = std::stoi(argv[2]);
    int NUMROBOTS = std::stoi(argv[3]);
    if (planetNumber > 32 || planetNumber < 20) {
        printf("Invalid planet number(Use Planets 20-32)");
        exit(-1);
    }
    //printf("Starting CC.exe...\n");

    PROCESS_INFORMATION pi;
    STARTUPINFO s;
    
    GetStartupInfo(&s);

    char path[] = "CC.exe";
    
    clock_t tStart = clock();

    if (CreateProcess(path, NULL, NULL, NULL,
        false, 0, NULL, NULL, &s, &pi) == FALSE)
    {
        printf("Error %d starting CC\n", GetLastError());
        exit(-1);
    }

    char pipename[18];
    sprintf_s(pipename, 18, "\\\\.\\pipe\\CC-%X", pi.dwProcessId);

     //wait for the CC to initialize the pipe from its end
    while (WaitNamedPipe(pipename, INFINITE) == FALSE) {
        Sleep(100); // pause for 100 ms
    }
     
    HANDLE CC = CreateFile(pipename, GENERIC_READ | GENERIC_WRITE,
        0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
    

    CommandCC ccCommand(CONNECT, planetNumber, caveNumber, NUMROBOTS);
    ResponseCC ccResponse;
    
    DWORD numBytesWritten = 0;
    BOOL sendCC = WriteFile(CC, &ccCommand, sizeof(CommandCC), &numBytesWritten, NULL);
    if (sendCC == FAILURE) {
        printf("Error %d sending message to CC\n", GetLastError());
        exit(-1);
    }

    DWORD numBytesRead = 0;
    BOOL receiveCC = ReadFile(CC, &ccResponse, sizeof(ResponseCC), &numBytesRead, NULL);
    if (receiveCC == FAILURE) {
        printf("Error %d receiving message from CC\n", GetLastError());
        exit(-1);
    }
    
    //printf("%s\n", ccResponse.msg);
    printf("Opened (planet %d , cave %d) with %d robot(s)\n", planetNumber, caveNumber, NUMROBOTS);
    
    StatParam* statParam = new StatParam[1];
    UINT64 uPop = 0;
    UINT64 uSize = 0;
    UINT64 dSize = 0;
    UINT64 rooms = 0;
    UINT64 uniqueCount = 0;
    UINT64 totalCount = 1;
    int numberNodes = 0;
    int distance = 0;
    int threadActive = 0;
    int threadRunning = 0;
    bool finished = false;

    
    statParam->uPop = &uPop;
    statParam->uSize = &uSize;
    statParam->dSize = &dSize;
    statParam->rooms = &rooms;
    statParam->uniqueCount = &uniqueCount;
    statParam->totalCount = &totalCount;
    statParam->threadActive = &threadActive;
    statParam->threadRunning = &threadRunning;
    statParam->finished = &finished;
    HANDLE stats = CreateThread(NULL, 0, StatFun, statParam, 0, 0);
    SetThreadPriority(stats, THREAD_PRIORITY_ABOVE_NORMAL);
    

    CreatorParams* creationparams = new CreatorParams[1];
    creationparams->NUMROBOTS = NUMROBOTS;
    creationparams->planetNumber = planetNumber;
    creationparams->processID = pi.dwProcessId;
    creationparams->uPop = &uPop;
    creationparams->uSize = &uSize;
    creationparams->dSize = &dSize;
    creationparams->uniqueCount = &uniqueCount;
    creationparams->totalCount = &totalCount;
    creationparams->rooms = &rooms;
    creationparams->distance = &distance;
    creationparams->numberNodes = &numberNodes;
    creationparams->threadActive = &threadActive;
    creationparams->threadRunning = &threadRunning;
    creationparams->finished = &finished;
    HANDLE threadCreation = CreateThread(NULL, 0, ThreadCreator, creationparams, 0, NULL);

    //WaitForSingleObject(threadCreation, INFINITE);
    
    while (WaitForSingleObject(threadCreation, INFINITE)) {
        Sleep(500);
    }
    
    CloseHandle(stats);
    printf("Disconnecting CC...\n");
    ccCommand.setCommand(DISCONNECT);
    sendCC = WriteFile(CC, &ccCommand, sizeof(CommandCC), &numBytesWritten, NULL);
    printf("Waiting for CC.exe to quit...\n");

    CloseHandle(CC);
    WaitForSingleObject(pi.hProcess, INFINITE);
    CloseHandle(pi.hProcess);
    CloseHandle(pi.hThread);

    printf("Execution time %.2f seconds\n", ((double)clock() - tStart) / CLOCKS_PER_SEC);
}
