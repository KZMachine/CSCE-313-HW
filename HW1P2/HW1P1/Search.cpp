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

class Discovered {
public:
	set <UINT64> D;
	Discovered() {

	}
	void CheckAdd(UINT64 roomID) {
		D.insert(roomID);
	}
	int getSize() {
		return D.size();
	}

	bool isDiscovered(UINT64 room) {
		if (D.count(room) == 1) {
			//printf("It's In here\n");
			return true;
		}
		return false;
	}
};

class UnexploredRoom {
	UINT64		ID;
	int			distance;
};

class Ubreadth{
public:
	queue <pair<UINT64, int>> U;
	queue <int> a;
	
	Ubreadth() {
		
	}
	void push(UINT64 roomID, float intensity, int distance) {
		pair<UINT64, int> x;
		x.first = roomID;
		x.second = distance;
		U.push(x);
	}
	
	pair<UINT64, int> pop() {
		pair<UINT64, int> x = U.front();
		U.pop();
		return x;
	}

	int size() {
		return U.size();
	}

	bool isEmpty() {
		return U.empty();
	}
	
};

class Udepth {
public:
	stack <pair<UINT64, int>> U;

	Udepth() {

	}
	void push(UINT64 roomID, float intensity, int distance) {
		pair<UINT64, int> x;
		x.first = roomID;
		x.second = distance;
		U.push(x);
	}

	pair<UINT64, int> pop() {
		pair<UINT64, int> x = U.top();
		U.pop();
		return x;
	}

	int size() {
		return U.size();
	}

	bool isEmpty() {
		return U.empty();
	}

};

class Ubest {
public:
	priority_queue< tuple <float, UINT64, int>> U;
	Ubest() {

	}
	void push(UINT64 roomID, float intensity, int distance) {
		tuple <float, UINT64, int> x;
		get<0>(x) = intensity;
		get<1>(x) = roomID;
		get<2>(x) = distance;
		U.push(x);
	}

	tuple <float, UINT64, int> pop() {
		tuple <float, UINT64, int> x;
		x = U.top();
		U.pop();
		return x;
	}

	int size() {
		return U.size();
	}

	bool isEmpty() {
		return U.empty();
	}

};

class Uastar {
public:
	priority_queue< tuple <float, UINT64, int>> U;
	Uastar() {

	}
	void push(UINT64 roomID, float intensity, int distance) {
		tuple <float, UINT64, int> x;
		get<0>(x) = intensity + 20 / (distance + 1);
		get<1>(x) = roomID;
		get<2>(x) = distance;
		U.push(x);
	}

	tuple <float, UINT64, int> pop() {
		tuple <float, UINT64, int> x;
		x = U.top();
		U.pop();
		return x;
	}

	int size() {
		return U.size();
	}

	bool isEmpty() {
		return U.empty();
	}

};