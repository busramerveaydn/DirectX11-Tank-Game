#pragma once
//#include "stdafx.h"
#include<iostream>
#include<queue>
#define V 34
using namespace std;

struct Dijkstra{
private:

	int adjMatrix[V][V];

	int distance[V];

	bool mark[V]; //keep track of visited node

	int predecessor[V];

public:

	queue<int> path;

	int destination;

	int source;

	void read(int*);

	void initialize();

	int getClosestUnmarkedNode();

	void calculateDistance();

	void printPath(int);

	void sourceDesDef(int src, int des);

}G;
