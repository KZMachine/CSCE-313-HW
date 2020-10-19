
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
class CommandRobotHeader {
public:
    DWORD command;
    CommandRobotHeader(DWORD com) {
        command = com;
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
};
#pragma pack(pop)


class ResponseRobotHeader {
public:
    DWORD status : 3; // up to 8 unique result codes
    DWORD len : 29; // max len = 0.5B neighbors
};

#pragma pack(push, 1)
class NodeTuple64 {
public:
    DWORD     node;    // 8 bytes [64 bits]
};
#pragma pack(pop)