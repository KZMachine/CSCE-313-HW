// Karim Zaher
// CSCE 313 - 201
// Fall 2020

#include "pch.h"
#include <set>
#include <iterator>
#include <queue>
#include <stack>
#include <tuple>

using namespace std;

//myQueue
/*class MyQueue {
	HANDLE heap; // custom heap
	DWORD* buf; // buffer pointer
	DWORD head, tail; // usual head/tail offsets
	DWORD spaceAllocated; // buffer size
	DWORD sizeQ; // number of items in Q
public:
	MyQueue() {
		heap = HeapCreate(HEAP_NO_SERIALIZE, 1, 0);
		buf = (DWORD*)HeapAlloc(heap, HEAP_NO_SERIALIZE, 1);
		spaceAllocated = 1;
		head = 0;
		tail = 0;
		sizeQ = 0;
	}
	~MyQueue() {
		//HeapFree(heap, HEAP_NO_SERIALIZE, buf); ADD error check
	}
	void Push(DWORD item) {		// single push
		if (spaceAllocated == sizeQ) {
			spaceAllocated *= 2;
			HeapReAlloc(heap, HEAP_NO_SERIALIZE, buf, sizeQ);
		}
		buf[sizeQ] = item;
		tail = sizeQ;
		if (sizeQ == 0) {
			head = 0;
		}
		sizeQ++;
	}
	DWORD Pop(DWORD* array, int batchSize) {// batch pop 
		int size = min(sizeQ, batchSize);
		memcpy(array, buf, size);
		sizeQ -= size;
		return size;
	}
};*/

/*class Discov {
	LONG* array = new LONG[1LU<<27]();
	Discov() {
		
	}
	void CheckAdd() {

	}
};*/

class Discovered {
public:
	set <DWORD> D;
	Discovered() {

	}
	void CheckAdd(DWORD roomID) {
		D.insert(roomID);
	}
	int getSize() {
		return D.size();
	}

	bool isDiscovered(DWORD room) {
		if (D.count(room)) {
			//printf("It's In here\n");
			return true;
		}
		return false;
	}
};

class UnexploredRoom {
public:
	float		intensity = 0;
	UINT64		ID = 0;
	int			distance = 0;
	friend bool operator < (const UnexploredRoom&, const UnexploredRoom&);
	friend bool operator > (const UnexploredRoom&, const UnexploredRoom&);
};

class Ubase {
public:
	virtual void push(UINT64 roomID, int distance) = 0;
	virtual UnexploredRoom pop(void) = 0; // pops the next room
	virtual int size(void) = 0; // checks the size of U 
	virtual bool isEmpty(void) = 0;
};

class Ubreadth : public Ubase {
public:
	queue <UnexploredRoom*> U;
	queue <int> a;

	Ubreadth() {

	}
	void Ubase::push(UINT64 roomID, int distance) {

		UnexploredRoom* a = new UnexploredRoom;
		a->ID = roomID;
		a->distance = distance;
		U.push(a);

		//x.first = roomID;
		//x.second = distance;
		//U.push(x);
	}

	UnexploredRoom Ubase::pop() {
		UnexploredRoom* x = U.front();
		U.pop();
		return *x;
	}

	int Ubase::size() {
		return U.size();
	}

	bool Ubase::isEmpty() {
		return U.empty();
	}

};