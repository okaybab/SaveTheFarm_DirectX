#include "PlayScene.h"
#include <Camera.h>
#include <Image.h>
#include <Canvas.h>
#include <RectTransform.h>
#include <SpriteRenderer.h>

#include "ItemManager.h"
#include "GameManager.h"
#include "GimmickManager.h"
#include "EnemySpawnManager.h"

#include "CameraShaker.h"
#include "CrosshairFire.h"
#include "SoundManager.h"
#include "CameraMove.h"
#include "CrosshairMove.h"
#include "CrosshairPrefab.h"
#include "EnhancedCrosshairFire.h"
#include "DirIndicatorController.h"

void PlayScene::Initialize()
{
	//플레이어1 카메라
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

	auto p1CamEnemyIndicator = new GameObject(L"p1 enemy indicator");
	auto p1IndicatorController = p1CamEnemyIndicator->AddComponent<DirIndicatorController>();
	p1IndicatorController->cam = player1Cam;
	p1IndicatorController->id = 0;


	//플레이어2 카메라
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

	auto p2CamEnemyIndicator = new GameObject(L"p2 enemy indicator");
	auto p2IndicatorController = p2CamEnemyIndicator->AddComponent<DirIndicatorController>();
	p2IndicatorController->cam = player2Cam;
	p2IndicatorController->id = 1;

	//카메라 세팅
	auto p1 = GameObject::Find(L"Player1");
	auto p2 = GameObject::Find(L"Player2");

	if (Object::IsValidObject(p1))
	{
		p1->GetComponent<CrosshairMove>()->cam = player1Cam;
	}
	if (Object::IsValidObject(p2))
	{
		p2->GetComponent<CrosshairMove>()->cam = player2Cam;
	}

	//배경 이미지
	auto BackgroundGO = new GameObject(L"Background");	
	auto BackgdoundSprite = BackgroundGO->AddComponent<SpriteRenderer>();
	BackgdoundSprite->SetSprite(L"../Resources/artResource/Background/Background1.png");
	BackgroundGO->GetTransform()->SetLossyScale({ 0.7f, 0.7f});
	BackgdoundSprite->SetRenderOrder(-2500);

	//메인 캔버스
	auto canvas = new GameObject(L"Canvas");
	canvas->AddComponent<Canvas>();
	auto itemManager = new GameObject(L"아이템매니저");
	itemManager->AddComponent<ItemManager>();
	auto gameManager = new GameObject(L"게임매니저");
	gameManager->AddComponent<GameManager>();
	auto gimmickManager = new GameObject(L"기믹매니저");
	gimmickManager->AddComponent<GimmickManager>();
	EnemySpawnManager::instance->SetEGameType(E_Game_Type::GAME1);
}
