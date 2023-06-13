#pragma once
#include <string>
#include <vector>
#include <Windows.h>
#include <iostream>
#include<cstdlib>

#include "Object.hpp"
#include "console.hpp"

using namespace std;
class Scene {
public : 
	//화면 크기
	int width;
	int height;

	//게임 상태
	bool gamePause = false;
	bool rankWirte = false;
	
	//0x01 종료
	//0x02 게임 진행
	//0x03 랭킹
	//0x04 
	//0x05 게임 오버
	unsigned char Status;
	unsigned char prevStatus;

	bool isNight = false;

	//오브젝트 속도
	float frontSpeed = 0.1;
	float backSpeed = 0.3;

	//스크린 관련 버퍼,색
	vector<vector<string>> screenBuf;
	vector<vector<int>> screenBufColor;

	string drawString = "";

	//오브젝트 풀
	int PoolCount = 10;
	vector<Obstacle> ObstaclePool;
	vector<Obstacle2> ObstaclePool2;
	vector<Cloud> CloudPool;

	//객체
	Player* p;	
	Point* point;
	Ground* ground;
	Mountain* mountain;
	Ui* ui;

	// 게임 루프 변수 초기화
	double deltaTime = 0.0;
	chrono::steady_clock::time_point currentTime = chrono::steady_clock::now();
	chrono::steady_clock::time_point previousTime = currentTime;

	//맵관련
	vector<int> mapV;
	int mapCnt = 0;

	// 생성 딜레이 체크 
	//장애물
	double obstacleDelay;
	double obstacleTimer = 0.0;
	int obstaclePoolCnt = 0;
	int obstaclePoolCnt2 = 0;
	//구름
	double CloudDelay;
	double CloudTimer = 0.0;
	int CloudPoolCnt = 0;


	//게임시간
	double gameTime = 0.0;

	//입력
	unsigned int key = 0;

	Scene(int width,int height) {
		this->width = width;
		this->height = height;

		screenBuf.resize(height, vector<string>(width));
		screenBufColor.resize(height, vector<int>(width));

		ClearScreenBuf();

		//생성
		p = new Player(width, height);
		point = new Point(width, height);
		ground = new Ground(width, height);
		mountain = new Mountain(width, height);
		ui = new Ui(width, height);

		for (int i = 0; i < PoolCount; i++) {
			ObstaclePool.push_back(Obstacle(width,height));
		}

		for (int i = 0; i < PoolCount; i++) {
			ObstaclePool2.push_back(Obstacle2(width, height));
		}

		for (int j = 0; j < PoolCount; j++) {
			CloudPool.push_back(Cloud(width, height));
		}

		//1에서 2초 1000밀리 ~ 2000밀리 세컨즈
		srand(GetTickCount());
		obstacleDelay = 1.0;

		//콘솔 숨기기
		CONSOLE_CURSOR_INFO cursorInfo = { 0, };
		cursorInfo.dwSize = 1; //커서 굵기 (1 ~ 100)
		cursorInfo.bVisible = FALSE; //커서 Visible TRUE(보임) FALSE(숨김)
		SetConsoleCursorInfo(GetStdHandle(STD_OUTPUT_HANDLE), &cursorInfo);
	}

	//스크린 버퍼 클리어
	void ClearScreenBuf() {
		for (int y = 0; y < height; ++y) {
			for (int x = 0; x < width; ++x) {
				screenBuf[y][x] = " ";
				if (!isNight) {
					screenBufColor[y][x] = lightgray;
				}
				else {
					screenBufColor[y][x] = black;
				}
			}
		}
	}

	void Update() {
		//시간
		currentTime = chrono::steady_clock::now();
		chrono::duration<double> elapsedTime = currentTime - previousTime;
		previousTime = currentTime;
		deltaTime = elapsedTime.count();

		if (key == 27 && Status == 0x02) {
			gamePause = gamePause ? 0 : 1;
		}

		//게임오버 아니고 게임시작 했으면
		if (Status == 0x02 && !gamePause) {
			rankWirte = false;
			gameTime += deltaTime;

			//활성화된 장애물 업데이트
			for (int i = 0; i < PoolCount; i++) {
				if (ObstaclePool[i].active == true) {
					statusChange(ObstaclePool[i].Update(frontSpeed,p));
					if (Status == 0x05) {
						ObstaclePool[i].setActive();
					}
				}
			}

			//활성화된 장애물 업데이트
			for (int i = 0; i < PoolCount; i++) {
				if (ObstaclePool2[i].active == true) {
					statusChange(ObstaclePool2[i].Update(frontSpeed, p));
					if (Status == 0x05) {
						ObstaclePool[i].setActive();
					}
				}
			}

			//플레이어 업데이트
			p->Update(key);

			frontSpeed -= point->thisGamePoint * 0.000001;

			//점수 업데이트
			point->Update(gameTime);
			if (point->thisGamePoint > 30) {
				isNight = true;
			}
			
			//활성화된 구름 업데이트
			for (int i = 0; i < PoolCount; i++) {
				if (CloudPool[i].active == true) {
					CloudPool[i].Update(backSpeed);
				}
			}

			backSpeed -= point->thisGamePoint * 0.0000001;

			//산 업데이트
			mountain->Update(backSpeed + 0.3);

			//장애물, 구름 활성화 시간 체크
			obstacleTimer += deltaTime;
			CloudTimer += deltaTime;

			//장애물 간격 
			if (obstacleTimer >= obstacleDelay) {
				if (mapV[mapCnt] == 1) {
					//맵 로더에 따른 장애물 꺼내오기
					obstacleTimer = 0.0;

					ObstaclePool[obstaclePoolCnt].setActive();
					obstaclePoolCnt = obstaclePoolCnt == PoolCount - 1 ? 0 : ++obstaclePoolCnt;
				}
				if (mapV[mapCnt] == 2) {
					//맵 로더에 따른 장애물 꺼내오기
					obstacleTimer = 0.0;

					ObstaclePool2[obstaclePoolCnt2].setActive();
					obstaclePoolCnt2 = obstaclePoolCnt2 == PoolCount - 1 ? 0 : ++obstaclePoolCnt2;
				}
				mapCnt++;
				//맵 로드 끝나면 초기화
				if (mapCnt > mapV.size())
					mapCnt = 0;
				obstacleTimer = 0;
			}

			//구름 간격 
			if (CloudTimer >= CloudDelay) {
				CloudTimer = 0.0;

				CloudPool[CloudPoolCnt].setActive();
				CloudPoolCnt = CloudPoolCnt == PoolCount - 1 ? 0 : ++CloudPoolCnt;

				// 새로운 obstacleDelay 설정
				srand(GetTickCount64());
				CloudDelay = (1000 + (rand() % 200000)) / 1000;
			}
		}
		else if(Status == 0x05){
			//게임 오버일 때
			gameTime = 0.0;
			p->initCOORD();

			frontSpeed = 0.1;
			backSpeed = 0.1;

			for (int i = 0; i < PoolCount; i++) {
				if (ObstaclePool[i].active == true) {
					ObstaclePool[i].setActive();
				}
			}
			if (rankWirte) {
				point->PointInit();
			}
			isNight = false;
		}
	}

	//그리기
	void Draw() {
		//화면 초기화
		ClearScreenBuf();

		//랭킹 볼때 랭킹 화면 외엔 그리지 않음
		if (Status == 0x03) {
			statusChange(ui->RankSee(screenBuf, key));
		}
		else {
			p->Render(screenBuf);

			//게임 중이 아닐때
			if (Status != 0x02 && Status != 0x05) {
				statusChange(ui->Start(screenBuf, key));
			}
			//사망시
			else if (Status == 0x05) {
				if (!rankWirte) {
					rankWirte = ui->Rank(screenBuf, key, point->thisGamePoint);
				}
				else if(rankWirte){
					statusChange(ui->GameOver(screenBuf, key));
				}
			}
			//게임 중
			else {
				//오브젝트 그리기
				point->Render(screenBuf);
				ground->Render(screenBuf);

				for (int i = 0; i < PoolCount; i++) {
					if (ObstaclePool[i].active == true) {
						ObstaclePool[i].Render(screenBuf);
					}
				}

				for (int i = 0; i < PoolCount; i++) {
					if (ObstaclePool2[i].active == true) {
						ObstaclePool2[i].Render(screenBuf);
					}
				}

				mountain->Render(screenBuf, screenBufColor);

				for (int i = 0; i < PoolCount; i++) {
					if (CloudPool[i].active == true) {
						CloudPool[i].Render(screenBuf, screenBufColor);
					}
				}

				//게임 중 일때 일시정지면
				if (gamePause) {
					statusChange(ui->Pause(screenBuf, key));
				}
			}
		}

		//출력 수정 에정 gotoXY()
		for (int y = 0; y < height; y++) {
			for (int x = 0; x < width; x++) {
				if (!isNight) {
					setColor(darkgray, screenBufColor[y][x]);
				}
				else {
					setColor(white, screenBufColor[y][x]);
				}
				drawString += screenBuf[y][x];
			}
			drawString += '\n';
		}
		gotoxy(0, 0);
		cout << drawString;
		drawString = "";
		cout << flush;
	}

	//상태 받기
	void statusChange(char status) {
		switch (status)
		{
		case 0x01:
			Status = 0x01;
			prevStatus = 0x01;
			break;
		case 0x02:
			Status = 0x02;
			prevStatus = 0x02;
			break;
		case 0x03:
			Status = 0x03;
			break;
		case 0x04:
			Status = 0x04;
			prevStatus = 0x04;
			break;
		case 0x05:
			Status = 0x05;
			prevStatus = 0x05;
			break;
		case 0x06:
			Status = prevStatus;
			break;
		default:
			break;
		}
	}

	//맵 읽기
	void mapLoad() {
		ifstream fin;
		fin.open("Map.txt");

		char character;
		while (true)
		{
			fin.get(character);
			mapV.push_back(character - '0');
			if (fin.fail())
			{
				break;
			}
		}

		fin.close();
	}

	~Scene(){
		free(p);
		free(point);
		free(ground);
		free(mountain);
		free(ui);
	}
};