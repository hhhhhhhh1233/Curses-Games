#include "Puyo.h"


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

void Puyo::MoveDown()
{
	Pivot.Position.y++;
	UpdateTagalong();
}

void Puyo::UpdateTagalong()
{
	Tagalong.Position = Vec2(Pivot.Position.x + rotations[currentRotation].x,Pivot.Position.y + rotations[currentRotation].y);
}

void Puyo::Draw(int X, int Y)
{
	attron(COLOR_PAIR(((int)Pivot.Type)+2));
	move(Pivot.Position.y + Y, Pivot.Position.x + X);
	printw("%c", ctoch[Pivot.Type]);
	//attroff(COLOR_PAIR(REDPUYOFIELD_PAIR));
	attroff(COLOR_PAIR(((int)Pivot.Type)+2));
	attron(COLOR_PAIR(((int)Tagalong.Type)+2));
	//attron(COLOR_PAIR(BLUEPUYOFIELD_PAIR));
	move(Tagalong.Position.y + Y, Tagalong.Position.x + X);
	printw("%c", ctoch[Tagalong.Type]);
	attroff(COLOR_PAIR(((int)Tagalong.Type)+2));
	//attroff(COLOR_PAIR(BLUEPUYOFIELD_PAIR));
}
