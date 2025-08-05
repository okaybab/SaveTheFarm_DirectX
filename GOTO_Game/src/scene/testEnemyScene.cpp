#include "testEnemyScene.h"

#include "CrosshairPrefab.h"
#include "StartMenuPrefab.h"
#include <Camera.h>
#include <SpriteRenderer.h>
#include <Canvas.h>
#include <Text.h>
#include <RectTransform.h>
#include <Screen.h>

#include "EnemySpawner.h"
#include "ItemManager.h"
#include "GameManager.h"
#include "GimmickManager.h"
#include "SoundManager.h"

#include "StartMenuPrefab.h"
#include "CameraShaker.h"
#include "CrosshairFire.h"

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

	StartMenuPrefab::CreateStartMenu();


	auto soundManager = new GameObject(L"사운드매니저");

	soundManager->AddComponent<SoundManager>();
	//*/

	/*/ Play Scene
	auto player1CamGO = Camera::CreateMainCamera();
	auto player1Cam = player1CamGO->GetComponent<Camera>();
	player1Cam->name = L"플레이어1 카메라";
	player1Cam->SetRect({ 0.0f, 0.0f, 0.5f, 1.0f });
	player1Cam->SetRenderLayer(1 << 1);
	auto player1CamShaker = player1Cam->AddComponent<CameraShaker>();
	auto player1CrosshairGO = GameObject::Find(L"Player1");
	if (Object::IsValidObject(player1CrosshairGO))
	{
		player1CrosshairGO->GetComponent<CrosshairFire>()->onFire.Add([player1CamShaker](int id) { player1CamShaker->ShakeCamera(24, 55, 8); });
	}

	auto player2CamGO = Camera::CreateSubCamera();
	auto player2Cam = player2CamGO->GetComponent<Camera>();
	player2Cam->name = L"플레이어2 카메라";
	player2Cam->SetRect({ 0.5f, 0.0f, 0.5f, 1.0f });
	player2Cam->SetRenderLayer(1 << 2);
	auto player2CamShaker = player2Cam->AddComponent<CameraShaker>();
	auto player2CrosshairGO = GameObject::Find(L"Player2");
	if (Object::IsValidObject(player2CrosshairGO))
	{
		player2CrosshairGO->GetComponent<CrosshairFire>()->onFire.Add([player2CamShaker](int id) { player2CamShaker->ShakeCamera(24, 55, 8); });
	}

	//auto BackgroundGO = new GameObject(L"Background");
	//auto BackgdoundSprite = BackgroundGO->AddComponent<SpriteRenderer>();
	//BackgdoundSprite->SetSprite(L"../Resources/Demo/BG.png");
	//BackgroundGO->GetTransform()->SetLossyScale({ 1.35f, 1.35f });

	auto canvas = new GameObject(L"Canvas");
	canvas->AddComponent<Canvas>();
	auto itemManager = new GameObject(L"아이템매니저");
	itemManager->AddComponent<ItemManager>();
	auto gameManager = new GameObject(L"스코어매니저");
	gameManager->AddComponent<GameManager>();
	auto gimmickManager = new GameObject(L"기믹매니저");
	gimmickManager->AddComponent<GimmickManager>();
	auto spawner = new GameObject(L"enemySpawner");
	spawner->AddComponent<EnemySpawner>();
	//*/
}
