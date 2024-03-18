#pragma once

struct Vec2
{
	int x;
	int y;

	Vec2 (int X = 0, int Y = 0)
	{
		x = X;
		y = Y;
	}

	bool operator==(Vec2 rhs) const
	{
		return x == rhs.x && y == rhs.y;
	}
};
