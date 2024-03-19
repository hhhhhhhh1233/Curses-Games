#include "Grid.h"
#include "Vec2.h"
#include <unistd.h>

Grid::Grid(int Width, int Height, int CellWidth, int CellHeight)
{
	width = Width;
	height = Height;
	cellWidth = CellWidth;
	cellHeight = CellHeight;
	for (int i = 0; i < Width * Height; i++)
		s.push_back(EMPTYGRID);
}

char Grid::GetChar(int x, int y)
{
	if (x < 0 || y < 0 || y > width - 1 || x > height)
		return ' ';
	return s[x * width + y];
}

void Grid::SetChar(int x, int y, char c)
{
	if (x < 0 || y < 0 || y >= width || x >= height)
		return;
	s[x * width + y] = c;
}

bool Grid::IsEmpty(int x, int y)
{
	return GetChar(y, x) == EMPTYGRID;
}

void Grid::Draw(int OffsetX, int OffsetY)
{
	for (int x = -1; x < (cellWidth * width)+1; x++)
	{
		for (int y = -1; y < (cellHeight*height)+1; y++)
		{
			attron(COLOR_PAIR(OUTLINEFIELD_PAIR));
			move(y + OffsetY, x + OffsetX);
			printw(" ");
			attroff(COLOR_PAIR(OUTLINEFIELD_PAIR));
		}
	}
	map<char, int> ColorPair = {{EMPTYGRID, 1}, {'R', 2}, {'B', 3}, {'Y', 4}, {'G', 5}, {'P', 6}};
	for (int x = 0; x < width; x++)
	{
		for (int y = 0; y < height; y++)
		{
			for(int cx = 0; cx < cellWidth; cx++)
			{
				for (int cy = 0; cy < cellHeight; cy++)
				{
					if (cx == 0 && cy == 0 || cx == 4 && cy == 0 || cx == 1 && cy == 1 || cx == 3 && cy == 1)
						attron(COLOR_PAIR(EMPTYFIELD_PAIR));
					else
						attron(COLOR_PAIR(ColorPair[GetChar(y,x)]));
					move(OffsetY + y * cellHeight + cy, OffsetX + x * cellWidth + cx);
					printw("%c", GetChar(y, x));
					if (cx == 0 && cy == 0 || cx == 4 && cy == 0 || cx == 1 && cy == 1 || cx == 3 && cy == 1)
						attroff(COLOR_PAIR(EMPTYFIELD_PAIR));
					else
						attroff(COLOR_PAIR(ColorPair[GetChar(y,x)]));
				}
			}
		}
	}
}

int Grid::ClearPuyos()
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

void Grid::DropAllPuyos()
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
