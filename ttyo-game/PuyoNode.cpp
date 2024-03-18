#include "PuyoNode.h"


Vec2 Position;
Color Type;

PuyoNode::PuyoNode(Vec2 pos, Color type)
{
	Position = pos;
	Type = type;
}

PuyoNode::PuyoNode(Vec2 pos)
{
	Position = pos;
	Type = static_cast<Color>(rand()%5);
}
