#pragma once

#include <vector>
#include <unistd.h>
#include "PuyoNode.h"

class Puyo
{
public:
	PuyoNode Pivot;
	PuyoNode Tagalong;

	char Body;
	Vec2 rotations[4] = { Vec2(0, -1), Vec2(1, 0), Vec2(0, 1), Vec2(-1, 0) };
	int currentRotation;

	Puyo(int X, int Y, Color t, int body);
	Puyo(int X, int Y, int body);
	void CW();
	void CCW();
	void MoveLeft();
	void MoveRight();
	void MoveDown();
	void UpdateTagalong();
	void Draw(int X, int Y);
};
