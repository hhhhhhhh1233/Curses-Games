#include <curses.h>
#include <string>
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
		if (x < 0 || y < 0 || x > width || y > height)
			return;
		s[x * width + y] = c;
	}

	bool IsEmpty(int x, int y)
	{
		return GetChar(x, y) == EMPTYGRID;
	}

	void Draw(int X = 0, int Y = 0)
	{
		for (int y = 0; y < height; y++)
		{
			for (int x = 0; x < width; x++)
			{
				move(y + Y, x + X);
				printw("%c", GetChar(y, x));
				move(0,20);
				printw("(width: %d) * (y: %d) + (x: %d) = (index: %d), newval(%c)", width, y, x, y * width + x, GetChar(y,x));
				refresh();
				sleep(1);
			}
		}
	}
};

int main()
{
	Grid grid(4,10);

	grid.s[0] = 'a';
	grid.s[4] = 'b';
	grid.s[8] = 'c';
	grid.s[12] = 'd';
	grid.s[16] = 'e';
	grid.s[20] = 'f';
	
	std::string ha;
	for (auto n : grid.s)
		ha.push_back(n);
	const char* haa = ha.c_str();

	// Variables
	int ch;
	bool quit = false;
	int i = 0;

	// Initiate the screen
	initscr();

	// Configuration
	noecho();				// Don't echo input to the screen
	nodelay(stdscr, TRUE);	// Make input nonblocking

	while (!quit)
	{
		move(10,2);
		printw(haa);

		// Only drop the puyo every 10000 frames
		if (i % FRAMESTOSLEEP == 0)
		{
			clear();
		}
		grid.Draw();
		refresh();
		i++;
	}

	// Reset the terminal situation
	endwin();

	return 0;
}
