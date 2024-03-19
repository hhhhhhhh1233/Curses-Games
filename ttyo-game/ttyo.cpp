#include <chrono>
#include <cstdlib>
#include <curses.h>
#include <iostream>
#include <unistd.h>
#include "Field.h"

constexpr int UP = 65;
constexpr int DOWN = 66;
constexpr int RIGHT = 67;
constexpr int LEFT = 68;
constexpr int SPACE = 32;
constexpr int QUIT_KEY = 113;
constexpr int Z_KEY = 122;
constexpr int X_KEY = 120;

constexpr int FRAMESTOSLEEP = 5000;

int main()
{
	srand(time(NULL));

	system("resize -s 50 50");

	Field f(Vec2(2,2), 6, 12, 5, 3);
	
	//Field AIField(Vec2(21,2), 6, 12, 5, 3, false);
	
	// Variables
	int ch;
	bool quit = false;
	int i = 0;

	// Initiate the screen
	initscr();

	if (!has_colors())
	{
		endwin();
		std::cout << "Your terminal does not support color\n";
		exit(1);
	}

	// Configuration
	noecho();				// Don't echo input to the screen
	curs_set(0);			// Hide the cursor
	nodelay(stdscr, TRUE);	// Make input nonblocking
	start_color();			// Allows us to have colors in our game 

	init_pair(EMPTYFIELD_PAIR, COLOR_BLACK, COLOR_BLACK);
	init_pair(REDFIELD_PAIR, COLOR_RED, COLOR_RED);
	init_pair(BLUEFIELD_PAIR, COLOR_BLUE, COLOR_BLUE);
	init_pair(YELLOWFIELD_PAIR, COLOR_YELLOW, COLOR_YELLOW);
	init_pair(GREENFIELD_PAIR, COLOR_GREEN, COLOR_GREEN);
	init_pair(PURPLEFIELD_PAIR, COLOR_MAGENTA, COLOR_MAGENTA);
	init_pair(OUTLINEFIELD_PAIR, COLOR_WHITE, COLOR_WHITE);
	init_pair(DEATHFIELD_PAIR, COLOR_RED, COLOR_WHITE);

	auto StartTime = std::chrono::system_clock::now();
	int playTiming = 0;
	//int aiTiming = 0;

	while (f.GameRunning && !quit)
	{
		auto tim = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now() - StartTime).count();
		//move(20,0);
		//printw("%d\n%d", (int)tim % 1000, (int)tim / 1000);
		//printw("%d, %d", f.ActivePuyo.Pivot.Position.x, f.ActivePuyo.Pivot.Position.y); 
		if ((ch = getch()) != ERR)
		{
			if (ch == UP)
				f.PuyoRotateClockwise();
			else if (ch == Z_KEY)
				f.PuyoRotateCounterClockwise();
			else if (ch == X_KEY)
				f.PuyoRotateClockwise();
			else if (ch == DOWN && f.ActivePuyo.Pivot.Position.y > 2)
			{
				f.PuyoFall();
				f.Score += 3;
			}
			else if (ch == RIGHT)
				f.PuyoMoveRight();
			else if (ch == LEFT)
				f.PuyoMoveLeft();
			else if (ch == SPACE)
				f.PuyoDrop();
			else if (ch == QUIT_KEY)
				quit = true;
		}

		//if (aiTiming == tim/1000)
		//{
		//	aiTiming++;
		//	int choice = rand()%4;
		//	if (choice == 0)
		//		AIField.PuyoRotateClockwise();
		//	else if (choice == 1)
		//		AIField.PuyoRotateCounterClockwise();
		//	else if (choice == 2)
		//		AIField.PuyoMoveRight();
		//	else if (choice == 3)
		//		AIField.PuyoMoveLeft();
		//}

		// Only drop the puyo every few thousand frames
		if (playTiming == tim/1000)
		{
			playTiming++;
			if (ch != DOWN)
				f.PuyoFall();
			//AIField.PuyoFall();
		}
		f.Draw();
		//AIField.Draw();
		refresh();
		i++;
	}

	while(!quit)
	{
		if ((ch = getch()) != ERR)
		{
			if (ch == QUIT_KEY)
				quit = true;
		}
		refresh();
	}

	// Reset the terminal situation
	endwin();

	return 0;
}
