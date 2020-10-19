#include "pch.h"
/*
char* request = new char[sizeof(CommandRobotHeader) + MAX_BATCH * sizeof(DWORD)];
CommandRobotHeader* crh = (CommandRobotHeader*)request;
crh->command = MOVE;
DWORD* rooms = (DWORD*)(cr + 1);
while (true) {
	if (quit) // flag set?
		break;
	int batch = 0;
	CS.lock(); // PC 3.4
	if (Q[cur].sizeQ > 0) {
		batch = Q[cur].pop(rooms, MAX_BATCH);
		activeThreads++;
		// other stats go here
	}
	CS.unlock();
	if (batch == 0) { // got nothing from Q?
		Sleep(100);
		continue;
	}
	pipe.SendMsg(...); // send request[]
	pipe.RecvMsg(...); // read response

	while (rooms left in response) {
		DWORD ID = ... // get next room
			DWORD offset = ...
			DWORD bit = ...
			if (InterlockedBitTestSet(hashTable + offset, bit) == 0)
				localQ.push(ID);
	}
	CS.lock();
	// batch-pop all elements from
	// localQ into Q[cur^1]
	activeThreads--;
	if (this BFS level is over)
		if (next level empty)
			quit = true;
		else
			cur ^= 1;
	CS.unlock();
}*/



