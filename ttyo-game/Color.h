#pragma once

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

constexpr char EMPTYGRID = '*';

constexpr int EMPTYFIELD_PAIR = 1;
constexpr int REDFIELD_PAIR = 2;
constexpr int BLUEFIELD_PAIR = 3;
constexpr int YELLOWFIELD_PAIR = 4;
constexpr int GREENFIELD_PAIR = 5;
constexpr int PURPLEFIELD_PAIR = 6;
constexpr int OUTLINEFIELD_PAIR = 7;
constexpr int DEATHFIELD_PAIR = 8;

enum Color { red, blue, yellow, green, purple };

static map<Color, char> ctoch = {{Color::red, 'R'}, {Color::blue, 'B'}, {Color::yellow, 'Y'}, {Color::green, 'G'}, {Color::purple, 'P'}};
