#pragma once

/* 버퍼 초기화 함수 */
int clearBuffer(char* screenBuf, int width, int height);

/* 수업시간에 배운 x,y에 스트링을 쓰는 함수를 짜면 됨. */
int writeStringToBuffer(const char* string, int x, int y);

/* 타이틀 화면 그리는 버퍼랑 게임오버, 게임설명 등등 이런식으로 만들면 보기 편하겠죠? */
int setTitleToScreenBuffer(char* screenBuf, int width, int height);

int setGameOverToScreenBuffer(char* screenBuf, int width, int height);