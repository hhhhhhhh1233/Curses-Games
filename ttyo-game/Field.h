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
#include "Vec2.h"
#include "Grid.h"
#include "Puyo.h"

using std::map;
using std::vector;

class Field
{
public:
	Vec2 GridPosition;
	Grid grid;
	Puyo ActivePuyo;
	Puyo NextPuyo;
	vector<Puyo> SetPuyos;
	int Score;
	bool GameRunning;
	bool CanPause;

	Field(Vec2 gridPos, int width, int height, int cellWidth, int cellHeight, bool Player = true);
	void PuyoFall();
	void PuyoMoveLeft();
	void PuyoMoveRight();
	void PuyoRotateClockwise();
	void PuyoRotateCounterClockwise();
	void GameOver();
	void PuyoDrop();
	void Draw();
	void DrawBare();
	void DrawActiveHint();
	void DrawXs();
};
