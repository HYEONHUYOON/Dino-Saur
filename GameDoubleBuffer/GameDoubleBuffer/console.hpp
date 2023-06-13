#pragma once

#include <Windows.h>

#ifndef __COLOR_LIST_
#define __COLOR_LIST_

enum {
	black,
	blue,
	green,
	cyan,
	red,
	purple,
	brown,
	lightgray,
	darkgray,
	lightblue,
	lightgreen,
	lightcyan,
	lightred,
	lightpurple,
	yellow,
	white
};

#endif 

//좌표 이동
void gotoxy(int x, int y){
	HANDLE consoleHandle = GetStdHandle(STD_OUTPUT_HANDLE);
	COORD pos;
	pos.X = x;
	pos.Y = y;
	SetConsoleCursorPosition(consoleHandle, pos);
}

//글자, 배경
void setColor(int forground, int background) {
	HANDLE consoleHandle = GetStdHandle(STD_OUTPUT_HANDLE);
	int code = forground + background * 16;
	SetConsoleTextAttribute(consoleHandle, code);
}

//좌표 복사
void CoordCopy(COORD* coord1, COORD* coord2) {
	coord1->X = coord2->X;
	coord1->Y = coord2->Y;
}

//좌표 비교
bool CoordCompare(COORD coord1, COORD coord2) {
	if ((coord1.X == coord2.X) && (coord1.Y == coord2.Y))
	{
		return true;
	}
	return false;
}
