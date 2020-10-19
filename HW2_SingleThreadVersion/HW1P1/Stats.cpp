#include "pch.h"

class CreatorParams {
public:
    int NUMROBOTS;
    int planetNumber;
    DWORD processID;
    UINT64* uPop;
    UINT64* uSize;
    UINT64* dSize;
    UINT64* rooms;
    UINT64* uniqueCount;
    UINT64* totalCount;
    int* distance;
    int* numberNodes;
    int* threadActive;
    int* threadRunning;
    bool* finished;
};

class StatParam {
public:
    UINT64* uPop;
    UINT64* uSize;
    UINT64* dSize;
    UINT64* rooms;
    UINT64* uniqueCount;
    UINT64* totalCount;
    int* threadActive;
    int* threadRunning;
    bool* finished;
};

DWORD __stdcall ThreadCreator(LPVOID p) {
    CreatorParams* x = (CreatorParams*)p;
    int NUMROBOTS = x->NUMROBOTS;
    int planetNumber = x->planetNumber;
    DWORD processID = x->processID;

    char robotPipeName[1024];

    HANDLE* thread = new HANDLE[NUMROBOTS];
    ThreadParams* t = new ThreadParams[NUMROBOTS];
    SearchThrough me;
    me.uPop = (x->uPop);
    me.uSize = (x->uSize);
    me.dSize = (x->dSize);
    me.numRooms = (x->rooms);
    me.uniqueCount = (x->uniqueCount);
    me.totalCount = (x->totalCount);
    me.threadActive = (x->threadActive);
    me.threadRunning =(x->threadRunning);


    me.NUMROBOTS = NUMROBOTS;
    me.planetNumber = planetNumber;
    Hash d;
    me.hashTable = &d;
    for (int i = 0; i < NUMROBOTS; i++) {
        t[i].threadID = i;
        t[i].me = &me;
        sprintf_s(robotPipeName, "\\\\.\\pipe\\CC-%X-robot-%X", processID, i);


        while (WaitNamedPipe(robotPipeName, INFINITE) == FALSE) {
            Sleep(100); // pause for 100 ms
        }

        t[i].R = CreateFile(robotPipeName, GENERIC_READ | GENERIC_WRITE, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
        if ((thread[i] = CreateThread(NULL, 0, ThreadStarter, t + i, 0, NULL)) == NULL) {
            printf("Error %d creating thread\n", GetLastError());
            exit(-1);
        }
        SetThreadPriority(thread[i], THREAD_PRIORITY_IDLE);
    }

    for (int i = 0; i < NUMROBOTS; i++) {
        WaitForSingleObject(thread[i], INFINITE);
        CloseHandle(thread[i]);
    }
    *(x->finished) = true;
	return 0;
}



DWORD __stdcall StatFun(LPVOID p) {
    StatParam* s = (StatParam*)p;
    int prevCave = 0;
    int prevUnique = 0;
    int total;
    CPU cpu;
    while (!(*(s->finished))) {
        /*if (quit == true) {
            break;
        }*/
        Sleep(2000);
        printf("[%.1fM] U %.1fM D %.1fM, %.2fM/sec, %d*, %.0f%% uniq [%.0f CPU, %u MB]\n", *(s->uPop) / 1000000.0, *(s->uSize) / 1000000.0, *(s->dSize) / 1000000.0, ((*(s->uPop) - prevCave) / 2.0) / 1000000.0, *(s->threadActive), ((float)(*(s->uniqueCount)) / *(s->totalCount)) * 100, cpu.GetCpuUtilization(NULL), cpu.GetProcessRAMUsage(true));
        prevCave = *(s->uPop);
        //*(s->totalCount) = 0;
        //*(s->uniqueCount) = 0;
        //prevUnique = *(s->uniqueCount);

        //*(s->uniqueCount) = 0;
        
        //*(s->totalCount) = 1;
    }
    return 0;

}