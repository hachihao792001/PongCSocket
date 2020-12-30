#include "Physics.h"
#include <chrono>
#include <thread>
using namespace chrono;
#define FPS 100

MyRectangle topBorder = { Pos{0,0}, WIDTH, 1 };
MyRectangle bottomBorder = { Pos{0,HEIGHT - 1}, WIDTH, 1 };
MyRectangle leftBorder = { Pos{0,0}, 1, HEIGHT };
MyRectangle rightBorder = { Pos{WIDTH - 1,0}, 1, HEIGHT };

Player leftPlayer = { { Pos{DIST_PLAYER_BORDER,0}, PLAYER_THICKNESS, PLAYER_SIZE } };
Player rightPlayer = { { Pos{WIDTH - PLAYER_THICKNESS - DIST_PLAYER_BORDER, 0}, PLAYER_THICKNESS, PLAYER_SIZE } };

Ball theBall = { { Pos{WIDTH / 2 - BALL_SIZE / 2,HEIGHT / 2 - BALL_SIZE / 2}, BALL_SIZE, BALL_SIZE }, {1,1} };

int leftPlayerScore = 0, rightPlayerScore = 0;
bool leftPlayerWin = false, rightPlayerWin = false;

bool leftPlayer_Up, leftPlayer_Down;
bool rightPlayer_Up, rightPlayer_Down;

int InitSocket() {
	if (!AfxWinInit(::GetModuleHandle(NULL), NULL, ::GetCommandLine(), 0))
	{
		// TODO: change error code to suit your needs
		_tprintf(_T("Fatal Error: MFC initialization failed\n"));
		return -1;
	}

	if (AfxSocketInit() == FALSE)
	{
		cout << "Khong the khoi tao Socket Library";
		return FALSE;
	}

	return TRUE;
}

void CalculatePhysics() {
	auto lastTime = high_resolution_clock::now();
	while (true) {
		if ((int)duration_cast<milliseconds>(high_resolution_clock::now() - lastTime).count() >= (1.0 / FPS) * 1000) {

			leftPlayer.UpdatePosition(leftPlayer_Up, leftPlayer_Down);
			rightPlayer.UpdatePosition(rightPlayer_Up, rightPlayer_Down);

			theBall.CheckHitGoal(leftBorder, rightBorder, leftPlayerWin, rightPlayerWin, leftPlayerScore, rightPlayerScore);
			theBall.CheckHitObj(topBorder, bottomBorder, leftPlayer, rightPlayer);

			theBall.velocity.setMagnitude(BALL_SPEED);
			theBall.UpdatePosition();

			if (leftPlayerWin || rightPlayerWin) {
				theBall.CenterPos_RandomDirection(leftPlayerWin, rightPlayerWin);
				theBall.velocity.setMagnitude(BALL_SPEED);
			}

			//cout << rightPlayer.playerRect.topLeftCorner.x << " " << rightPlayer.playerRect.topLeftCorner.y << endl;

			lastTime = high_resolution_clock::now();

			/*system("cls");
			cout << "Ball pos: ";	theBall.ballRect.print();
			cout << "Ball velocity: ";	theBall.velocity.print();
			cout << "Left player pos: ";	leftPlayer.playerRect.print();
			cout << "Right player pos: ";	rightPlayer.playerRect.print();
			cout << "Left player score: " << leftPlayerScore << endl;
			cout << "Right player score: " << rightPlayerScore << endl;*/
		}
	}
}

void HandleClient(SOCKET raw, bool& up, bool& down) {
	CSocket connector;
	connector.Attach(raw);

	auto lastTime = high_resolution_clock::now();
	while (true) {
		if ((int)duration_cast<milliseconds>(high_resolution_clock::now() - lastTime).count() >= (1.0 / FPS) * 1000) {
			const int BUFFER_SIZE = 1;
			byte buffer[1];
			byte received[2];
			int count;
			int i = 0;
			do {
				count = connector.Receive(&buffer, BUFFER_SIZE);
				for (int j = 0; j < count; j++)
					received[i++] = buffer[j];
			} while (i < 2);

			copy(received, received + 1, (char*)&up);
			copy(received + 1, received + 2, (char*)&down);

			byte sendBytes[56];
			copy((char*)&leftPlayer.playerRect.topLeftCorner.x, (char*)&leftPlayer.playerRect.topLeftCorner.x + 4, &sendBytes[0]);
			copy((char*)&leftPlayer.playerRect.topLeftCorner.y, (char*)&leftPlayer.playerRect.topLeftCorner.y + 4, &sendBytes[4]);
			copy((char*)&leftPlayer.playerRect.width, (char*)&leftPlayer.playerRect.width + 4, &sendBytes[8]);
			copy((char*)&leftPlayer.playerRect.height, (char*)&leftPlayer.playerRect.height + 4, &sendBytes[12]);
			copy((char*)&rightPlayer.playerRect.topLeftCorner.x, (char*)&rightPlayer.playerRect.topLeftCorner.x + 4, &sendBytes[16]);
			copy((char*)&rightPlayer.playerRect.topLeftCorner.y, (char*)&rightPlayer.playerRect.topLeftCorner.y + 4, &sendBytes[20]);
			copy((char*)&rightPlayer.playerRect.width, (char*)&rightPlayer.playerRect.width + 4, &sendBytes[24]);
			copy((char*)&rightPlayer.playerRect.height, (char*)&rightPlayer.playerRect.height + 4, &sendBytes[28]);
			copy((char*)&theBall.ballRect.topLeftCorner.x, (char*)&theBall.ballRect.topLeftCorner.x + 4, &sendBytes[32]);
			copy((char*)&theBall.ballRect.topLeftCorner.y, (char*)&theBall.ballRect.topLeftCorner.y + 4, &sendBytes[36]);
			copy((char*)&theBall.ballRect.width, (char*)&theBall.ballRect.width + 4, &sendBytes[40]);
			copy((char*)&theBall.ballRect.height, (char*)&theBall.ballRect.height + 4, &sendBytes[44]);
			copy((char*)&leftPlayerScore, (char*)&leftPlayerScore + 4, &sendBytes[48]);
			copy((char*)&rightPlayerScore, (char*)&rightPlayerScore + 4, &sendBytes[52]);

			connector.Send(sendBytes, 56);
			lastTime = high_resolution_clock::now();
		}
	}
}

int main() {
	srand((unsigned)time(0));
	if (InitSocket() <= 0)
		return -1;

	CSocket serverSocket;
	if (serverSocket.Create(1234, SOCK_STREAM) == 0) {
		cout << "Khong the tao server!" << endl;
		return -1;
	}

	cout << "Tao server thanh cong!" << endl;

	if (!serverSocket.Listen(2)) {
		cout << "Khong the lang nghe tren port nay !!!" << endl;
	}
	else {
		cout << "Dang cho nguoi choi dau tien...\n";
		CSocket leftConnector, rightConnector;
		if (serverSocket.Accept(leftConnector)) {
			cout << "Dang cho nguoi choi thu hai...\n";
			if (serverSocket.Accept(rightConnector)) {

				bool canStart = true;
				leftConnector.Send(&canStart, sizeof(bool));
				rightConnector.Send(&canStart, sizeof(bool));
				cout << "Bat dau choi!\n";

				thread leftClient(HandleClient, leftConnector.Detach(), ref(leftPlayer_Up), ref(leftPlayer_Down));
				thread rightClient(HandleClient, rightConnector.Detach(), ref(rightPlayer_Up), ref(rightPlayer_Down));
				thread physics(CalculatePhysics);
				leftClient.join();
				rightClient.join();
				physics.join();
			}
		}
	}

	serverSocket.Close();
	return 0;
}