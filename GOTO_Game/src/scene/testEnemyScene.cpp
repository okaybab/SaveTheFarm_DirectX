#include "testEnemyScene.h"

#include "CrosshairPrefab.h"
#include "StartMenuPrefab.h"
#include <Camera.h>
#include <SpriteRenderer.h>
#include <Canvas.h>
#include <Text.h>
#include <RectTransform.h>
#include <Screen.h>

#include "EnemySpawnManager.h"
#include "ItemManager.h"
#include "GameManager.h"
#include "GimmickManager.h"
#include "SoundManager.h"

#include "FadeInOutFXManagerPrefab.h"
#include "FadeInOutFXManager.h"

#include "CameraShaker.h"
#include "CrosshairFire.h"
#include "CameraMove.h"

using namespace std;

using namespace GOTOEngine;

void testEnemyScene::Initialize()
{
	//*/ Start Scene
	//카메라
	auto mainCam = Camera::CreateMainCamera();

	if (!GameObject::Find(L"Player1") && !GameObject::Find(L"Player2"))
	{
		auto CrossHair1GO = CrosshairPrefab::CreateCrosshair(0);
		auto CrossHair2GO = CrosshairPrefab::CreateCrosshair(1);

		CrossHair1GO->name = L"Player1";
		CrossHair2GO->name = L"Player2";

		//커서 유지
		Object::DontDestroyOnLoad(CrossHair1GO);
		Object::DontDestroyOnLoad(CrossHair2GO);
	}

	auto RumbleManagerGO = new GameObject(L"GamePadRumbleManager");
	RumbleManagerGO->AddComponent<GamepadRumbleManager>();

	//StartMenuPrefab::CreateStartMenu();

	FadeInOutFXManagerPrefab::CreateFadeInOutFXPrefab();
	if (FadeInOutFXManager::instance)
		FadeInOutFXManager::instance->FadeIn();


	auto soundManager = new GameObject(L"사운드매니저");

	soundManager->AddComponent<SoundManager>();
	//*/

	//*/ Play Scene
	auto player1CamGO = Camera::CreateMainCamera();

	auto player1CamMoverGO = new GameObject(L"카메라 핸들러");
	player1CamMoverGO->AddComponent<CameraMove>()->id = 0;
	player1CamGO->GetTransform()->SetParent(player1CamMoverGO->GetTransform(), false);

	auto player1Cam = player1CamGO->GetComponent<Camera>();
	player1Cam->GetGameObject()->name = L"p1Cam";
	player1Cam->SetRect({ 0.0f, 0.0f, 0.5f, 1.0f });
	player1Cam->SetRenderLayer(1 << 1);
	auto player1CamShaker = player1Cam->AddComponent<CameraShaker>();
	auto player1CrosshairGO = GameObject::Find(L"Player1");

	auto player2CamGO = Camera::CreateSubCamera();

	auto player2CamMoverGO = new GameObject(L"카메라 핸들러2");
	player2CamMoverGO->AddComponent<CameraMove>()->id = 1;
	player2CamGO->GetTransform()->SetParent(player2CamMoverGO->GetTransform(), false);

	auto player2Cam = player2CamGO->GetComponent<Camera>();
	player2Cam->GetGameObject()->name = L"p2Cam";
	player2Cam->SetRect({ 0.5f, 0.0f, 0.5f, 1.0f });
	player2Cam->SetRenderLayer(1 << 2);
	auto player2CamShaker = player2Cam->AddComponent<CameraShaker>();
	auto player2CrosshairGO = GameObject::Find(L"Player2");

	auto BackgroundGO = new GameObject(L"Background");
	auto BackgdoundSprite = BackgroundGO->AddComponent<SpriteRenderer>();
	BackgdoundSprite->SetSprite(L"../Resources/Demo/BG.png");
	BackgroundGO->GetTransform()->SetLossyScale({ 1.35f, 1.35f });

	auto canvas = new GameObject(L"Canvas");
	canvas->AddComponent<Canvas>();
	auto itemManager = new GameObject(L"아이템매니저");
	itemManager->AddComponent<ItemManager>();
	auto gameManager = new GameObject(L"스코어매니저");
	gameManager->AddComponent<GameManager>();
	auto gimmickManager = new GameObject(L"기믹매니저");
	gimmickManager->AddComponent<GimmickManager>();
	auto spawner = new GameObject(L"스폰매니저");
	spawner->AddComponent<EnemySpawnManager>();
	//*/
}
