#include <curses.h>
#include <iostream>
#include <vector>
#include <unistd.h>

using std::vector;

constexpr int UP = 65;
constexpr int DOWN = 66;
constexpr int RIGHT = 67;
constexpr int LEFT = 68;
constexpr int SPACE = 32;

constexpr char EMPTYGRID = '*';
constexpr char SETPUYO = 'D';
constexpr char ACTIVEPUYO = 'P';

constexpr int FRAMESTOSLEEP = 10000;

constexpr int EMPTYFIELD_PAIR = 1;
constexpr int REDPUYOFIELD_PAIR = 2;
constexpr int BLUEPUYOFIELD_PAIR = 3;

enum Color { red, blue, yellow, green, purple };

struct Vec2
{
	int x;
	int y;

	Vec2 (int X = 0, int Y = 0)
	{
		x = X;
		y = Y;
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
		//attron(COLOR_PAIR(REDPUYOFIELD_PAIR));
		move(Pivot.Position.y + Y, Pivot.Position.x + X);
		printw("%c", Body);
		//attroff(COLOR_PAIR(REDPUYOFIELD_PAIR));
		//attron(COLOR_PAIR(BLUEPUYOFIELD_PAIR));
		move(Tagalong.Position.y + Y, Tagalong.Position.x + X);
		printw("%c", 'O');
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
		for (int x = 0; x < width; x++)
		{
			for (int y = 0; y < height; y++)
			{
				//attron(COLOR_PAIR(EMPTYFIELD_PAIR));
				move(y + Y, x + X);
				printw("%c", GetChar(y, x));
				//attroff(COLOR_PAIR(EMPTYFIELD_PAIR));
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
	vector<Puyo> SetPuyos;

	Field(Vec2 gridPos, int width, int height) : GridPosition(gridPos), grid(Grid(width, height)), ActivePuyo(Puyo(width/2, 0, Color::red, 'X')) {}

	void PuyoFall()
	{
		if (grid.IsEmpty(ActivePuyo.Pivot.Position.x, ActivePuyo.Pivot.Position.y + 1) && grid.IsEmpty(ActivePuyo.Tagalong.Position.x, ActivePuyo.Tagalong.Position.y + 1))
			ActivePuyo.MoveDown();
		else
		{
			SetPuyos.push_back(ActivePuyo);
			grid.SetOccupied(ActivePuyo);
//			move(50,50);
			//printw("%d", grid.width/2);
			//refresh();
			//sleep(2);
			ActivePuyo = Puyo(grid.width/2, 0, Color::red, 'X');
		}
	}

	void PuyoMoveLeft()
	{
		if (grid.IsEmpty(ActivePuyo.Pivot.Position.x - 1, ActivePuyo.Pivot.Position.y) && grid.IsEmpty(ActivePuyo.Tagalong.Position.x - 1, ActivePuyo.Tagalong.Position.y))
			ActivePuyo.MoveLeft();
	}
	
	void PuyoMoveRight()
	{
//		move(50,50);
//		printw("%c", grid.GetChar(ActivePuyo.Pivot.Position.x + 1, ActivePuyo.Pivot.Position.y));
//		refresh();
//		sleep(1);
		if (grid.IsEmpty(ActivePuyo.Pivot.Position.x + 1, ActivePuyo.Pivot.Position.y) && grid.IsEmpty(ActivePuyo.Tagalong.Position.x + 1, ActivePuyo.Tagalong.Position.y))
			ActivePuyo.MoveRight();
	}

	void PuyoRotateClockwise()
	{
		ActivePuyo.CW();
		if (!grid.IsEmpty(ActivePuyo.Tagalong.Position.x, ActivePuyo.Tagalong.Position.y))
		{
			ActivePuyo.CW();
		}
	}

	void PuyoDrop()
	{
		while (grid.IsEmpty(ActivePuyo.Pivot.Position.x, ActivePuyo.Pivot.Position.y + 1) && grid.IsEmpty(ActivePuyo.Tagalong.Position.x, ActivePuyo.Tagalong.Position.y + 1))
		{
			PuyoFall();
		}
		PuyoFall();
	}
	
	void Draw()
	{
		grid.Draw(GridPosition.x, GridPosition.y);
		//for (auto puyo : SetPuyos)
		//	puyo.Draw(GridPosition.x, GridPosition.y);
		ActivePuyo.Draw(GridPosition.x, GridPosition.y);
	}

};


int main()
{
	Field f(Vec2(30,20), 10, 15);
	
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
	//start_color();			// Allows us to have colors in our game 

	init_pair(EMPTYFIELD_PAIR, COLOR_WHITE, COLOR_WHITE);
	init_pair(REDPUYOFIELD_PAIR, COLOR_RED, COLOR_RED);
	init_pair(BLUEPUYOFIELD_PAIR, COLOR_BLUE, COLOR_BLUE);
	while (!quit)
	{
		move(50,50);
		printw("%d, %d", f.ActivePuyo.Pivot.Position.x, f.ActivePuyo.Pivot.Position.y); 
		if ((ch = getch()) != ERR)
		{
			clear();
			if (ch == UP)
				f.PuyoRotateClockwise();
			else if (ch == DOWN)
				f.PuyoFall();
			else if (ch == RIGHT)
				f.PuyoMoveRight();
			else if (ch == LEFT)
				f.PuyoMoveLeft();
			else if (ch == SPACE)
				f.PuyoDrop();
		}

		// Only drop the puyo every 10000 frames
		if (i % FRAMESTOSLEEP == 0)
		{
			clear();
			f.PuyoFall();
		}
		f.Draw();
		refresh();
		i++;
	}

	// Reset the terminal situation
	endwin();

	return 0;
}
