#include "StartScene.h"
#include "CrosshairPrefab.h"
#include "StartMenuPrefab.h"
#include <Camera.h>
#include "GamepadRumbleManager.h"

void StartScene::Initialize()
{
	//카메라
	auto mainCam = Camera::CreateMainCamera();

	if (!GameObject::Find(L"Player1") && !GameObject::Find(L"Player2"))
	{
		auto CrossHair1GO = CrosshairPrefab::CreateEnhancedCrosshair(0);
		auto CrossHair2GO = CrosshairPrefab::CreateEnhancedCrosshair(1);

		CrossHair1GO->name = L"Player1";
		CrossHair2GO->name = L"Player2";

		//커서 유지
		Object::DontDestroyOnLoad(CrossHair1GO);
		Object::DontDestroyOnLoad(CrossHair2GO);
	}

	auto RumbleManagerGO = new GameObject(L"GamePadRumbleManager");
	RumbleManagerGO->AddComponent<GamepadRumbleManager>();

	StartMenuPrefab::CreateStartMenu();
}
