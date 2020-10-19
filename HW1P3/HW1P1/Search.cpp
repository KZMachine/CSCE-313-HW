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

/*class Discovered {
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
};*/




/*class Ubreadth{
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
		get<0>(x) = intensity + 20.0 / ((double)distance + 1);
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

};*/


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
	virtual void push(UINT64 roomID, float intensity, int distance) = 0;
	virtual UnexploredRoom pop(void) = 0; // pops the next room
	virtual int size(void) = 0; // checks the size of U 
	virtual bool isEmpty(void) = 0;
};

/*bool operator< (const UnexploredRoom& p1, const UnexploredRoom& p2)
{
	return p1.intensity > p2.intensity;
}
//Overload the > operator.
bool operator> (const UnexploredRoom& p1, const UnexploredRoom& p2)
{
	return p1.intensity < p2.intensity;
}*/


class Ubreadth : public Ubase {
public:
	queue <UnexploredRoom*> U;
	queue <int> a;

	Ubreadth() {

	}
	void Ubase::push(UINT64 roomID, float intensity, int distance) {

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

class Udepth : public Ubase {
public:
	stack <UnexploredRoom*> U;

	Udepth() {

	}
	void Ubase::push(UINT64 roomID, float intensity, int distance) {
		UnexploredRoom* x = new UnexploredRoom;
		x->ID = roomID;
		x->distance = distance;
		U.push(x);
	}

	UnexploredRoom Ubase::pop() {
		UnexploredRoom* x = U.top();
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

class Ubest : public Ubase {
public:
	priority_queue< tuple <float, UINT64, int>> U;
	Ubest() {

	}
	void Ubase::push(UINT64 roomID, float intensity, int distance) {
		tuple <float, UINT64, int> x;
		get<0>(x) = intensity;
		get<1>(x) = roomID;
		get<2>(x) = distance;
		U.push(x);
	}

	UnexploredRoom Ubase::pop() {
		tuple <float, UINT64, int> y;
		y = U.top();
		U.pop();
		UnexploredRoom* x = new UnexploredRoom;
		x->intensity = get<0>(y);
		x->ID = get<1>(y);
		x->distance = get<2>(y);
		return *x;
	}

	int Ubase::size() {
		return U.size();
	}

	bool Ubase::isEmpty() {
		return U.empty();
	}

};

class Uastar : public Ubase {
public:
	priority_queue< tuple <float, UINT64, int>> U;
	Uastar() {

	}
	void push(UINT64 roomID, float intensity, int distance) {
		tuple <float, UINT64, int> x;
		get<0>(x) = intensity + 20.0 / ((double)distance + 1);
		get<1>(x) = roomID;
		get<2>(x) = distance;
		U.push(x);
	}

	UnexploredRoom Ubase::pop() {
		tuple <float, UINT64, int> y;
		y = U.top();
		U.pop();
		UnexploredRoom* x = new UnexploredRoom;
		x->intensity = get<0>(y);
		x->ID = get<1>(y);
		x->distance = get<2>(y);
		return *x;
	}

	int Ubase::size() {
		return U.size();
	}

	bool Ubase::isEmpty() {
		return U.empty();
	}

};