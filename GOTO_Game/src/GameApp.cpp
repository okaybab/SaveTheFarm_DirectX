// 콘솔을 겸용한 진입점을 정의합니다.

// 기본 라이브러리
#include <iostream>
#include <Windows.h>

// 엔진 및 매니저
#include "Engine.h"
#include "SceneManager.h"

// 씬
#include "PlayScene.h"
#include "StartScene.h"
#include "TestItemScene.h"
#include "testEnemyScene.h"
#include "DefenseScene.h"

void StartEngineLoop()
{
	ENGINE_INIT(1920, 1080, L"농장을 지켜라");

	//씬 할당 및 추가
	SCENE_ADD_SCENE<PlayScene>(L"PlayScene");
	SCENE_ADD_SCENE<TestItemScene>(L"TestItemScene");
	SCENE_ADD_SCENE<testEnemyScene>(L"testEnemyScene");
	SCENE_ADD_SCENE<StartScene>(L"StartScene");
	SCENE_ADD_SCENE<DefenseScene>(L"DefenseScene");

	//첫 씬으로 변환
	SCENE_CHANGE_SCENE(L"StartScene");

	ENGINE_RUN();
	ENGINE_SHUTDOWN();
}

int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
	_In_opt_ HINSTANCE hPrevInstance,
	_In_ LPWSTR    lpCmdLine,
	_In_ int       nCmdShow)
{
	StartEngineLoop();
	return 0;
}

int main()
{
	StartEngineLoop();
	return 0;
}