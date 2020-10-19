#include "pch.h"
#include <iostream>
#include <fstream>


class SearchThrough {
public:
    UINT64* uPop = new UINT64(0);
    UINT64* uSize = new UINT64(0);
    UINT64* dSize = new UINT64(0);
    UINT64* numRooms = new UINT64(0);
    UINT64* uniqueCount = new UINT64(0);
    UINT64* totalCount = new UINT64(1);
    int* numberNodes = new int(0);
    int* distance = new int(0);
    int* threadActive = new int(0);
    int* threadRunning = new int(0);

    int steps = 0;
    int cur = 0;
    int NUMROBOTS;
    int planetNumber;
    CRITICAL_SECTION CS;
    MyQueue *Q = new MyQueue[2];
    bool quit = false;
    Hash *hashTable = new Hash;
    HANDLE lpHandles[2];

    void search(CommandRobotHeader robotCommand, HANDLE R, int threadID) {
        MyQueue localQ;
        InitializeCriticalSection(&CS);
        DWORD numBytesRead, numBytesWritten = 0;
        ResponseRobotHeader robotResponse;
        BOOL sendRobot = WriteFile(R, &robotCommand, sizeof(DWORD), &numBytesWritten, NULL);
        if (sendRobot == FAILURE) {
            printf("Error %d sending message to Robot\n", GetLastError());
            exit(-1);
        }
        BOOL receiveRobot = ReadFile(R, &robotResponse, sizeof(ResponseRobotHeader), &numBytesRead, NULL);
        if (receiveRobot == FAILURE) {
            printf("Error %d receiving message from Robot\n", GetLastError());
            exit(-1);
        }
        
        /*printf("Robot says: status = %d, length = %d\n", robotResponse.status, robotResponse.len);*/
        char* request = new char[(sizeof(CommandRobotHeader) + (MAX_BATCH) * sizeof(DWORD))+1];
        CommandRobotHeader* crh = (CommandRobotHeader*)request;
        crh->command = MOVE;
        DWORD* rooms = (DWORD*)(crh + 1);
        receiveRobot = ReadFile(R, rooms, sizeof(DWORD) * robotResponse.len, &numBytesRead, NULL);
        if (receiveRobot == FAILURE) {
            printf("Error %d receiving message from Robot\n", GetLastError());
            exit(-1);
        }
        DWORD ID = rooms[0]; // get next room
        DWORD offset = ID >> 5;
        DWORD bit = ID & 0x1F;
        if (InterlockedBitTestAndSet(hashTable->array + offset, bit) == 0) {
            Q[cur].Push(rooms[0]);
            *uniqueCount += 1;
            *dSize += 1;
            *totalCount += 1;
        }
        while (true) {
            if (quit) // flag set?
                break;
            int batch = 0;

            EnterCriticalSection(&CS); // PC 3.4
            if (Q[cur].getSize() > 0) {
                //for (int i = 0; i < 1500; i++) {
                //    if (i == 1429) {
                //        printf("----------");
                //    }
                //    printf("%llX\n", Q[cur].getBuf()[i]);
                //}
                batch = Q[cur].Pop(rooms, MAX_BATCH);
                *uSize = Q[cur].getSize() + Q[cur^1].getSize();
                //printf("Starting Batch Size: %d\n", batch);
                //printf("Remaining Elements: %d\n", Q[cur].getSize());
                *threadActive += 1;
                *uPop += batch;
                // other stats go here
            }
            LeaveCriticalSection(&CS);
            if (batch == 0) { // got nothing from Q?
                Sleep(100);
                continue;
            }
            // Send Message
            sendRobot = WriteFile(R, crh, sizeof(CommandRobotHeader) + batch * sizeof(DWORD), &numBytesWritten, NULL);
            if (sendRobot == FAILURE) {
                printf("Error %d sending message to Robot\n", GetLastError());
                exit(-1);
            }
            /*printf("Contents of rooms: ");
            for (int i = 0; i < batch; i++) {
                printf("%llX ", rooms[i]);
            }
            printf("\n");*/

            //printf("Preparing to enter loop... %d\n", batch);
            UINT64 loop = 0;
            DWORD numBytesRemaining = 0;
            DWORD* inputRooms = new DWORD[batch];
            for (int i = 0; i < batch; i++) {
                inputRooms[i] = rooms[i];
            }
            while (loop < batch) {
                // Receive Message
                //printf("Entered Loop... %d/%d\n", loop, batch);
                receiveRobot = ReadFile(R, &robotResponse, sizeof(ResponseRobotHeader), &numBytesRead, NULL);
                if (receiveRobot == FAILURE) {
                    printf("Error %d receiving message from Robot\n", GetLastError());
                    exit(-1);
                }
                //printf("READ ");
                if (robotResponse.len == 0) {
                    printf("Thread [%d]:found exit %llX, %d steps, distance, rooms explored \n", threadID, inputRooms[0], distance);
                    commaAdd(*numberNodes);
                }
                int tempResponseLen = robotResponse.len;
                bool largeLen = false;
                
                do {
                    
                    receiveRobot = ReadFile(R, rooms, sizeof(DWORD) * min(tempResponseLen, MAX_BATCH), &numBytesRead, NULL);
                    if (receiveRobot == FAILURE) {
                        printf("Error %d receiving message from Robot\n", GetLastError());
                        exit(-1);
                    }
                    tempResponseLen -= 10000;

                    int numRooms = min(robotResponse.len, MAX_BATCH);
                    int t = 0;

                    //printf("Room Neighbors:\n");
                    while (t < numRooms) {
                        /*if (largeLen && t%500 == 0) {
                            t = t;
                        }*/
                        DWORD ID = rooms[t]; // get next room
                        DWORD offset = ID >> 5;
                        DWORD bit = ID & 0x1F;
                        *totalCount += 1;
                        if (InterlockedBitTestAndSet(hashTable->array + offset, bit) == 0) {
                            *numberNodes += 1;
                            //printf("%llX ", rooms[t]);
                            //printf("IN");
                            localQ.Push(ID);
                            //printf(" AND OUT\n");
                            *uniqueCount += 1;
                            *dSize += 1;
                        }
                        t++;
                    }
                    largeLen = false;
                } while (tempResponseLen > 0);
                //printf("\n");
                loop++;
            }
            //printf("NO LONGER IN THERE!!!\n");
            EnterCriticalSection(&CS);
            // batch-pop all elements from
            // localQ into Q[cur^1]
            
            do{
                DWORD sizePop = localQ.Pop(rooms, 10000);
                int currentLocation = Q[cur ^ 1].Modify(sizePop);
                if (currentLocation != 0) {
                    currentLocation++;
                }
                memcpy(&Q[cur ^ 1].getBuf()[currentLocation], rooms, (sizePop) * sizeof(DWORD));
            } while (localQ.getSize() > 0);
            //printf("Content of Q[cur ^ 1] Buffer:\n");
            //Q[cur ^ 1].printBuf();
            *uSize = Q[cur].getSize() + Q[cur ^ 1].getSize();
            *threadActive-=1;
            if (Q[cur].getSize() == 0) {
                if (Q[cur^1].getSize() == 0) {
                    quit = true;
                }
                else {
                    printf("--------- Switching to level %d with ", ++(*distance));
                    commaAdd(*numberNodes);
                    printf(" nodes\n");
                    cur ^= 1;
                    *numberNodes = 0;
                }
            }
            LeaveCriticalSection(&CS);
        }
    }
};

class ThreadParams {
public:
    SearchThrough* me = new SearchThrough;
    int threadID;
    DWORD ProcessID;
    HANDLE R;
};

DWORD __stdcall ThreadStarter(LPVOID p) {
    ThreadParams* t = (ThreadParams*)p;
    HANDLE R = t->R;

    DWORD numBytesWritten = 0;
    DWORD numBytesRead = 0;


    UINT64 roomNumber = 0;
    CommandRobotHeader robotCommand(CONNECT);
    ResponseRobotHeader robotResponse;

    t->me->search(robotCommand, R, t->threadID);

    robotCommand.setCommand(DISCONNECT);
    BOOL closeRobot = WriteFile(R, &robotCommand.command, sizeof(DWORD), &numBytesWritten, NULL);
    if (closeRobot == FAILURE) {
        printf("Error %d sending message to close Robot\n", GetLastError());
        exit(-1);
    }
    return 0;

};