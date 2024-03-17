#include <algorithm>
#include <curses.h>
#include <iostream>
#include <map>
#include <vector>
#include <unistd.h>

using std::map;
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
	}

	Puyo(int X, int Y, int body) : Pivot(PuyoNode(Vec2())), Tagalong(PuyoNode(Vec2()))
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
		printw("%c", ctoch[Pivot.Type]);
		//attroff(COLOR_PAIR(REDPUYOFIELD_PAIR));
		//attron(COLOR_PAIR(BLUEPUYOFIELD_PAIR));
		move(Tagalong.Position.y + Y, Tagalong.Position.x + X);
		printw("%c", ctoch[Tagalong.Type]);
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

	void ClearPuyos()
	{
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
					}
				}
			}
		}
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
	vector<Puyo> SetPuyos;

	Field(Vec2 gridPos, int width, int height) : GridPosition(gridPos), grid(Grid(width, height)), ActivePuyo(Puyo(width/2, 0, 'X')) {}

	void PuyoFall()
	{
		if (grid.IsEmpty(ActivePuyo.Pivot.Position.x, ActivePuyo.Pivot.Position.y + 1) && grid.IsEmpty(ActivePuyo.Tagalong.Position.x, ActivePuyo.Tagalong.Position.y + 1))
			ActivePuyo.MoveDown();
		else
		{
			SetPuyos.push_back(ActivePuyo);
			while (grid.IsEmpty(ActivePuyo.Pivot.Position.x, ActivePuyo.Pivot.Position.y + 1) && ActivePuyo.Pivot.Position.y + 1 != ActivePuyo.Tagalong.Position.y)
			{
				ActivePuyo.Pivot.Position.y++;
				Draw();
				refresh();
				sleep(1);
			}
			grid.SetChar(ActivePuyo.Pivot.Position.y, ActivePuyo.Pivot.Position.x, ctoch[ActivePuyo.Pivot.Type]);
			while (grid.IsEmpty(ActivePuyo.Tagalong.Position.x, ActivePuyo.Tagalong.Position.y + 1))
			{
				ActivePuyo.Tagalong.Position.y++;
				Draw();
				refresh();
				sleep(1);
			}
			grid.SetChar(ActivePuyo.Tagalong.Position.y, ActivePuyo.Tagalong.Position.x, ctoch[ActivePuyo.Tagalong.Type]);
			grid.ClearPuyos();
			grid.DropAllPuyos();
			ActivePuyo = Puyo(grid.width/2, 0, 'X');
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
	srand(time(NULL));
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
