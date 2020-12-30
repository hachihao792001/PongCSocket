#pragma once
#define WIDTH 960
#define HEIGHT 600
#define PLAYER_SIZE 100
#define PLAYER_THICKNESS 20
#define PLAYER_MOVESPEED 10
#define BALL_SIZE 12
#define BALL_SPEED 10
#define DIST_PLAYER_BORDER 70
#include <iostream>
#include <cmath>
#include <sstream>
#include "Server.h"
using namespace std;

struct Pos {
	int x, y;
};

struct Vector2D {
	int x, y;
	void setMagnitude(double magnitude) {
		double currentMagnitude = sqrt(x * x + y * y);
		x = (int)((double)x / currentMagnitude * magnitude);
		y = (int)((double)y / currentMagnitude * magnitude);
	}
	void print() {
		cout << x << " " << y << endl;
	}
};

struct MyRectangle {
	Pos topLeftCorner;
	int width, height;

	void print() {
		cout << topLeftCorner.x << " " << topLeftCorner.y << endl;
	}
};


struct Player {
	MyRectangle playerRect;
	MyRectangle TopEdge() {
		return { Pos{playerRect.topLeftCorner.x + 2, playerRect.topLeftCorner.y}, playerRect.width - 4,1 };
	}
	MyRectangle BottomEdge() {
		return { Pos{playerRect.topLeftCorner.x + 2, playerRect.topLeftCorner.y + playerRect.height}, playerRect.width - 4,1 };
	}
	MyRectangle LeftEdge() {
		return { Pos{playerRect.topLeftCorner.x, playerRect.topLeftCorner.y}, 2,playerRect.height };
	}
	MyRectangle RightEdge() {
		return { Pos{playerRect.topLeftCorner.x + playerRect.width - 2, playerRect.topLeftCorner.y}, 2,playerRect.height };
	}

	void UpdatePosition(bool up, bool down) {

		if (up)
		{
			playerRect.topLeftCorner.y -= PLAYER_MOVESPEED;
			if (playerRect.topLeftCorner.y < 0)
				playerRect.topLeftCorner.y = 0;
		}
		if (down) {
			playerRect.topLeftCorner.y += PLAYER_MOVESPEED;
			if (playerRect.topLeftCorner.y > HEIGHT - PLAYER_SIZE)
				playerRect.topLeftCorner.y = HEIGHT - PLAYER_SIZE;
		}
	}
};

bool CheckCollide(MyRectangle rec1, MyRectangle rec2);

struct Ball {
	MyRectangle ballRect;
	Vector2D velocity;
	void UpdatePosition() {
		ballRect.topLeftCorner.x += velocity.x;
		ballRect.topLeftCorner.y += velocity.y;
	}

	void CenterPos_RandomDirection(bool& leftPlayerWin, bool& rightPlayerWin) {
		ballRect.topLeftCorner = { WIDTH / 2 - BALL_SIZE / 2,HEIGHT / 2 - BALL_SIZE / 2 };
		if (leftPlayerWin) {
			velocity.x = 10 + rand() % 10;
			int up = rand() % 2;
			if (up == 0)
				velocity.y = 2 + rand() % 3;
			else
				velocity.y = -2 - rand() % 3;
			leftPlayerWin = false;
		}
		else {
			velocity = { -10 + rand() % 10, rand() % 5 };
			int up = rand() % 2;
			if (up == 0)
				velocity.y = 2 + rand() % 3;
			else
				velocity.y = -2 - rand() % 3;
			rightPlayerWin = false;
		}
	}

	void CheckHitObj(MyRectangle topBorder, MyRectangle bottomBorder, Player leftPlayer, Player rightPlayer) {
		if (CheckCollide(ballRect, topBorder) || CheckCollide(ballRect, bottomBorder))
			velocity.y = -velocity.y;

		if (CheckCollide(ballRect, leftPlayer.RightEdge()))
			velocity.x = abs(velocity.x);
		else if (CheckCollide(ballRect, leftPlayer.LeftEdge()))
			velocity.x = -abs(velocity.x);
		else if (CheckCollide(ballRect, leftPlayer.TopEdge()))
			velocity.y = -abs(velocity.y);
		else if (CheckCollide(ballRect, leftPlayer.BottomEdge()))
			velocity.y = abs(velocity.y);

		if (CheckCollide(ballRect, rightPlayer.RightEdge()))
			velocity.x = abs(velocity.x);
		else if (CheckCollide(ballRect, rightPlayer.LeftEdge()))
			velocity.x = -abs(velocity.x);
		else if (CheckCollide(ballRect, rightPlayer.TopEdge()))
			velocity.y = -abs(velocity.y);
		else if (CheckCollide(ballRect, rightPlayer.BottomEdge()))
			velocity.y = abs(velocity.y);
	}

	void CheckHitGoal(MyRectangle leftBorder, MyRectangle rightBorder, bool& leftPlayerWin, bool& rightPlayerWin, int& leftPlayerScore, int& rightPlayerScore) {
		if (CheckCollide(ballRect, leftBorder)) {
			rightPlayerWin = true;
			rightPlayerScore++;
		}
		if (CheckCollide(ballRect, rightBorder)) {
			leftPlayerWin = true;
			leftPlayerScore++;
		}
	}
};