#include "DrawStuff.h"
#include <chrono>
#include <thread>
using namespace chrono;
#define FPS 100

MyRectangle leftPlayer;
MyRectangle rightPlayer;

MyRectangle theBall;

int leftPlayerScore = 0, rightPlayerScore = 0;

void Render() {
	auto lastTime = high_resolution_clock::now();
	while (true) {
		if ((int)duration_cast<milliseconds>(high_resolution_clock::now() - lastTime).count() >= (1.0 / FPS) * 1000) {
			ClearScreen();
			DrawRectangle(leftPlayer, WHITE);
			DrawRectangle(rightPlayer, WHITE);
			DrawRectangle(theBall, WHITE);
			DrawCustomText(intToString(leftPlayerScore), WIDTH / 2 - 100, 30);
			DrawCustomText(intToString(rightPlayerScore), WIDTH / 2 + 100, 30);
			lastTime = high_resolution_clock::now();
		}
	}
}

void CommunicateWithServer(SOCKET raw) {
	CSocket clientSocket;
	clientSocket.Attach(raw);

	auto lastTime = high_resolution_clock::now();
	while (true) {
		if ((int)duration_cast<milliseconds>(high_resolution_clock::now() - lastTime).count() >= (1.0 / FPS) * 1000) {

			bool thisWindowIsFocused = GetConsoleWindow() == GetForegroundWindow();
			bool up = GetKeyState(VK_UP) < 0 && thisWindowIsFocused;
			bool down = GetKeyState(VK_DOWN) < 0 && thisWindowIsFocused;

			byte sendBytes[2];
			copy((char*)&up, (char*)&up + 1, sendBytes);
			copy((char*)&down, (char*)&down + 1, sendBytes + 1);
			clientSocket.Send(sendBytes, 2);

			const int BUFFER_SIZE = 16;
			byte buffer[16];
			byte received[56];
			int count;
			int i = 0;
			Sleep(1);
			do {
				count = clientSocket.Receive(buffer, BUFFER_SIZE);
				for (int j = 0; j < count; j++)
					received[i++] = buffer[j];
			} while (i < 56);

			int receivedInts[14];
			for (int j = 0; j < 14; j++)
				copy(received + j * 4, received + j * 4 + 4, (char*)(receivedInts + j));

			leftPlayer.topLeftCorner.x = receivedInts[0];
			leftPlayer.topLeftCorner.y = receivedInts[1];
			leftPlayer.width = receivedInts[2];
			leftPlayer.height = receivedInts[3];
			rightPlayer.topLeftCorner.x = receivedInts[4];
			rightPlayer.topLeftCorner.y = receivedInts[5];
			rightPlayer.width = receivedInts[6];
			rightPlayer.height = receivedInts[7];
			theBall.topLeftCorner.x = receivedInts[8];
			theBall.topLeftCorner.y = receivedInts[9];
			theBall.width = receivedInts[10];
			theBall.height = receivedInts[11];
			leftPlayerScore = receivedInts[12];
			rightPlayerScore = receivedInts[13];
			lastTime = high_resolution_clock::now();
		}
	}
}

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

int main() {
	srand((unsigned)time(0));
	if (InitSocket() <= 0)
		return -1;

	CSocket clientSocket;
	clientSocket.Create();
	char IP[16];
	cout << "Nhap dia chi IP cua server: ";
	cin >> IP;
	if (clientSocket.Connect(CA2W(IP), 1234) != 0) {
		cout << "Ket noi server thanh cong!\n";
		cout << "Dang cho nguoi choi thu hai...";
		bool canStart;
		clientSocket.Receive(&canStart, sizeof(bool), 0);
		InitWindow();

		thread render(Render);
		thread server(CommunicateWithServer, clientSocket.Detach());
		render.join();
		server.join();
	}
	clientSocket.Close();
	return 0;
}