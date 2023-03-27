#pragma once

// A C++ Program to implement A* Search Algorithm

#include <iostream>
#include <utility>
#include <stack>
#include <vector>
#include <set>

#include <glm/vec2.hpp>

using namespace std;

#define ROW 64
#define COL 64

// Creating a shortcut for int, int pair type
typedef pair<int, int> Pair;

// Creating a shortcut for pair<int, pair<int, int>> type
typedef pair<double, pair<int, int> > pPair;

// A structure to hold the necessary parameters
struct cell {
	// Row and Column index of its parent
	// Note that 0 <= i <= ROW-1 & 0 <= j <= COL-1
	int parent_i, parent_j;
	// f = g + h
	double f, g, h;
};

class A_STAR {
private:

	// A Utility Function to check whether given cell (row, col)
	// is a valid cell or not.
	bool isValid(int row, int col);

	// A Utility Function to check whether the given cell is
	// blocked or not
	bool isUnBlocked(int grid[][COL], int row, int col);

	// A Utility Function to check whether destination cell has
	// been reached or not
	bool isDestination(int row, int col, Pair dest);

	// A Utility Function to calculate the 'h' heuristics.
	double calculateHValue(int row, int col, Pair dest);

	// A Utility Function to trace the path from the source
	// to destination
	void tracePath(cell cellDetails[][COL], Pair dest);

public:
	// A Function to find the shortest path between
	// a given source cell to a destination cell according
	// to A* Search Algorithm
	void aStarSearch(int grid[][COL], Pair src, Pair dest);

	vector<glm::vec2>& GetPath();

private:
	vector<glm::vec2> path;
};