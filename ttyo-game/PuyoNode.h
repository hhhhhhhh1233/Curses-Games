#pragma once

#include <algorithm>
#include <map>
#include <thread>
#include <vector>
#include "Vec2.h"
#include "Color.h"

using std::map;
using std::vector;

class PuyoNode
{
public:
	Vec2 Position;
	Color Type;

	PuyoNode(Vec2 pos, Color type);
	PuyoNode(Vec2 pos);
};
