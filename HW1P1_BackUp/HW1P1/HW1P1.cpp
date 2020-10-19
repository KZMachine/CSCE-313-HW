// Karim Zaher
// CSCE 313 - 201
// Fall 2020

#include "pch.h"

#define CONNECT             0
#define DISCONNECT          1
#define MOVE                2

#define FAILURE             0
#define SUCCESS             1

#pragma pack(push, 1)
class CommandCC {
public:
    UCHAR      command : 2; // lower 2 bits
    UCHAR      planet : 6;  // remaining 6 bits
    DWORD      cave;        // which cave
    USHORT     robots;      // how many robots

    CommandCC(UCHAR comm, UCHAR p, DWORD c, USHORT r) {
        command = comm;
        planet = p;
        cave = c;
        robots = r;
    }

    void setCommand(int c) {
        if (c == 0 || c == 1) {
            command = c;
        }
        else {
            printf("Invalid CC Command...");
            exit(-1);
        }

    }
};
#pragma pack(pop)

class ResponseCC {
public:
    DWORD   status;
    char    msg[64];
};

#pragma pack(push, 1)
class CommandRobot {
public:
    DWORD       command;
    UINT64      room;
    CommandRobot(DWORD com, UINT64 r) {
        command = com;
        room = r;
    }
    void setCommand(int c) {
        if (c < 2 && c >= 0) {
            command = c;
        }
        else {
            printf("Invalid Robot Command...");
            exit(-1);
        }
    }
};
#pragma pack(pop)

class ResponseRobot {
public:
    DWORD       status;
    char        msg[64];
};

class NodeTuple64 {
public:
    UINT64     node;
    float       intensity;
};

int main(int argc, char *argv[])
{

    if (argc != 4) {
        printf("Usage: file_name planet_number cave_number search_method");
        exit(-1);
    }
    int planetNumber = std::stoi(argv[1]);
    int caveNumber = std::stoi(argv[2]);
    int NUMROBOTS = 1;

    if (planetNumber > 7 || planetNumber < 1) {
        printf("Invalid planet number(Use Planets 1-7)");
        exit(-1);
    }
    printf("Starting CC.exe...\n");

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

    char pipename[17];
    sprintf_s(pipename, "\\\\.\\pipe\\CC-%X", pi.dwProcessId);

     //wait for the CC to initialize the pipe from its end
    while (WaitNamedPipe(pipename, INFINITE) == FALSE) {
        Sleep(100); // pause for 100 ms
    }
     
    HANDLE CC = CreateFile(pipename, GENERIC_READ | GENERIC_WRITE,
        0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
    
    printf("Connecting to CC with planet %d , cave %d, robots %d...\n", planetNumber, caveNumber, NUMROBOTS);

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
    
    printf("CC says: status = %d, msg =  \'%s\'\n", ccResponse.status, ccResponse.msg);
    
    char robotPipeName[1024];
    printf("Connecting to robot %d...\n", 0);
    sprintf_s(robotPipeName, "\\\\.\\pipe\\CC-%X-robot-%X", pi.dwProcessId, 0);

    while (WaitNamedPipe(robotPipeName, INFINITE) == FALSE) {
        Sleep(100); // pause for 100 ms
    }

    HANDLE R = CreateFile(robotPipeName, GENERIC_READ | GENERIC_WRITE,
        0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);


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

    printf("Robot says: status = %d, msg =  \'%s\'\n", robotResponse.status, robotResponse.msg);

    NodeTuple64 caveData;
    
    BOOL receivedData = ReadFile(R, &caveData, sizeof(NodeTuple64), &numBytesRead, NULL);
    if (receivedData == FAILURE) {
        printf("Error %d receiving cave data from Robot\n", GetLastError());
        exit(-1);
    }
    
    printf("Current position: room %llX, light intensity %.3g\n", caveData.node, caveData.intensity);
    printf("-------------------------\n");
    
    printf("Disconnecting robot 0...\n");
    robotCommand.setCommand(DISCONNECT);
    BOOL closeRobot = WriteFile(R, &robotCommand, sizeof(CommandRobot), &numBytesWritten, NULL);
    if (closeRobot == FAILURE) {
        printf("Error %d sending message to close Robot\n", GetLastError());
        exit(-1);
    }

    CloseHandle(R);

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
