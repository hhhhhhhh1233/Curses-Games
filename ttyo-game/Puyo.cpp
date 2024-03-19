#include "Puyo.h"
#include "Color.h"

Puyo::Puyo(int X, int Y, Color t, int body) : Pivot(PuyoNode(Vec2(), t)), Tagalong(PuyoNode(Vec2(), t))
{
	Body = body;
	currentRotation = 0;
	UpdateTagalong();
}

Puyo::Puyo(int X, int Y, int body) : Pivot(PuyoNode(Vec2(X, Y))), Tagalong(PuyoNode(Vec2()))
{
	Body = body;
	currentRotation = 0;
	UpdateTagalong();
}

void Puyo::CW()
{
	currentRotation++;
	currentRotation = currentRotation % 4;
	UpdateTagalong();
}

void Puyo::CCW()
{
	currentRotation--;
	if (currentRotation == -1)
		currentRotation = 3;
	UpdateTagalong();
}

void Puyo::MoveLeft()
{
	Pivot.Position.x--;
	UpdateTagalong();
}

void Puyo::MoveRight()
{
	Pivot.Position.x++;
	UpdateTagalong();
}

void Puyo::MoveUp()
{
	Pivot.Position.y--;
	UpdateTagalong();
}

void Puyo::MoveDown()
{
	Pivot.Position.y++;
	UpdateTagalong();
}

void Puyo::UpdateTagalong()
{
	Tagalong.Position = Vec2(Pivot.Position.x + rotations[currentRotation].x,Pivot.Position.y + rotations[currentRotation].y);
}

void Puyo::Draw(int OffsetX, int OffsetY, int CellWidth, int CellHeight)
{
	for (int y = 0; y < 3; y++)
	{
		for (int x = 0; x < 5; x++)
		{
			if (x == 0 && y == 0 || x == 4 && y == 0 || x == 1 && y == 1 || x == 3 && y == 1)
				attron(COLOR_PAIR(EMPTYFIELD_PAIR));
			else
				attron(COLOR_PAIR(((int)Pivot.Type)+2));
			move(OffsetY + Pivot.Position.y * CellHeight + y, OffsetX + Pivot.Position.x * CellWidth + x);
			printw("%c", ctoch[Pivot.Type]);
			if (x == 0 && y == 0 || x == 4 && y == 0 || x == 1 && y == 1 || x == 3 && y == 1)
				attroff(COLOR_PAIR(EMPTYFIELD_PAIR));
			else
				attroff(COLOR_PAIR(((int)Pivot.Type)+2));
		}
	}
	for (int y = 0; y < 3; y++)
	{
		for (int x = 0; x < 5; x++)
		{
			if (x == 0 && y == 0 || x == 4 && y == 0 || x == 1 && y == 1 || x == 3 && y == 1)
				attron(COLOR_PAIR(EMPTYFIELD_PAIR));
			else
				attron(COLOR_PAIR(((int)Tagalong.Type)+2));
			move(OffsetY + Tagalong.Position.y * CellHeight + y, OffsetX + Tagalong.Position.x * CellWidth + x);
			printw("%c", ctoch[Tagalong.Type]);
			if (x == 0 && y == 0 || x == 4 && y == 0 || x == 1 && y == 1 || x == 3 && y == 1)
				attroff(COLOR_PAIR(EMPTYFIELD_PAIR));
			else
				attroff(COLOR_PAIR(((int)Tagalong.Type)+2));
		}
	}
}
