#include <stdio.h>
#include <chrono>
#include <thread>

#include "Scene.hpp"

using namespace std;

int KeyEvent() {
    if (_kbhit()>0) {
        return _getch();
    }
}

int main(){
    // 게임 루프 설정
    const double targetFPS = 60;
    const double targetFrameTime = 1.0 / targetFPS;

    // 게임 루프 변수 초기화
    double deltaTime = 0.0;
    chrono::steady_clock::time_point currentTime = chrono::steady_clock::now();
    chrono::steady_clock::time_point previousTime = currentTime;

    // 게임 초기화
    Scene scene(100, 16);
    scene.mapLoad();

    while (1)
    {
        currentTime = chrono::steady_clock::now();
        chrono::duration<double> elapsedTime = currentTime - previousTime;
        previousTime = currentTime;
        deltaTime = elapsedTime.count();

        // 입력 처리
        scene.key = KeyEvent();

        // 게임 로직 업데이트
        scene.Update();

        // 화면 그리기
        scene.Draw();

        //게임 종료
        if (scene.Status == 0x01) {
            break;
        }

        //프레임 간 시간 지연
        double sleepTime = targetFrameTime - deltaTime;
        if (sleepTime > 0) {
            // 대기 시간이 있다면 슬립을 통해 루프 속도 조절
            this_thread::sleep_for(chrono::duration<double>(sleepTime));
        }
    }

    // 게임 종료 처리

    return 0;
}