#pragma once
//#define CMD_CHAR_WIDTH 8
//#define CMD_CHAR_HEIGHT 12
//#define COL 120
//#define ROW 50
#define WIDTH 960
#define HEIGHT 600
#include <iostream>
#include <cmath>
#include <sstream>
#include "Client.h"
using namespace std;
const COLORREF BLACK = RGB(0, 0, 0);
const COLORREF WHITE = RGB(255, 255, 255);

struct Pos {
	int x, y;
};

struct Vector2D {
	int x, y;
	double getMagnitude() {
		return sqrt(x * x + y * y);
	}
	void setMagnitude(double magnitude) {
		double currentMagnitude = getMagnitude();
		x = (int)((double)x / currentMagnitude * magnitude);
		y = (int)((double)y / currentMagnitude * magnitude);
	}
};

struct MyRectangle {
	Pos topLeftCorner;
	int width, height;
};

string intToString(int i);

void InitWindow();

void ClearScreen();

void DrawRectangle(MyRectangle rec, COLORREF color);

bool CheckCollide(MyRectangle rec1, MyRectangle rec2);

void DrawCustomText(string messageStr, int x, int y);