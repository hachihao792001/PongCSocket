#include "DrawStuff.h"
HDC hdc = GetDC(GetConsoleWindow());

string intToString(int i) {
	stringstream ss;
	ss << i;
	return ss.str();
}

void InitWindow() {
	system("mode 120, 50");
	ClearScreen();
}

void ClearScreen() {
	DrawRectangle(MyRectangle{ Pos{0,0}, WIDTH, HEIGHT }, BLACK);
}

void DrawRectangle(MyRectangle rec, COLORREF color) {
	//copy from stackoverflow
	RECT rect = { rec.topLeftCorner.x, rec.topLeftCorner.y, rec.topLeftCorner.x + rec.width, rec.topLeftCorner.y + rec.height };
	HBRUSH currentBrush = CreateSolidBrush(color);
	HBRUSH hTmp = (HBRUSH)SelectObject(hdc, currentBrush);
	FillRect(hdc, &rect, currentBrush);
	DeleteObject(SelectObject(hdc, hTmp));
}

std::wstring s2ws(const std::string& s)
{
	//copy from stackoverflow
	int len;
	int slength = (int)s.length() + 1;
	len = MultiByteToWideChar(CP_ACP, 0, s.c_str(), slength, 0, 0);
	wchar_t* buf = new wchar_t[len];
	MultiByteToWideChar(CP_ACP, 0, s.c_str(), slength, buf, len);
	std::wstring r(buf);
	delete[] buf;
	return r;
}

void DrawCustomText(string messageStr, int x, int y) {
	//copy from stackoverflow
	std::wstring stemp = s2ws(messageStr);
	LPCWSTR message = stemp.c_str();
	int len = messageStr.length();

	SetTextColor(hdc, WHITE);
	HFONT hFont = CreateFont(50, 0, 0, 0, FW_NORMAL, 0, 0, 0, 0, 0, 0, 2, 0, L"SYSTEM_FIXED_FONT");
	HFONT hTmp = (HFONT)SelectObject(hdc, hFont);
	SelectObject(hdc, hFont);
	SetBkMode(hdc, TRANSPARENT);
	TextOut(hdc, x, y, message, len);
	DeleteObject(SelectObject(hdc, hTmp));
}