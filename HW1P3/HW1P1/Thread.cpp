#include "pch.h"
#include <iostream>
#include <fstream>


class SearchThrough {
public:
    int* uPop = new int(0);
    int* uSize = new int(0);
    int* dSize = new int(0);
    int* rooms = new int(0);
    int* threadActive = new int(0);
    int* threadRunning = new int(0);

    int steps = 0;
    int NUMROBOTS;
    int searchType;
    int planetNumber;
    HANDLE m;
    HANDLE sema;
    HANDLE eventQuit;
    Ubase *U;
    Discovered d;
    HANDLE lpHandles[2];
    
    //ofstream fp;
    //long long int xTotal;
    

void search(CommandRobot robotCommand, ResponseRobot robotResponse, HANDLE R, char* buf, int buffSize, NodeTuple64* nt, int threadID) {
    lpHandles[0] = eventQuit;
    lpHandles[1] = sema;
    WaitForSingleObject(m, INFINITE);
    if (steps == 0) {
        if (!d.isDiscovered(nt->node)) {
            switch (searchType) {
            case BFS:
                U = new Ubreadth;
                printf("Starting search using BFS...\n");
                break;
            case DFS:
                U = new Udepth;
                printf("Starting search using DFS...\n");
                break;
            case ASTAR:
                U = new Uastar;
                printf("Starting search using A*...\n");
                break;
            case bFS:
                U = new Ubest;
                printf("Starting search using bFS...\n");
                break;
            }
            U->push(nt->node, nt->intensity, 0);
            d.CheckAdd(nt->node);
            //fp.open("question4_7.csv");
        }
    }
    ReleaseMutex(m);
    WaitForSingleObject(m, INFINITE);
    *threadRunning += 1;
    ReleaseMutex(m);
    while (true) {
        int distance = 0;
        int totalBytesRead = 900;
        if (WaitForMultipleObjects(2, lpHandles, FALSE, 0) == WAIT_OBJECT_0) {
            WaitForSingleObject(m, INFINITE);
            *threadRunning-=1;
            ReleaseMutex(m);
            break;
        }
        
        WaitForSingleObject(m, INFINITE);
        if (U->isEmpty()) {
            ReleaseMutex(m);
            continue;
        }
        *threadActive += 1;
        UnexploredRoom popped = U->pop();
        *uSize = U->size();
        *(uPop) += 1;
        ReleaseMutex(m);
        
        //printf("PRINT POP:%d\n", uPop);
        UINT64 nextRoom = popped.ID;

        distance = popped.distance;
        robotCommand.setRoom(nextRoom);
        DWORD numBytesWritten = 0;
        DWORD numBytesRead = 0;


        BOOL sendRobot = WriteFile(R, &robotCommand, sizeof(CommandRobot), &numBytesWritten, NULL);
        if (sendRobot == FAILURE) {
            printf("Error %d sending message to Robot\n", GetLastError());
            exit(-1);
        }

        BOOL receiveRobot = ReadFile(R, &robotResponse, sizeof(ResponseRobot), &numBytesRead, NULL);
        if (receiveRobot == FAILURE) {
            printf("Error %d receiving message from Robot\n", GetLastError());
            exit(-1);
        }
        BOOL peekData = PeekNamedPipe(R, NULL, NULL, NULL, &numBytesRead, NULL);
        if (peekData == FAILURE) {
            printf("Error %d peeking message from Robot\n", GetLastError());
            exit(-1);
        }
        //printf("%d Exporing, \n", threadID);


        if (numBytesRead == 0) {
            WaitForSingleObject(m, INFINITE);
            if (!U->isEmpty()) {
                SetEvent(eventQuit);
                printf("Thread %d:found exit %llX, %d steps, distance %d\n", threadID, nextRoom, (steps + 1), distance);
                *threadActive -= 1;
                ReleaseMutex(m);
                break;
            }
            else {
                SetEvent(eventQuit);
                printf("No exit found.");
                *threadActive -= 1;
                ReleaseMutex(m);
                break;
            }
            *threadActive -= 1;
            ReleaseMutex(m);
            break;
        }
        //printf("Robot says: status = %d, msg =  \'%s\'\n", robotResponse.status, robotResponse.msg);
        BOOL receivedData = ReadFile(R, buf, buffSize, &numBytesRead, NULL);
        if (receivedData == FAILURE) {
            printf("Error %d receiving cave data from Robot\n", GetLastError());
            exit(-1);
        }

        totalBytesRead = numBytesRead;
        DWORD remainderBytes = 0;
        int newBuff = 0;
        if (buffSize == numBytesRead) {
            BOOL peek = PeekNamedPipe(R, NULL, NULL, NULL, &remainderBytes, NULL);
            if (peek == FAILURE) {
                printf("Error %d peeking message from Robot\n", GetLastError());
                exit(-1);
            }

            if (remainderBytes != 0) {
                totalBytesRead += remainderBytes;
                newBuff = buffSize + remainderBytes;
                char* temp = new char[newBuff];
                for (int i = 0; i < buffSize; i++) {
                    temp[i] = buf[i];
                }
                delete[] buf;
                buf = temp;
                nt = (NodeTuple64*)buf;

                char* additional = new char[remainderBytes];

                receiveRobot = ReadFile(R, additional, remainderBytes, &numBytesRead, NULL);
                if (receiveRobot == FAILURE) {
                    printf("Error %d receiving message from Robot\n", GetLastError());
                    exit(-1);
                }

                for (int i = buffSize; i < newBuff; i++) {
                    buf[i] = additional[i - buffSize];
                }
                nt = (NodeTuple64*)buf;

                buffSize = newBuff;

            }
        }


        int t = 0;
        WaitForSingleObject(m, INFINITE);
        //fp << distance << ", " << endl;
        //xTotal += distance;

        int degree = totalBytesRead / sizeof(NodeTuple64);
        while (t < degree) {
            //printf("%llX\n", nt[t].node);
            if (!d.isDiscovered(nt[t].node)) {
                U->push(nt[t].node, nt[t].intensity, distance + 1);
                *uSize = U->size();
            }

            d.CheckAdd(nt[t].node);
            *dSize = d.getSize();
            t++;
        }
        *threadActive-=1;
        if (U->size() == 0 && *threadActive == 0) {
            SetEvent(eventQuit);
        }
        steps++;
        if (t > 0) {
            ReleaseSemaphore(sema, t, NULL);
        }
        ReleaseMutex(m);
    }
}

};

class ThreadParams {
public:
    SearchThrough* me = new SearchThrough;
    int threadID;
    string searchType;
    DWORD ProcessID;
    HANDLE R;
};

DWORD __stdcall ThreadStarter(LPVOID p) {
    ThreadParams* t = (ThreadParams*)p;
    HANDLE R = t->R;
    
    DWORD numBytesWritten;
    DWORD numBytesRead;


    UINT64 roomNumber = 0;
    CommandRobot robotCommand(CONNECT, roomNumber);
    ResponseRobot robotResponse;

    BOOL sendRobot = WriteFile(R, &robotCommand, sizeof(CommandRobot), &numBytesWritten, NULL);
    if (sendRobot == FAILURE) {
        printf("Error %d sending message to Robot\n", GetLastError());
        exit(-1);
    }

    BOOL receiveRobot = ReadFile(R, &robotResponse, sizeof(ResponseRobot), &numBytesRead, NULL);
    if (receiveRobot == FAILURE) {
        printf("Error %d receiving message from Robot\n", GetLastError());
        exit(-1);
    }
    
    //printf("Robot says: status = %d, msg =  \'%s\'\n", robotResponse.status, robotResponse.msg);

    
    int buffSize = BUF_SIZE;
    char* buf = new char[buffSize];
    BOOL receivedData = ReadFile(R, buf, buffSize, &numBytesRead, NULL);
    if (receivedData == FAILURE) {
        printf("Error %d receiving cave data from Robot\n", GetLastError());
        exit(-1);
    }
    NodeTuple64* nt = (NodeTuple64 *) buf;
    //printf("Current position: room %llX, light intensity %.3g\n", nt->node, nt->intensity);

    robotCommand.setCommand(MOVE);
    int distance = 0;
    t->me->search(robotCommand, robotResponse, R, buf, buffSize, nt, t->threadID);
    
    robotCommand.setCommand(DISCONNECT);
    BOOL closeRobot = WriteFile(R, &robotCommand, sizeof(CommandRobot), &numBytesWritten, NULL);
    if (closeRobot == FAILURE) {
        printf("Error %d sending message to close Robot\n", GetLastError());
        exit(-1);
    }
    return 0;

}
