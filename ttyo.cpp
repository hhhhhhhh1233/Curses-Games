#include <algorithm>
#include <chrono>
#include <cstdlib>
#include <curses.h>
#include <iostream>
#include <map>
#include <thread>
#include <vector>
#include <unistd.h>

using std::map;
using std::vector;

constexpr int UP = 65;
constexpr int DOWN = 66;
constexpr int RIGHT = 67;
constexpr int LEFT = 68;
constexpr int SPACE = 32;
constexpr int QUIT_KEY = 113;
constexpr int Z_KEY = 122;
constexpr int X_KEY = 120;

constexpr char EMPTYGRID = '*';
constexpr char SETPUYO = 'D';
constexpr char ACTIVEPUYO = 'P';

constexpr int FRAMESTOSLEEP = 5000;

constexpr int EMPTYFIELD_PAIR = 1;
constexpr int REDFIELD_PAIR = 2;
constexpr int BLUEFIELD_PAIR = 3;
constexpr int YELLOWFIELD_PAIR = 4;
constexpr int GREENFIELD_PAIR = 5;
constexpr int PURPLEFIELD_PAIR = 6;
constexpr int OUTLINEFIELD_PAIR = 7;
constexpr int DEATHFIELD_PAIR = 8;

enum Color { red, blue, yellow, green, purple };
map<Color, char> ctoch = {{Color::red, 'R'}, {Color::blue, 'B'}, {Color::yellow, 'Y'}, {Color::green, 'G'}, {Color::purple, 'P'}};

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

class PuyoNode
{
public:
	Vec2 Position;
	Color Type;

	PuyoNode(Vec2 pos, Color type)
	{
		Position = pos;
		Type = type;
	}

	PuyoNode(Vec2 pos)
	{
		Position = pos;
		Type = static_cast<Color>(rand()%5);
	}
};

struct Puyo
{
	PuyoNode Pivot;
	PuyoNode Tagalong;

	char Body;
	Vec2 rotations[4] = { Vec2(0, -1), Vec2(1, 0), Vec2(0, 1), Vec2(-1, 0) };
	int currentRotation;

	Puyo(int X, int Y, Color t, int body) : Pivot(PuyoNode(Vec2(), t)), Tagalong(PuyoNode(Vec2(), t))
	{
		Body = body;
		currentRotation = 0;
		UpdateTagalong();
	}

	Puyo(int X, int Y, int body) : Pivot(PuyoNode(Vec2(X, Y))), Tagalong(PuyoNode(Vec2()))
	{
		Body = body;
		currentRotation = 0;
		UpdateTagalong();
	}

	void CW()
	{
		currentRotation++;
		currentRotation = currentRotation % 4;
		UpdateTagalong();
	}

	void CCW()
	{
		currentRotation--;
		if (currentRotation == -1)
			currentRotation = 3;
		UpdateTagalong();
	}

	void MoveLeft()
	{
		Pivot.Position.x--;
		UpdateTagalong();
	}

	void MoveRight()
	{
		Pivot.Position.x++;
		UpdateTagalong();
	}

	void MoveDown()
	{
		Pivot.Position.y++;
		UpdateTagalong();
	}

	void UpdateTagalong()
	{
		Tagalong.Position = Vec2(Pivot.Position.x + rotations[currentRotation].x,Pivot.Position.y + rotations[currentRotation].y);
	}

	void Draw(int X, int Y)
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
};

class Grid
{
public:
	vector<char> s;
	int width;
	int height;

	Grid(int Width, int Height)
	{
		width = Width;
		height = Height;
		for (int i = 0; i < Width * Height; i++)
			s.push_back(EMPTYGRID);
	}

	char GetChar(int x, int y)
	{
		if (x < 0 || y < 0 || y > width - 1 || x > height)
			return ' ';
		return s[x * width + y];
	}

	void SetChar(int x, int y, char c)
	{
		if (x < 0 || y < 0 || y >= width || x >= height)
			return;
		s[x * width + y] = c;
	}

	bool IsEmpty(int x, int y)
	{
		return GetChar(y, x) == EMPTYGRID;
	}

	void SetActive(Puyo n)
	{
		SetChar(n.Pivot.Position.y, n.Pivot.Position.x, ACTIVEPUYO);
		SetChar(n.Pivot.Position.y + n.rotations[n.currentRotation].y, n.Pivot.Position.x + n.rotations[n.currentRotation].x, ACTIVEPUYO);
	}

	void SetOccupied(Puyo n)
	{
		SetChar(n.Pivot.Position.y, n.Pivot.Position.x, SETPUYO);
		SetChar(n.Pivot.Position.y + n.rotations[n.currentRotation].y, n.Pivot.Position.x + n.rotations[n.currentRotation].x, SETPUYO);
	}

	void Draw(int X, int Y)
	{
		for (int x = -1; x < width+1; x++)
		{
			for (int y = -1; y < height+1; y++)
			{
				attron(COLOR_PAIR(OUTLINEFIELD_PAIR));
				move(y + Y, x + X);
				printw(" ");
				attroff(COLOR_PAIR(OUTLINEFIELD_PAIR));
			}
		}
		map<char, int> ColorPair = {{EMPTYGRID, 1}, {'R', 2}, {'B', 3}, {'Y', 4}, {'G', 5}, {'P', 6}};
		for (int x = 0; x < width; x++)
		{
			for (int y = 0; y < height; y++)
			{
				attron(COLOR_PAIR(ColorPair[GetChar(y,x)]));
				move(y + Y, x + X);
				printw("%c", GetChar(y, x));
				attroff(COLOR_PAIR(ColorPair[GetChar(y,x)]));
			}
		}
	}

	int ClearPuyos()
	{
		int Cleared = 0;
		for (int x = 0; x < width; x++)
		{
			for (int y = 0; y < height; y++)
			{
				if (GetChar(y, x) != EMPTYGRID)
				{
					vector<Vec2> PotentialClearable;
					vector<Vec2> Candidates = {{x+1,y}, {x-1,y}, {x,y+1}, {x,y-1}};
					PotentialClearable.push_back(Vec2(x, y));
					while (Candidates.size() != 0)
					{
						if (GetChar(Candidates[0].y, Candidates[0].x) == GetChar(y,x))
						{
							PotentialClearable.push_back(Candidates[0]);
							vector<Vec2> Neighbors = {{Candidates[0].x+1, Candidates[0].y}, {Candidates[0].x-1, Candidates[0].y}, {Candidates[0].x, Candidates[0].y+1}, {Candidates[0].x, Candidates[0].y-1}};
							for (auto vec : Neighbors)
							{
								if (GetChar(vec.y, vec.x) == GetChar(y, x) && std::find(PotentialClearable.begin(), PotentialClearable.end(), vec) == PotentialClearable.end())
								{
									Candidates.push_back(vec);
								}
							}
						}
						Candidates.erase(Candidates.begin());
					}
					if (PotentialClearable.size() >= 4)
					{
						for (auto puyo : PotentialClearable)
						{
							SetChar(puyo.y, puyo.x, EMPTYGRID);
						}
						Cleared = PotentialClearable.size();
					}
				}
			}
		}
		return Cleared;
	}

	void DropAllPuyos()
	{
		vector<int> ClearColumns;
		for (int x = width - 1; x >= 0; x--)
		{
			for (int y = height; y >= 0; y--)
			{
				if (GetChar(y, x) == EMPTYGRID)
				{
					int i = 1;
					while (y - i >= 0 && GetChar(y-i, x) == EMPTYGRID)
						i++;
					if (y-i>=0)
					{
						SetChar(y, x, GetChar(y-i, x));
						SetChar(y-i, x, EMPTYGRID);
					}
				}
			}
		}
	}
};

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

	Field(Vec2 gridPos, int width, int height, bool Player = true) : GridPosition(gridPos), grid(Grid(width, height)), ActivePuyo(Puyo((width-1)/2, 1, 'X')), NextPuyo(Puyo((width-1)/2, 1, 'X')), Score(0), GameRunning(true), CanPause(Player) {}

	void PuyoFall()
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

	void PuyoMoveLeft()
	{
		if (!GameRunning)
			return;
		if (grid.IsEmpty(ActivePuyo.Pivot.Position.x - 1, ActivePuyo.Pivot.Position.y) && grid.IsEmpty(ActivePuyo.Tagalong.Position.x - 1, ActivePuyo.Tagalong.Position.y))
			ActivePuyo.MoveLeft();
	}
	
	void PuyoMoveRight()
	{
		if (!GameRunning)
			return;
		if (grid.IsEmpty(ActivePuyo.Pivot.Position.x + 1, ActivePuyo.Pivot.Position.y) && grid.IsEmpty(ActivePuyo.Tagalong.Position.x + 1, ActivePuyo.Tagalong.Position.y))
			ActivePuyo.MoveRight();
	}

	void PuyoRotateClockwise()
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

	void PuyoRotateCounterClockwise()
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

	void GameOver()
	{
		GameRunning = false;
	}

	void PuyoDrop()
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
	
	void Draw()
	{
		grid.Draw(GridPosition.x, GridPosition.y);
		DrawActiveHint();
		move(GridPosition.y + 0,GridPosition.x + grid.width + 2);
		printw("Score: ");
		move(GridPosition.y + 1,GridPosition.x + grid.width + 2);
		printw("%d", Score);
		move(GridPosition.y + 3, GridPosition.x + grid.width + 2);
		printw("Next:");
		NextPuyo.Draw(GridPosition.x + grid.width, GridPosition.y + 4);
		ActivePuyo.Draw(GridPosition.x, GridPosition.y);
		DrawXs();
		if (!GameRunning)
		{
			move(GridPosition.y + grid.height/2, GridPosition.x + grid.width/2 - 4);
			printw("GAME OVER");
		}
	}

	void DrawBare()
	{
		grid.Draw(GridPosition.x, GridPosition.y);
		DrawXs();
	}

	void DrawActiveHint()
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
		move(GridPosition.y + ActivePuyo.Pivot.Position.y + PivotOffset - 1, GridPosition.x + ActivePuyo.Pivot.Position.x);
		printw("*");
		attroff(COLOR_PAIR(((int)ActivePuyo.Pivot.Type)+2));

		attron(COLOR_PAIR(((int)ActivePuyo.Tagalong.Type)+2));
		move(GridPosition.y + ActivePuyo.Tagalong.Position.y + TagalongOffset - 1, GridPosition.x + ActivePuyo.Tagalong.Position.x);
		printw("*");
		attroff(COLOR_PAIR(((int)ActivePuyo.Tagalong.Type)+2));
	}

	void DrawXs()
	{
		attron(COLOR_PAIR(DEATHFIELD_PAIR));
		for (int i = 0; i < grid.width; i++)
			mvaddch(GridPosition.y-1, GridPosition.x + i, 'X');
		attroff(COLOR_PAIR(DEATHFIELD_PAIR));
	}

};


int main()
{
	srand(time(NULL));

	system("resize -s 50 50");

	Field f(Vec2(2,2), 6, 12);
	
	Field AIField(Vec2(21,2), 6, 12, false);
	
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
	init_pair(REDFIELD_PAIR, COLOR_RED, COLOR_BLACK);
	init_pair(BLUEFIELD_PAIR, COLOR_BLUE, COLOR_BLACK);
	init_pair(YELLOWFIELD_PAIR, COLOR_YELLOW, COLOR_BLACK);
	init_pair(GREENFIELD_PAIR, COLOR_GREEN, COLOR_BLACK);
	init_pair(PURPLEFIELD_PAIR, COLOR_MAGENTA, COLOR_BLACK);
	init_pair(OUTLINEFIELD_PAIR, COLOR_WHITE, COLOR_WHITE);
	init_pair(DEATHFIELD_PAIR, COLOR_RED, COLOR_WHITE);

	auto StartTime = std::chrono::system_clock::now();
	int playTiming = 0;
	int aiTiming = 0;

	while (f.GameRunning && !quit)
	{
		auto tim = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now() - StartTime).count();
		move(20,0);
		printw("%d\n%d", (int)tim % 1000, (int)tim / 1000);
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

		if (aiTiming == tim/1000)
		{
			aiTiming++;
			int choice = rand()%4;
			if (choice == 0)
				AIField.PuyoRotateClockwise();
			else if (choice == 1)
				AIField.PuyoRotateCounterClockwise();
			else if (choice == 2)
				AIField.PuyoMoveRight();
			else if (choice == 3)
				AIField.PuyoMoveLeft();
		}

		// Only drop the puyo every few thousand frames
		if (playTiming == tim/1000)
		{
			playTiming++;
			if (ch != DOWN)
				f.PuyoFall();
			AIField.PuyoFall();
		}
		f.Draw();
		AIField.Draw();
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
