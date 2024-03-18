#pragma once

#include <algorithm>
#include <chrono>
#include <cstdlib>
#include <curses.h>
#include <iostream>
#include <map>
#include <thread>
#include <vector>
#include <unistd.h>
#include "Color.h"

using std::map;
using std::vector;

class Grid
{
public:
	vector<char> s;
	int width;
	int height;

	Grid(int Width, int Height);
	char GetChar(int x, int y);
	void SetChar(int x, int y, char c);
	bool IsEmpty(int x, int y);
	void Draw(int X, int Y);
	int ClearPuyos();
	void DropAllPuyos();
};
