#include <curses.h>
#include <vector>
#include <unistd.h>

using std::vector;

constexpr int UP = 65;
constexpr int DOWN = 66;
constexpr int RIGHT = 67;
constexpr int LEFT = 68;

constexpr char EMPTYGRID = '*';
constexpr char SETPUYO = 'D';
constexpr char ACTIVEPUYO = 'P';

constexpr int FRAMESTOSLEEP = 10000;

struct Vec2
{
	int x;
	int y;

	Vec2 (int X, int Y)
	{
		x = X;
		y = Y;
	}
};

struct Node
{
	int x;
	int y;
	char Body;
	Vec2 rotations[4] = { Vec2(0, -1), Vec2(1, 0), Vec2(0, 1), Vec2(-1, 0) };
	int currentRotation;

	Node(int X, int Y, int body)
	{
		x = X;
		y = Y;
		Body = body;
		currentRotation = 0;
	}

	void CW()
	{
		currentRotation++;
		currentRotation = currentRotation % 4;
	}

	void CCW()
	{
		currentRotation--;
		if (currentRotation == -1)
			currentRotation = 3;
	}

	void Draw()
	{
		move(y, x);
		printw("%c", Body);
		move(y + rotations[currentRotation].y, x + rotations[currentRotation].x);
		printw("%c", 'O');
	}

	void Draw(int X, int Y)
	{
		move(y + Y, x + X);
		printw("%c", Body);
		move(y + Y + rotations[currentRotation].y, x + X + rotations[currentRotation].x);
		printw("%c", 'O');
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
		if (x < 0 || y < 0 || y > width || x > height)
			return ' ';
		return s[x * width + y];
	}

	void SetChar(int x, int y, char c)
	{
		if (x < 0 || y < 0 || y > width || x > height)
			return;
		s[x * width + y] = c;
	}

	bool IsEmpty(int x, int y)
	{
		return GetChar(y, x) == EMPTYGRID;
	}

	void SetActive(Node n)
	{
		SetChar(n.x, n.y, ACTIVEPUYO);
		SetChar(n.x + n.rotations[n.currentRotation].x, n.y + n.rotations[n.currentRotation].y, ACTIVEPUYO);
	}

	void SetOccupied(Node n)
	{
		SetChar(n.y, n.x, SETPUYO);
		SetChar(n.y + n.rotations[n.currentRotation].y, n.x + n.rotations[n.currentRotation].x, SETPUYO);
	}

	void Draw(int X, int Y)
	{
		for (int x = 0; x < width; x++)
		{
			for (int y = 0; y < height; y++)
			{
				move(y + Y, x + X);
				printw("%c", GetChar(y, x));
			}
		}
	}
};

class Field
{
public:
	Vec2 GridPosition;
	Grid grid;
	Node ActivePuyo;
	vector<Node> SetPuyos;

	Field(Vec2 gridPos, int width, int height) : GridPosition(gridPos), grid(Grid(width, height)), ActivePuyo(Node(width/2, 0, 'X')) {}

	void PuyoFall()
	{
		int attachedX = ActivePuyo.x + ActivePuyo.rotations[ActivePuyo.currentRotation].x;
		int attachedY = ActivePuyo.y + ActivePuyo.rotations[ActivePuyo.currentRotation].y;
		if (grid.IsEmpty(ActivePuyo.x, ActivePuyo.y + 1) && grid.IsEmpty(attachedX, attachedY + 1))
			ActivePuyo.y++;
		else
		{
			SetPuyos.push_back(ActivePuyo);
			grid.SetOccupied(ActivePuyo);
			ActivePuyo = Node(grid.width/2, 0, 'X');
		}
	}

	void PuyoMoveLeft()
	{
		int attachedX = ActivePuyo.x + ActivePuyo.rotations[ActivePuyo.currentRotation].x;
		int attachedY = ActivePuyo.y + ActivePuyo.rotations[ActivePuyo.currentRotation].y;
		if (grid.IsEmpty(ActivePuyo.x - 1, ActivePuyo.y) && grid.IsEmpty(attachedX - 1, attachedY))
			ActivePuyo.x--;
	}
	
	void PuyoMoveRight()
	{
		int attachedX = ActivePuyo.x + ActivePuyo.rotations[ActivePuyo.currentRotation].x;
		int attachedY = ActivePuyo.y + ActivePuyo.rotations[ActivePuyo.currentRotation].y;
		if (grid.IsEmpty(ActivePuyo.x + 1, ActivePuyo.y) && grid.IsEmpty(attachedX + 1, attachedY))
			ActivePuyo.x++;
	}
	
	void Draw()
	{
		grid.Draw(GridPosition.x, GridPosition.y);
		ActivePuyo.Draw(GridPosition.x, GridPosition.y);
		for (auto puyo : SetPuyos)
			puyo.Draw(GridPosition.x, GridPosition.y);
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

	// Configuration
	noecho();				// Don't echo input to the screen
	curs_set(0);			// Hide the cursor
	nodelay(stdscr, TRUE);	// Make input nonblocking

	while (!quit)
	{
		// If there is input, put it on screen
		if ((ch = getch()) != ERR)
		{
			clear();
			move(1, 1);
			if (ch == UP)
				f.ActivePuyo.CW();
			else if (ch == DOWN)
				f.PuyoFall();
			else if (ch == RIGHT)
				f.PuyoMoveRight();
			else if (ch == LEFT)
				f.PuyoMoveLeft();
		}

		// Only drop the puyo every 10000 frames
		if (i % FRAMESTOSLEEP == 0)
		{
			clear();
			f.PuyoFall();
			move(3,1);
		}
		f.Draw();
		refresh();
		i++;
	}

	// Reset the terminal situation
	endwin();

	return 0;
}
