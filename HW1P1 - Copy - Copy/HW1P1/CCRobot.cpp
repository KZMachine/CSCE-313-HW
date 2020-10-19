
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
    void setCommand(DWORD c) {
        if (c <= 2 && c >= 0) {
            command = c;
        }
        else {
            printf("Invalid Robot Command...");
            exit(-1);
        }
    }
    void setRoom(UINT64 r) {
        room = r;
    }
};
#pragma pack(pop)

class ResponseRobot {
public:
    DWORD       status;
    char        msg[64];
};

#pragma pack(push, 1)
class NodeTuple64 {
public:
    UINT64     node;    // 8 bytes [64 bits]
    float       intensity;  // 4 bytes [32 bits]
};
#pragma pack(pop)