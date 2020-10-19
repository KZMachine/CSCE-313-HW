// Karim Zaher
// CSCE 313 - 201
// Fall 2020

#pragma once

#define CONNECT             0
#define DISCONNECT          1
#define MOVE                2

#define FAILURE             0
#define SUCCESS             1

#define BUF_SIZE            128
#define MAX_BATCH			10000

#define BFS 0


// Response Robot Messages
#define STATUS_OK					0 // no error, command successful
#define STATUS_ALREADY_CONNECTED	1 // repeated attempt to connect
#define STATUS_INVALID_COMMAND		2 // command too short or invalid
#define STATUS_INVALID_ROOM			3 // room ID doesn't exist
#define STATUS_INVALID_BATCH_SIZE	4 // batch size too large or equals 0
#define STATUS_MUST_CONNECT			5 // first command must be CONNECT 

#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <Windows.h>
#include <time.h>
#include <chrono>
#include <iostream>
#include <cmath>
#include <list>


#include "Search.cpp"
#include <iterator>
#include <set>
#include <utility>
#include <queue>


#include "CCRobot.cpp"
#include "Thread.cpp"
#include "Stats.cpp"
//#include "GenStruct.cpp"