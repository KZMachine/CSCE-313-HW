#include "pch.h"

class CreatorParams {
public:
    int NUMROBOTS;
    int planetNumber;
    string searchType;
    DWORD processID;
    int* uPop;
    int* uSize;
    int* dSize;
    int* rooms;
    int* threadActive;
    int* threadRunning;
    bool* finished;
};

class StatParam {
public:
    int* uPop;
    int* uSize;
    int* dSize;
    int* rooms;
    int* threadActive;
    int* threadRunning;
    bool* finished;
};

DWORD __stdcall ThreadCreator(LPVOID p) {
    CreatorParams* x = (CreatorParams*)p;
    int NUMROBOTS = x->NUMROBOTS;
    int planetNumber = x->planetNumber;
    string searchType = x->searchType;
    DWORD processID = x->processID;

    char robotPipeName[1024];

    HANDLE* thread = new HANDLE[NUMROBOTS];
    ThreadParams* t = new ThreadParams[NUMROBOTS];
    SearchThrough me;
    me.uPop = (x->uPop);
    me.uSize = (x->uSize);
    me.dSize = (x->dSize);
    me.rooms = (x->rooms);
    me.threadActive = (x->threadActive);
    me.threadRunning =(x->threadRunning);
    if (searchType == "BFS") {
        me.searchType = BFS;
    } else if (searchType == "DFS") {
        me.searchType = DFS;
    } else if (searchType == "Astar") {
        me.searchType = ASTAR;
    }
    else {
        me.searchType = bFS;
    }

    me.NUMROBOTS = NUMROBOTS;
    me.planetNumber = planetNumber;
    me.m = CreateMutex(NULL, FALSE, NULL); // not locked initially
    me.sema = CreateSemaphore(NULL, 0, pow(10, planetNumber), NULL);
    me.eventQuit = CreateEvent(NULL, true, false, NULL); // signaled to quit
    Discovered d;
    me.d = d;
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
    //printf("result = %d\n", me.activeThreads);
    *(x->finished) = true;
	return 0;
}



DWORD __stdcall StatFun(LPVOID p) {
    StatParam* s = (StatParam*)p;
    int prevCave = 0;
    int time = 2;
    while (!(*(s->finished))) {
        Sleep(2000);
        printf("[%ds] E %.2fK, U %.2fK, D %.2fK, %d/sec, active %d, run %d\n",time, *(s->uPop) / 1000.0, *(s->uSize) / 1000.0, *(s->dSize) / 1000.0, ((*(s->uPop)-prevCave) / 2), *(s->threadActive), *(s->threadRunning));
        prevCave = *(s->uPop);
        time += 2;
    }

    printf("[Final] E %.2fK, U %.2fK, D %.2fK, %d/sec, active %d, run %d\n", *(s->uPop) / 1000.0, *(s->uSize) / 1000.0, *(s->dSize) / 1000.0, ((*(s->uPop) - prevCave) / 2), *(s->threadActive), *(s->threadRunning));
    return 0;

}