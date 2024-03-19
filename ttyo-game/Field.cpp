#include "Field.h"

Field::Field(Vec2 gridPos, int width, int height, int cellWidth, int cellHeight, bool Player) : GridPosition(gridPos), grid(Grid(width, height, cellWidth, cellHeight)), ActivePuyo(Puyo((width-1)/2, 1, 'X')), NextPuyo(Puyo((width-1)/2, 1, 'X')), Score(0), GameRunning(true), CanPause(Player) {}

void Field::PuyoFall()
{
	if (!GameRunning)
		return;
	if (grid.IsEmpty(ActivePuyo.Pivot.Position.x, ActivePuyo.Pivot.Position.y + 1) && grid.IsEmpty(ActivePuyo.Tagalong.Position.x, ActivePuyo.Tagalong.Position.y + 1))
		ActivePuyo.MoveDown();
	else
	{
		if (!grid.IsEmpty(ActivePuyo.Pivot.Position.x, ActivePuyo.Pivot.Position.y))
		{
			GameOver();
			return;
		}
		SetPuyos.push_back(ActivePuyo);
		while (grid.IsEmpty(ActivePuyo.Pivot.Position.x, ActivePuyo.Pivot.Position.y + 1) && ActivePuyo.Pivot.Position.y + 1 != ActivePuyo.Tagalong.Position.y)
		{
			ActivePuyo.Pivot.Position.y++;
			Draw();
			refresh();
		}
		grid.SetChar(ActivePuyo.Pivot.Position.y, ActivePuyo.Pivot.Position.x, ctoch[ActivePuyo.Pivot.Type]);
		while (grid.IsEmpty(ActivePuyo.Tagalong.Position.x, ActivePuyo.Tagalong.Position.y + 1))
		{
			ActivePuyo.Tagalong.Position.y++;
			Draw();
			refresh();
		}
		grid.SetChar(ActivePuyo.Tagalong.Position.y, ActivePuyo.Tagalong.Position.x, ctoch[ActivePuyo.Tagalong.Type]);
		int Chain = 1;
		while (int x = grid.ClearPuyos())
		{
			Score += (x - 3) * 100 * Chain;
			DrawBare();
			refresh();
			if (CanPause)
				std::this_thread::sleep_for(std::chrono::milliseconds(200));
			grid.DropAllPuyos();
			DrawBare();
			refresh();
			if (CanPause)
				std::this_thread::sleep_for(std::chrono::milliseconds(200));
			Chain++;
		}
		ActivePuyo = NextPuyo;
		NextPuyo = Puyo((grid.width-1)/2, 1, 'X');
	}
}

void Field::PuyoMoveLeft()
{
	if (!GameRunning)
		return;
	if (grid.IsEmpty(ActivePuyo.Pivot.Position.x - 1, ActivePuyo.Pivot.Position.y) && grid.IsEmpty(ActivePuyo.Tagalong.Position.x - 1, ActivePuyo.Tagalong.Position.y))
		ActivePuyo.MoveLeft();
}

void Field::PuyoMoveRight()
{
	if (!GameRunning)
		return;
	if (grid.IsEmpty(ActivePuyo.Pivot.Position.x + 1, ActivePuyo.Pivot.Position.y) && grid.IsEmpty(ActivePuyo.Tagalong.Position.x + 1, ActivePuyo.Tagalong.Position.y))
		ActivePuyo.MoveRight();
}

void Field::PuyoRotateClockwise()
{
	if (!GameRunning)
		return;
	ActivePuyo.CW();
	if (!grid.IsEmpty(ActivePuyo.Tagalong.Position.x, ActivePuyo.Tagalong.Position.y))
	{
		if (ActivePuyo.currentRotation == 1)
		{
			ActivePuyo.MoveLeft();
			if (!grid.IsEmpty(ActivePuyo.Pivot.Position.x, ActivePuyo.Pivot.Position.y))
			{
				ActivePuyo.MoveRight();
				ActivePuyo.CW();
			}
		}
		if (ActivePuyo.currentRotation == 3)
		{
			ActivePuyo.MoveRight();
			if (!grid.IsEmpty(ActivePuyo.Pivot.Position.x, ActivePuyo.Pivot.Position.y))
			{
				ActivePuyo.MoveLeft();
				ActivePuyo.CCW();
			}
		}
	}
}

void Field::PuyoRotateCounterClockwise()
{
	if (!GameRunning)
		return;
	ActivePuyo.CCW();
	if (!grid.IsEmpty(ActivePuyo.Tagalong.Position.x, ActivePuyo.Tagalong.Position.y))
	{
		if (ActivePuyo.currentRotation == 1)
		{
			ActivePuyo.MoveLeft();
			if (!grid.IsEmpty(ActivePuyo.Pivot.Position.x, ActivePuyo.Pivot.Position.y))
			{
				ActivePuyo.MoveRight();
				ActivePuyo.CW();
			}
		}
		if (ActivePuyo.currentRotation == 3)
		{
			ActivePuyo.MoveRight();
			if (!grid.IsEmpty(ActivePuyo.Pivot.Position.x, ActivePuyo.Pivot.Position.y))
			{
				ActivePuyo.MoveLeft();
				ActivePuyo.CCW();
			}
		}
	}
}

void Field::GameOver()
{
	GameRunning = false;
}

void Field::PuyoDrop()
{
	int PotentialScore = 0;
	while (grid.IsEmpty(ActivePuyo.Pivot.Position.x, ActivePuyo.Pivot.Position.y + 1) && grid.IsEmpty(ActivePuyo.Tagalong.Position.x, ActivePuyo.Tagalong.Position.y + 1))
	{
		PuyoFall();
		PotentialScore += 5;
	}
	PuyoFall();
	Score += PotentialScore;
}

void Field::Draw()
{
	grid.Draw(GridPosition.x, GridPosition.y);
	DrawActiveHint();
	//move(GridPosition.y + 0,GridPosition.x + grid.width + 2);
	//printw("Score: ");
	//move(GridPosition.y + 1,GridPosition.x + grid.width + 2);
	//printw("%d", Score);
	//move(GridPosition.y + 3, GridPosition.x + grid.width + 2);
	//printw("Next:");
	//NextPuyo.Draw(GridPosition.x + grid.width, GridPosition.y + 4);
	ActivePuyo.Draw(GridPosition.x, GridPosition.y, grid.cellWidth, grid.cellHeight);
	//DrawXs();
	if (!GameRunning)
	{
		move(GridPosition.y + grid.height/2, GridPosition.x + grid.width/2 - 4);
		printw("GAME OVER");
	}
}

void Field::DrawBare()
{
	grid.Draw(GridPosition.x, GridPosition.y);
	//DrawXs();
}

void Field::DrawActiveHint()
{
	int PivotOffset = 0;
	int TagalongOffset = 0;

	do
	{
		PivotOffset++;
	}
	while (ActivePuyo.Pivot.Position.y + PivotOffset < grid.height && grid.GetChar(ActivePuyo.Pivot.Position.y + PivotOffset, ActivePuyo.Pivot.Position.x) == EMPTYGRID);

	do
	{
		TagalongOffset++;
	}
	while (ActivePuyo.Tagalong.Position.y + TagalongOffset < grid.height && grid.GetChar(ActivePuyo.Tagalong.Position.y + TagalongOffset, ActivePuyo.Tagalong.Position.x) == EMPTYGRID);

	if (ActivePuyo.Pivot.Position.y > ActivePuyo.Tagalong.Position.y)
		TagalongOffset--;

	if (ActivePuyo.Pivot.Position.y < ActivePuyo.Tagalong.Position.y)
		PivotOffset--;

	attron(COLOR_PAIR(((int)ActivePuyo.Pivot.Type)+2));
	move(1 + GridPosition.y + (ActivePuyo.Pivot.Position.y + PivotOffset - 1) * grid.cellHeight, 2 + GridPosition.x + ActivePuyo.Pivot.Position.x * grid.cellWidth);
	printw("*");
	attroff(COLOR_PAIR(((int)ActivePuyo.Pivot.Type)+2));

	attron(COLOR_PAIR(((int)ActivePuyo.Tagalong.Type)+2));
	move(1 + GridPosition.y + (ActivePuyo.Tagalong.Position.y + TagalongOffset - 1) * grid.cellHeight, 2 + GridPosition.x + grid.cellWidth * ActivePuyo.Tagalong.Position.x);
	printw("*");
	attroff(COLOR_PAIR(((int)ActivePuyo.Tagalong.Type)+2));
}

void Field::DrawXs()
{
	attron(COLOR_PAIR(DEATHFIELD_PAIR));
	for (int i = 0; i < grid.width; i++)
		mvaddch(GridPosition.y-1, GridPosition.x + i, 'X');
	attroff(COLOR_PAIR(DEATHFIELD_PAIR));
}

