// Karim Zaher
// CSCE 313 - 201
// Fall 2020

#include "pch.h"

class Hash {
public:
	LONG* array;
	Hash() {
		array = new LONG[1LU << 27]();
		memset(array, 0, 1LU << 27);
	}
	~Hash() {
		delete[] array;
		array = nullptr;
	}
	LONG* getTable() {
		return array;
	}
};

//myQueue
class MyQueue {
	HANDLE heap; // custom heap
	DWORD* buf; // buffer pointer
	UINT64 head, tail; // usual head/tail offsets
	UINT64 spaceAllocated; // buffer size
	UINT64 sizeQ; // number of items in Q
public:
	MyQueue() {
		heap = HeapCreate(HEAP_NO_SERIALIZE, 0, 0);
		buf = (DWORD*)HeapAlloc(heap, HEAP_NO_SERIALIZE, sizeof(DWORD));
		spaceAllocated = sizeof(DWORD);
		head = 0;
		tail = 0;
		sizeQ = 0;
	}
	DWORD getSize() {
		return sizeQ;
	}
	int Modify(int size) {
		sizeQ += size;
		if (sizeQ >= spaceAllocated) {
			spaceAllocated += size;
			buf = (DWORD*)HeapReAlloc(heap, HEAP_NO_SERIALIZE, buf, sizeof(DWORD) * spaceAllocated);
		}
		int oldTail = tail;
		tail = sizeQ - 1;
		head = 0;
		return oldTail;
	}
	DWORD* getBuf() {
		return &buf[head];
	}
	~MyQueue() {
		//HeapFree(heap, HEAP_NO_SERIALIZE, buf); ADD error check
	}
	void Push(DWORD item) {		// single push
		if (spaceAllocated <= sizeQ) {
			//printf("TIME TO CHANGE THINGS UP\n");
			spaceAllocated = sizeQ * 2;
			//printf(", Allocated space, %d", spaceAllocated);
			buf = (DWORD*)HeapReAlloc(heap, HEAP_NO_SERIALIZE, buf, sizeof(DWORD)*spaceAllocated);
			//printf(" and done");
		}
		//printf(", Entered, ");
		tail++;
		//printf("tail: %d, ", tail);
		if (sizeQ == 0) {
			tail = 0;
		}
		//printf("Ready, ");
		buf[tail] = item;
		//printf("AND DONE, ");
		sizeQ++;
	}
	DWORD Pop(DWORD* array, int batchSize) {// batch pop 
		int size = min(sizeQ, batchSize);
		memcpy(array, &buf[head], size*sizeof(DWORD));
		sizeQ -= size;
		head += size;
		if (sizeQ == 0) {
			head = 0;
			tail = 0;
		}
		return size;
	}
	void printBuf() {
		printf("---------- Printing Buffer ----------\n");
		printf("\n");
		for (int i = 0; i < sizeQ; i++) {
			printf("%llX\n", buf[i]);
		}
		printf("\n");
		printf("---------- End of Buffer ----------\n");
	}
};

//MODIFIED VERSION
/*
DWORD Pop(DWORD* array, int batchSize) {// batch pop 
	int size = min(sizeQ, batchSize);
	memcpy(array, &buf[head], (head + size)*sizeof(DWORD));
	//sizeQ -= size;
	head += size;
	sizeQ = tail - head + 1;
	if (sizeQ <= 0) {
		head = 0;
		tail = 0;
	}
	return size;
}
*/

//ORIGINAL VERSION
/*DWORD Pop(DWORD* array, int batchSize) {// batch pop 
		int size = min(sizeQ, batchSize);
		printf("**************************************MIN SIZE: %d", size);
		memcpy(array, &buf[head], size);
		sizeQ -= size;
		head += size;
		if (sizeQ == 0) {
			head = 0;
			tail = 0;
		}
		return size;
	}*/