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
	//ȭ�� ũ��
	int width;
	int height;

	//���� ����
	bool gamePause = false;
	bool rankWirte = false;
	
	//0x01 ����
	//0x02 ���� ����
	//0x03 ��ŷ
	//0x04 
	//0x05 ���� ����
	unsigned char Status;
	unsigned char prevStatus;

	bool isNight = false;

	//������Ʈ �ӵ�
	float frontSpeed = 0.1;
	float backSpeed = 0.3;

	//��ũ�� ���� ����,��
	vector<vector<string>> screenBuf;
	vector<vector<int>> screenBufColor;

	string drawString = "";

	//������Ʈ Ǯ
	int PoolCount = 10;
	vector<Obstacle> ObstaclePool;
	vector<Obstacle2> ObstaclePool2;
	vector<Cloud> CloudPool;

	//��ü
	Player* p;	
	Point* point;
	Ground* ground;
	Mountain* mountain;
	Ui* ui;

	// ���� ���� ���� �ʱ�ȭ
	double deltaTime = 0.0;
	chrono::steady_clock::time_point currentTime = chrono::steady_clock::now();
	chrono::steady_clock::time_point previousTime = currentTime;

	//�ʰ���
	vector<int> mapV;
	int mapCnt = 0;

	// ���� ������ üũ 
	//��ֹ�
	double obstacleDelay;
	double obstacleTimer = 0.0;
	int obstaclePoolCnt = 0;
	int obstaclePoolCnt2 = 0;
	//����
	double CloudDelay;
	double CloudTimer = 0.0;
	int CloudPoolCnt = 0;


	//���ӽð�
	double gameTime = 0.0;

	//�Է�
	unsigned int key = 0;

	Scene(int width,int height) {
		this->width = width;
		this->height = height;

		screenBuf.resize(height, vector<string>(width));
		screenBufColor.resize(height, vector<int>(width));

		ClearScreenBuf();

		//����
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

		//1���� 2�� 1000�и� ~ 2000�и� ������
		srand(GetTickCount());
		obstacleDelay = 1.0;

		//�ܼ� �����
		CONSOLE_CURSOR_INFO cursorInfo = { 0, };
		cursorInfo.dwSize = 1; //Ŀ�� ���� (1 ~ 100)
		cursorInfo.bVisible = FALSE; //Ŀ�� Visible TRUE(����) FALSE(����)
		SetConsoleCursorInfo(GetStdHandle(STD_OUTPUT_HANDLE), &cursorInfo);
	}

	//��ũ�� ���� Ŭ����
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
		//�ð�
		currentTime = chrono::steady_clock::now();
		chrono::duration<double> elapsedTime = currentTime - previousTime;
		previousTime = currentTime;
		deltaTime = elapsedTime.count();

		if (key == 27 && Status == 0x02) {
			gamePause = gamePause ? 0 : 1;
		}

		//���ӿ��� �ƴϰ� ���ӽ��� ������
		if (Status == 0x02 && !gamePause) {
			rankWirte = false;
			gameTime += deltaTime;

			//Ȱ��ȭ�� ��ֹ� ������Ʈ
			for (int i = 0; i < PoolCount; i++) {
				if (ObstaclePool[i].active == true) {
					statusChange(ObstaclePool[i].Update(frontSpeed,p));
					if (Status == 0x05) {
						ObstaclePool[i].setActive();
					}
				}
			}

			//Ȱ��ȭ�� ��ֹ� ������Ʈ
			for (int i = 0; i < PoolCount; i++) {
				if (ObstaclePool2[i].active == true) {
					statusChange(ObstaclePool2[i].Update(frontSpeed, p));
					if (Status == 0x05) {
						ObstaclePool[i].setActive();
					}
				}
			}

			//�÷��̾� ������Ʈ
			p->Update(key);

			frontSpeed -= point->thisGamePoint * 0.000001;

			//���� ������Ʈ
			point->Update(gameTime);
			if (point->thisGamePoint > 30) {
				isNight = true;
			}
			
			//Ȱ��ȭ�� ���� ������Ʈ
			for (int i = 0; i < PoolCount; i++) {
				if (CloudPool[i].active == true) {
					CloudPool[i].Update(backSpeed);
				}
			}

			backSpeed -= point->thisGamePoint * 0.0000001;

			//�� ������Ʈ
			mountain->Update(backSpeed + 0.3);

			//��ֹ�, ���� Ȱ��ȭ �ð� üũ
			obstacleTimer += deltaTime;
			CloudTimer += deltaTime;

			//��ֹ� ���� 
			if (obstacleTimer >= obstacleDelay) {
				if (mapV[mapCnt] == 1) {
					//�� �δ��� ���� ��ֹ� ��������
					obstacleTimer = 0.0;

					ObstaclePool[obstaclePoolCnt].setActive();
					obstaclePoolCnt = obstaclePoolCnt == PoolCount - 1 ? 0 : ++obstaclePoolCnt;
				}
				if (mapV[mapCnt] == 2) {
					//�� �δ��� ���� ��ֹ� ��������
					obstacleTimer = 0.0;

					ObstaclePool2[obstaclePoolCnt2].setActive();
					obstaclePoolCnt2 = obstaclePoolCnt2 == PoolCount - 1 ? 0 : ++obstaclePoolCnt2;
				}
				mapCnt++;
				//�� �ε� ������ �ʱ�ȭ
				if (mapCnt > mapV.size())
					mapCnt = 0;
				obstacleTimer = 0;
			}

			//���� ���� 
			if (CloudTimer >= CloudDelay) {
				CloudTimer = 0.0;

				CloudPool[CloudPoolCnt].setActive();
				CloudPoolCnt = CloudPoolCnt == PoolCount - 1 ? 0 : ++CloudPoolCnt;

				// ���ο� obstacleDelay ����
				srand(GetTickCount64());
				CloudDelay = (1000 + (rand() % 200000)) / 1000;
			}
		}
		else if(Status == 0x05){
			//���� ������ ��
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

	//�׸���
	void Draw() {
		//ȭ�� �ʱ�ȭ
		ClearScreenBuf();

		//��ŷ ���� ��ŷ ȭ�� �ܿ� �׸��� ����
		if (Status == 0x03) {
			statusChange(ui->RankSee(screenBuf, key));
		}
		else {
			p->Render(screenBuf);

			//���� ���� �ƴҶ�
			if (Status != 0x02 && Status != 0x05) {
				statusChange(ui->Start(screenBuf, key));
			}
			//�����
			else if (Status == 0x05) {
				if (!rankWirte) {
					rankWirte = ui->Rank(screenBuf, key, point->thisGamePoint);
				}
				else if(rankWirte){
					statusChange(ui->GameOver(screenBuf, key));
				}
			}
			//���� ��
			else {
				//������Ʈ �׸���
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

				//���� �� �϶� �Ͻ�������
				if (gamePause) {
					statusChange(ui->Pause(screenBuf, key));
				}
			}
		}

		//��� ���� ���� gotoXY()
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

	//���� �ޱ�
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

	//�� �б�
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