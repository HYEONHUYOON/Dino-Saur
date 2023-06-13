#pragma once
#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include<string>
#include <conio.h>
#include <cstdlib>
#include <iostream>
#include <algorithm>
#include <Windows.h>
#include <vector>
#include <fstream>

#include "console.hpp"

using namespace std;

class GameObject {
public:
	int x, y;
	bool active = false;

	GameObject(int x, int y) {}

	void Collider() {}

	~GameObject() {}
};

class Player : public GameObject {
public:
	float gravity;
	int jumpPower = 12;

	//좌표
	COORD recentCoord[8];
	COORD prevCoord[8];

	int floorY;

	bool isFloor = true;
	bool isJump = false;

	//시간 관련 변수 초기화
	double speedtimer = 0;
	double deltaTime = 0.0;
	double steptimer = 0.0;
	chrono::steady_clock::time_point currentTime;
	chrono::steady_clock::time_point previousTime;

	Player(int x, int y) : GameObject(x, y) {
		//속도 시간 재기
		chrono::steady_clock::time_point;
		previousTime = currentTime;

		this->x = x;
		this->y = y;
		this->floorY = y;

		initCOORD();
	}

	void initCOORD() {
		//좌표
		//발
		recentCoord[0] = { 12,(short)(y - 2) };
		//꼬리
		recentCoord[1] = { 10,(short)(y - 3) };
		//몸통
		recentCoord[2] = { 11,(short)(y - 3) };
		recentCoord[3] = { 12,(short)(y - 3) };
		//팔
		recentCoord[4] = { 13,(short)(y - 3) };
		//목
		recentCoord[5] = { 12,(short)(y - 4) };
		//머리
		recentCoord[6] = { 12,(short)(y - 5) };
		recentCoord[7] = { 13,(short)(y - 5) };
	}

	//점프 버그 수정 할것
	void Update(int input) {
		// 시간 간격 계산		
		currentTime = chrono::steady_clock::now();
		chrono::duration<double> elapsedTime = currentTime - previousTime;
		previousTime = currentTime;
		deltaTime = elapsedTime.count();
		speedtimer += deltaTime;
		steptimer += deltaTime;

		if (input == 32) {
			Jump();
		}

		//최대점프 높이에 올라옴
		if (recentCoord[0].Y == y - jumpPower) {
			isJump = false;
			isFloor = false;
		}

		//점프 중
		if (isJump) {
			if (speedtimer > 0.032) {
				for (int i = 0; i < 8; i++) {
					recentCoord[i].Y -= 1;
				}
				speedtimer = 0;
			}
		}

		//점프 한후 공중에 떠있으면
		if (!isFloor && recentCoord[0].Y <  y) {
			if (speedtimer > 0.032) {
				Gravity();
				speedtimer = 0;
			}
		}

		//바닥에 붙어있으면
		if (recentCoord[0].Y >= y - 2) {
			isFloor = true;
		}

		//만약 바닥 아래로 빠져나갔을 경우 위치 이니셜라이즈
		if (recentCoord[0].Y >= y-1) {
			initCOORD();
		}

		//발바꿈
		if (!isJump&& isFloor) {
			if (steptimer > 0.2) {
				recentCoord[0] = { 11,(short)(y - 2) };
				steptimer = 0;
			}
			else {
				recentCoord[0] = { 12,(short)(y - 2) };
			}
		}
	}

	void Render(vector<vector<string>> &screenBuf) {
		//그리기
		for (int i = 0; i < 8; i++) {
			screenBuf[recentCoord[i].Y][recentCoord[i].X] = "■";
		}
	}

	void Gravity() {
		for (int i = 0; i < 8; i++) {
			recentCoord[i].Y += 1;
		}
	}

	void Jump() {
		if (isFloor) {
			isJump = true;
		}
	}

	~Player() {}
};

//고정 장애물
class Obstacle : public GameObject {
public:
	//좌표
	COORD recentCoord[7];
	COORD ColliderCoord[13];

	//시간 관련 변수 초기화
	double speedtimer = 0;
	double deltaTime = 0.0;
	chrono::steady_clock::time_point currentTime;
	chrono::steady_clock::time_point previousTime;

	void initCOORD() {
		//좌표
		recentCoord[0] = { (short)(x + 3),(short)(y - 2) };
		recentCoord[1] = { (short)(x + 4),(short)(y - 3) };
		recentCoord[2] = { (short)(x + 3),(short)(y - 3) };
		recentCoord[3] = { (short)(x + 2),(short)(y - 3) };
		recentCoord[4] = { (short)(x + 4),(short)(y - 4) };
		recentCoord[5] = { (short)(x + 2),(short)(y - 4) };
		recentCoord[6] = { (short)(x + 4),(short)(y - 5) };
	}

	Obstacle(int x, int y) : GameObject(x, y) {
		//속도 시간 재기
		chrono::steady_clock::time_point;
		previousTime = currentTime;

		this->x = x;
		this->y = y;

		initCOORD();
	}

	char Update(double speed, Player* p) {
		// 시간 간격 계산		
		currentTime = chrono::steady_clock::now();
		chrono::duration<double> elapsedTime = currentTime - previousTime;
		previousTime = currentTime;
		deltaTime = elapsedTime.count();
		speedtimer += deltaTime;

		//맵에서 벗어나면
		if (recentCoord[3].X < 0) {
			active = false;
			initCOORD();
		}

		//이동
		if (speedtimer > speed) {
			for (int i = 0; i < 7; i++) {
				// 속도를 점수에 따라 조절하여 좌표를 왼쪽으로 이동
				recentCoord[i].X -= 2;
			}
			speedtimer = 0;
		}

		//충돌 감지
		for (int i = 0; i < 7; i++) {
			for (int j = 0; j < 8; j++) {
				if (CoordCompare(p->recentCoord[j], recentCoord[i])) {
					return 0x05;
				}
			}
		}

		return NULL;
	}

	void setActive() {
		active = active ? false : true;
	}

	void Render(vector<vector<string>> &screenBuf) {
		//그리기
		for (int i = 0; i < 7; i++) {
			if (recentCoord[i].X > 0 && recentCoord[i].X < x - 1) {
				screenBuf[recentCoord[i].Y][recentCoord[i].X] = "■";
			}
		}
	}

	~Obstacle() {}
};

//움직이는 장애물
class Obstacle2 : public GameObject {
public:
	//좌표
	COORD recentCoord[5];

	COORD ColliderCoord[13];

	//시간 관련 변수 초기화
	double speedtimer = 0;
	double deltaTime = 0.0;
	chrono::steady_clock::time_point currentTime;
	chrono::steady_clock::time_point previousTime;

	void initCOORD() {
		//좌표
		recentCoord[0] = { (short)(x + 3),(short)(0 + 5) };
		recentCoord[1] = { (short)(x + 4),(short)(0 + 5) };
		//머리
		recentCoord[2] = { (short)(x + 3),(short)(0 + 4) };
		recentCoord[3] = { (short)(x + 2),(short)(0 + 4) };
		recentCoord[4] = { (short)(x + 4),(short)(0 + 3) };
	}

	Obstacle2(int x, int y) : GameObject(x, y) {
		//속도 시간 재기
		chrono::steady_clock::time_point;
		previousTime = currentTime;

		this->x = x;
		this->y = y;

		initCOORD();
	}

	char Update(double speed, Player* p) {
		// 시간 간격 계산		
		currentTime = chrono::steady_clock::now();
		chrono::duration<double> elapsedTime = currentTime - previousTime;
		previousTime = currentTime;
		deltaTime = elapsedTime.count();
		speedtimer += deltaTime;

		//맵에서 벗어나면
		if (recentCoord[1].X < 0) {
			active = false;
			initCOORD();
		}

		//이동
		if (speedtimer > speed) {
			for (int i = 0; i < 5; i++) {
				// 속도를 점수에 따라 조절하여 좌표를 왼쪽으로 이동

				if (recentCoord[1].X < x / 2  && recentCoord[0].Y < y-2) {
					recentCoord[i].Y += 1;
				}

				recentCoord[i].X -= 2;
			}
			speedtimer = 0;
		}

		//충돌 감지
		for (int i = 0; i < 5; i++) {
			for (int j = 0; j < 8; j++) {
				if (CoordCompare(p->recentCoord[j], recentCoord[i])) {
					return 0x05;
				}
			}
		}
		return NULL;
	}

	void setActive() {
		active = active ? false : true;
	}

	void Render(vector<vector<string>>& screenBuf) {
		//그리기
		for (int i = 0; i < 7; i++) {
			if (recentCoord[i].X > 0 && recentCoord[i].X < x - 1) {
				screenBuf[recentCoord[i].Y][recentCoord[i].X] = "■";
			}
		}
	}

	~Obstacle2() {}
};

class Ground : public GameObject{
public :
	COORD recentCoord[8];
	
	Ground(int x, int y) : GameObject(x, y) {
		this->x = x;
		this->y = y;
	}

	void Render(vector<vector<string>>& screenBuf) {
		bool ischanged = false;

		//그리기
		for (int i = 0; i < x; i++) {
			screenBuf[y-1][i] = "─";
		}
	}

	~Ground(){}
};

class BackGround {
public:
	int x;
	int y;
	int speed;
	BackGround(int x, int y) {}

	void Render() {}
	~BackGround() {}
};

class Cloud : public BackGround {
public:
	//좌표
	COORD recentCoord[11];
	COORD prevCoord[11];

	bool active = false;
	int position = 0;

	//시간 관련 변수 초기화
	double speedtimer = 0;
	double deltaTime = 0.0;
	chrono::steady_clock::time_point currentTime;
	chrono::steady_clock::time_point previousTime;

	void initCOORD() {
		//좌표
		recentCoord[0] = { (short)(x +4),(short)(3 + position) };
		recentCoord[1] = { (short)(x +3),(short)(3 + position) };
		recentCoord[2] = { (short)(x +2),(short)(3 + position) };
		recentCoord[3] = { (short)(x +1),(short)(3 + position) };
		recentCoord[4] = { (short)(x),(short)(3 + position) };
		recentCoord[5] = { (short)(x+4),(short)(2 + position) };
		recentCoord[6] = { (short)(x+3),(short)(2 + position) };
		recentCoord[7] = { (short)(x+2),(short)(2 + position) };
		recentCoord[8] = { (short)(x+1),(short)(2 + position) };
		recentCoord[9] = { (short)(x+3),(short)(1 + position) };
		recentCoord[10] = { (short)(x+2),(short)(1 + position) };
	}

	Cloud(int x, int y) : BackGround(x, y) {
		chrono::steady_clock::time_point;
		previousTime = currentTime;

		this->x = x;
		this->y = y;

		initCOORD();
	}

	void Update(double speed) {
		// 시간 간격 계산
		currentTime = chrono::steady_clock::now();
		chrono::duration<double> elapsedTime = currentTime - previousTime;
		previousTime = currentTime;
		deltaTime = elapsedTime.count();
		speedtimer += deltaTime;

		if (recentCoord[0].X < 0) {
			active = false;
			initCOORD();
		}

		if (speedtimer > speed) {
			// 좌표 이동
			for (int i = 0; i < 11; i++) {
				recentCoord[i].X -= 1; // 속도를 점수에 따라 조절하여 좌표를 왼쪽으로 이동
			}
			speedtimer = 0;
		}
	}

	void setActive() {
		srand((unsigned int)time(NULL));
		position = (rand() % 5);

		initCOORD();
		active = active ? false : true;
	}

	void Render(vector<vector<string>> &screenBuf, vector<vector<int>>& screenBufColor) {
		//그리기
		for (int i = 0; i < 11; i++) {
			//아무것도 없을 때 찍음
				if (recentCoord[i].X > 0 && recentCoord[i].X < x - 1) {
					if (screenBuf[recentCoord[i].Y][recentCoord[i].X] == " ") {
						screenBufColor[recentCoord[i].Y][recentCoord[i].X] = blue;
					screenBuf[recentCoord[i].Y][recentCoord[i].X] = "■";
				}
			}
		}
	}

	~Cloud() {}
};

class Mountain : public BackGround {
public:
public:
	//좌표
	COORD recentCoord[25];
	COORD prevCoord[25];

	bool active = false;
	int position = 0;

	//시간 관련 변수 초기화
	double speedtimer = 0;
	double deltaTime = 0.0;
	chrono::steady_clock::time_point currentTime;
	chrono::steady_clock::time_point previousTime;

	void initCOORD() {
		//좌표
		recentCoord[0] = { (short)(x + 9),(short)(y - 2) };
		recentCoord[1] = { (short)(x + 8),(short)(y - 2) };
		recentCoord[2] = { (short)(x + 7),(short)(y - 2) };
		recentCoord[3] = { (short)(x + 6),(short)(y - 2) };
		recentCoord[4] = { (short)(x + 5),(short)(y - 2) };
		recentCoord[5] = { (short)(x + 4),(short)(y - 2) };
		recentCoord[6] = { (short)(x + 3),(short)(y - 2) };
		recentCoord[7] = { (short)(x + 2),(short)(y - 2) };
		recentCoord[8] = { (short)(x + 1),(short)(y - 2) };
		recentCoord[9] = { (short)(x + 8),(short)(y - 3) };
		recentCoord[10] = { (short)(x + 7),(short)(y - 3) };
		recentCoord[11] = { (short)(x + 6),(short)(y - 3) };
		recentCoord[12] = { (short)(x + 5),(short)(y - 3) };
		recentCoord[13] = { (short)(x + 4),(short)(y - 3) };
		recentCoord[14] = { (short)(x + 3),(short)(y - 3) };
		recentCoord[15] = { (short)(x + 2),(short)(y - 3) };
		recentCoord[16] = { (short)(x + 7),(short)(y - 4) };
		recentCoord[17] = { (short)(x + 6),(short)(y - 4) };
		recentCoord[18] = { (short)(x + 5),(short)(y - 4) };
		recentCoord[19] = { (short)(x + 4),(short)(y - 4) };
		recentCoord[20] = { (short)(x + 3),(short)(y - 4) };
		recentCoord[21] = { (short)(x + 6),(short)(y - 5) };
		recentCoord[22] = { (short)(x + 5),(short)(y - 5) };
		recentCoord[23] = { (short)(x + 4),(short)(y - 5) };
		recentCoord[24] = { (short)(x + 5),(short)(y - 6) };
	}

	Mountain(int x, int y) : BackGround(x, y) {
		chrono::steady_clock::time_point;
		previousTime = currentTime;

		this->x = x;
		this->y = y;

		initCOORD();
	}

	void Update(double speed) {
		// 시간 간격 계산
		currentTime = chrono::steady_clock::now();
		chrono::duration<double> elapsedTime = currentTime - previousTime;
		previousTime = currentTime;
		deltaTime = elapsedTime.count();
		speedtimer += deltaTime;

		//끝까지 전부 벗어났을 때
		if (recentCoord[0].X <= 0) {
			active = false;
			initCOORD();
		}

		if (speedtimer > speed) {
			//좌표 이동
			for (int i = 0; i < 25; i++) {
				recentCoord[i].X--; // X 좌표를 왼쪽으로 이동
			}
			speedtimer = 0;
		}
	}

	void setActive() {
		active = active ? false : true;
	}

	void Render(vector<vector<string>>& screenBuf, vector<vector<int>>& screenBufColor) {
		//그리기
		for (int i = 0; i < 25; i++) {
			//아무것도 없을 때 찍음
			if (recentCoord[i].X > 0 && recentCoord[i].X < x - 1) {
				if (screenBuf[recentCoord[i].Y][recentCoord[i].X] == " ") {
					screenBufColor[recentCoord[i].Y][recentCoord[i].X] = green;
					screenBuf[recentCoord[i].Y][recentCoord[i].X] = "■";
				}
			}
		}
	}

	~Mountain() {}
};

class Point : BackGround {
public:
	int bestPoint = 0;
	int thisGamePoint = 0;

	COORD pointVector[5];
	COORD bestPointvector[5];

	char insertbestPoint[5];
	char insertPoint[5];

	Point(int x, int y) : BackGround(x, y) {	
		//좌표
		pointVector[0] = { (short)(x - 2),0};
		pointVector[1] = { (short)(x - 3),0};
		pointVector[2] = { (short)(x - 4),0};
		pointVector[3] = { (short)(x - 5),0};
		pointVector[4] = { (short)(x - 6),0};

		bestPointvector[0] = { (short)(x - 8),0 };
		bestPointvector[1] = { (short)(x - 9),0 };
		bestPointvector[2] = { (short)(x - 10),0 };
		bestPointvector[3] = { (short)(x - 11),0 };
		bestPointvector[4] = { (short)(x - 12),0 };

		for (int i = 0; i < 5; i++)
		{
			insertPoint[i] = '0';
			insertbestPoint[i] = '0';
		}
	}

	void PointInit() {
		//최고 점수 갱신
		if (bestPoint < thisGamePoint) {
			bestPoint = thisGamePoint;
			thisGamePoint = 0;

			insertbestPoint[4] = (char)((bestPoint / 10000) % 10 + 48);
			insertbestPoint[3] = (char)((bestPoint / 1000) % 10 + 48);
			insertbestPoint[2] = (char)((bestPoint / 100) % 10 + 48);
			insertbestPoint[1] = (char)((bestPoint / 10) % 10 + 48);
			insertbestPoint[0] = (char)((bestPoint % 10) + 48);
		}

		//파일 작성
	}

	void Update(double gamePoint) {
		thisGamePoint = (int)gamePoint;

		insertPoint[4] = (char)((thisGamePoint / 10000)%10 + 48);
		insertPoint[3] = (char)((thisGamePoint / 1000) % 10 + 48);
		insertPoint[2] = (char)((thisGamePoint / 100) % 10 + 48);
		insertPoint[1] = (char)((thisGamePoint / 10) % 10 + 48);
		insertPoint[0] = (char)((thisGamePoint % 10) + 48);
	}

	void Render(vector<vector<string>> &screenBuf) {
		for (int i = 0; i < 5; i++)
		{
			if (bestPoint > 0) {
				screenBuf[bestPointvector[i].Y][bestPointvector[i].X] = insertbestPoint[i];
			}
			screenBuf[pointVector[i].Y][pointVector[i].X] = insertPoint[i];
		}
	}
	~Point() {}
};

struct PlayerInform {
	string name;
	int score;

	PlayerInform(string name, int score) : name(name), score(score)
	{}
};

bool comparePlayers(const PlayerInform& p1, const PlayerInform& p2) {
	return p1.score > p2.score;  // 내림차순으로 정렬
}

class Ui {
public:
	int width;
	int height;

	string gameStartM = "Game Start";
	string GameOverM = "G A M E O V E R";
	string RestartM = "Re Start";
	string GamePauseM = "Pause";
	string exitGame = "Exit Game";
	string nameInput = "Input Name";
	string rankSee = "Rank★";
	string rank = "R A N K I N G";
	char name[3] = {'A','A','A'};

	int select = 1;

	int selectNameCnt = 0;
	int nameInputKey = 65;

	Ui(int width,int height) {
		this->width = width;
		this->height = height;
	}

	char Start(vector<vector<string>>& screenBuf,int input) {
		if (input == 72) {
			select -= 1;
			if (select < 1)
				select = 3;
		}
		else if (input == 80) {
			select += 1;
			if (select > 3)
				select = 1;
		}
		else if (input == 32) {
			if (select == 1) {
				select = 1;
				return 0x02;
			}
			else if (select == 2) {
				select = 1;
				return 0x03;
			}
			else if (select == 3) {
				select = 1;
				return 0x01;
			}
		}
		//게임 시작
		for (int i = 0; i < gameStartM.length(); i++) {
			screenBuf[(height / 2 - 3)][(width / 2) - (gameStartM.length() / 2) + i] = gameStartM[i];
		}

		//랭킹
		for (int i = 0; i < rankSee.length(); i++) {
			screenBuf[(height / 2 - 1)][(width / 2) - (rankSee.length() / 2) + i] = rankSee[i];
		}

		//종료
		for (int i = 0; i < exitGame.length(); i++) {
			screenBuf[(height / 2 + 1)][(width / 2) - (exitGame.length() / 2) + i] = exitGame[i];
		}

		if (select == 1) {
			screenBuf[(height / 2 - 3)][((width / 2) - (RestartM.length() / 2)) - 2] = "▷";
		}
		else if (select == 2) {
			screenBuf[(height / 2 -1)][((width / 2) - (exitGame.length() / 2)) - 2] = "▷";
		}
		else if (select == 3) {
			screenBuf[(height / 2 + 1)][((width / 2) - (exitGame.length() / 2)) - 2] = "▷";
		}

		return NULL;
	}

	char GameOver(vector<vector<string>>& screenBuf,int input) {
		if (input == 72) {
			select -= 1;
			if (select < 1)
				select = 3;
		}
		else if (input == 80) {
			select += 1;
			if (select > 3)
				select = 1;
		}
		else if (input == 32) {
			if (select == 1) {
				select = 1;
				return 0x02;
			}
			else if(select == 2 ){
				select = 1;
				return 0x03;
			}
			else if (select == 3) {
				select = 1;
				return 0x01;
			}
		}
			
		//게임오버
		for (int i = 0; i < GameOverM.length(); i++) {
			screenBuf[(height / 2 -3)][(width / 2) - (GameOverM.length() / 2) + i] = GameOverM[i];
		}

		//재시작
		for (int i = 0; i < RestartM.length(); i++) {
			screenBuf[(height / 2 -1)][(width / 2) - (RestartM.length() / 2) + i] = RestartM[i];
		}

		//랭킹
		for (int i = 0; i < rankSee.length(); i++) {
			screenBuf[(height / 2 + 1)][(width / 2) - (rankSee.length() / 2) + i] = rankSee[i];
		}

		//종료
		for (int i = 0; i < exitGame.length(); i++) {
			screenBuf[(height / 2 +3)][(width / 2) - (exitGame.length() / 2) + i] = exitGame[i];
		}

		if (select == 1) {
			screenBuf[(height / 2 -1)][((width / 2) - (RestartM.length() / 2)) - 2] = "▷";
		}
		else if(select == 2) {
			screenBuf[(height / 2 + 1)][((width / 2) - (exitGame.length() / 2)) - 2] = "▷";
		}
		else if (select == 3) {
			screenBuf[(height / 2 + 3)][((width / 2) - (exitGame.length() / 2)) - 2] = "▷";
		}

		return NULL;
	}

	char Pause(vector<vector<string>>& screenBuf,int input) {
		if (input == 27) {
			return 0x02;
		}
		for (int i = 0; i < GamePauseM.length(); i++) {
			screenBuf[(height / 2 - 1)][(width / 2) - (GamePauseM.length() / 2) + i] = GamePauseM[i];
		}
		return NULL;
	}

	bool Rank(vector<vector<string>>& screenBuf, int input,int score) {

		//다시 초기화
		if (selectNameCnt > 3) {
			selectNameCnt = 0;
		}

		//입력 처리
		if (input == 72) {
			nameInputKey++;
			if (nameInputKey > 90) {
				nameInputKey = 65;
			}
		}
		else if (input == 80)
		{
			nameInputKey--;
			if (nameInputKey < 65) {
				nameInputKey = 90;
			}
		}
		else if (input == 32) {
			selectNameCnt++;
		}
		
		if (selectNameCnt > 2) {
			ofstream RankFile;

			try {
				RankFile.open("Rank.txt", ios::out | ios::app);

				for (int i = 0; i < sizeof(name); i++) {
					RankFile << name[i];
				}

				RankFile <<" " << score << "\t\n";

				RankFile.close();
			}
			catch (exception e) {
				cout << "파일이 존재 하지 않습니다." << endl;
			}

			selectNameCnt++;
			return true;
		}

		name[selectNameCnt] = nameInputKey;

		//게임오버
		for (int i = 0; i < GameOverM.length(); i++) {
			screenBuf[(height / 2 - 3)][(width / 2) - (GameOverM.length() / 2) + i] = GameOverM[i];
		}

		for (int i = 0; i < nameInput.length(); i++) {
			screenBuf[(height / 2 )][(width / 2) - (nameInput.length() / 2) + i] = nameInput[i];
		}

		for (int i = 0; i < sizeof(name); i++) {
			screenBuf[(height / 2 + 2)][(width / 2) - (sizeof(name)) + (i*2)] = name[i];
		} 

		screenBuf[(height / 2 +1)][(width / 2) - (sizeof(name)) + selectNameCnt * 2] = "▲";
		screenBuf[(height / 2 +3)][(width / 2) - (sizeof(name)) + selectNameCnt * 2] = "▼";

		return false;
	}

	char RankSee(vector<vector<string>>& screenBuf, int input) {
		if (input == 27) {
			return 0x06;
		}
		vector<PlayerInform> inform;

		int informCnt = 0;
		int score;
		char name[4];
		errno_t err;
		FILE* RankFile;

		err = fopen_s(&RankFile, "Rank.txt", "r");

		if (err != 0)
		{
			printf("파일을 열 수 없습니다.\n");
			return 1;
		}
		while (!feof(RankFile))
		{
			fscanf_s(RankFile, "%s %d",name,4,&score);

			string str(name);
			inform.push_back(PlayerInform(str, score));
			informCnt++;
		}

		//랭킹 10위까지만 
		if (informCnt > 10)
			informCnt = 10;

		fclose(RankFile);

		//정렬
		sort(inform.begin(), inform.end(), comparePlayers);

		//rank 타이틀
		for (int i = 0; i < rank.length(); i++) {
			screenBuf[(height / 2) - (10 / 2)  - 2][(width / 2) - (rank.length() / 2) + i] = rank[i];
		}

		//점수 인덱스
		for (int i = 0; i < informCnt; i++) {
			screenBuf[(height / 2) - (10/2) + i][(width / 2) - (10/ 2)] = to_string(i+1);
		}

		//이름
		for (int i = 0; i < informCnt; i++) {
			for (int j = 0; j < inform[i].name.length(); j++) {
				if (i >= 9) {
					screenBuf[(height / 2) - (10 / 2) + i][(width / 2) - (10 / 2) + 2 + j] = inform[i].name[j];
				}
				else {
					screenBuf[(height / 2) - (10 / 2) + i][(width / 2) - (10 / 2) + 3 + j] = inform[i].name[j];
				}
			}
		}

		//점수
		for (int i = 0; i < informCnt; i++) {
			string score = to_string(inform[i].score);
			for (int j = 0; j < score.length(); j++) {
				screenBuf[(height / 2) - (10 / 2) + i][(width / 2) - (10 / 2) + 7 + j] = score[j];
			}
		}

		return NULL;
	}
};