#include "PlayScene.h"
#include <Camera.h>
#include <Image.h>
#include <Canvas.h>
#include <RectTransform.h>
#include <SpriteRenderer.h>
#include <Animator.h>
#include "CrosshairPrefab.h"
#include "EnhancedCrosshairFire.h"

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
	auto BackgroundGO1 = new GameObject(L"Background");	
	auto BackgdoundSprite1 = BackgroundGO1->AddComponent<SpriteRenderer>();
	BackgdoundSprite1->SetSprite(L"../Resources/artResource/Background/NewBackground_empty.png");
	BackgdoundSprite1->SetRenderLayer(1 << 1);
	BackgroundGO1->GetTransform()->SetLossyScale({ 0.7f, 0.7f});
	BackgdoundSprite1->SetRenderOrder(-2500);

	auto BackgroundGO2 = new GameObject(L"Background");
	auto BackgdoundSprite2 = BackgroundGO1->AddComponent<SpriteRenderer>();
	BackgdoundSprite2->SetSprite(L"../Resources/artResource/Background/NewBackground_empty_flip.png");
	BackgdoundSprite2->SetRenderLayer(1 << 2);
	BackgroundGO2->GetTransform()->SetLossyScale({ 0.7f, 0.7f });
	BackgdoundSprite2->SetRenderOrder(-2500);

	//배경 움직이는 그림
	auto plantAnimationSource = Resource::Load<AnimatorController>(L"../Resources/Animation/controller/plantCon_AnimController.json");

	auto p1Reed1GO = new GameObject(L"Reed");
	auto p1Reed1Sprite = p1Reed1GO->AddComponent<SpriteRenderer>();
	p1Reed1Sprite->SetRenderLayer(1 << 1);
	p1Reed1Sprite->SetRenderOrder(-2499);
	p1Reed1GO->GetTransform()->SetLossyScale({ 0.89f, 0.89f });
	p1Reed1GO->GetTransform()->SetPosition({ -760,-185 });
	auto p1Reed1Animator = p1Reed1GO->AddComponent<Animator>();
	p1Reed1Animator->SetAnimatorController(plantAnimationSource);
	p1Reed1Animator->Play(L"reed1");

	auto p1Sunflower1GO = new GameObject(L"Sunflower1");
	auto p1Sunflower1Sprite = p1Sunflower1GO->AddComponent<SpriteRenderer>();
	p1Sunflower1Sprite->SetRenderLayer(1 << 1);
	p1Sunflower1Sprite->SetRenderOrder(-2499);
	p1Sunflower1GO->GetTransform()->SetLossyScale({ 0.84f, 0.84f });
	p1Sunflower1GO->GetTransform()->SetPosition({ 810,-145 });
	auto p1Sunflower1Animator = p1Sunflower1GO->AddComponent<Animator>();
	p1Sunflower1Animator->SetAnimatorController(plantAnimationSource);
	p1Sunflower1Animator->Play(L"sunflower1");

	auto p1Sunflower2GO = new GameObject(L"Sunflower2");
	auto p1Sunflower2Sprite = p1Sunflower2GO->AddComponent<SpriteRenderer>();
	p1Sunflower2Sprite->SetRenderLayer(1 << 1);
	p1Sunflower2Sprite->SetRenderOrder(-2499);
	p1Sunflower2GO->GetTransform()->SetLossyScale({ 0.63f, 0.63f });
	p1Sunflower2GO->GetTransform()->SetPosition({ 630,-132 });
	auto p1Sunflower2Animator = p1Sunflower2GO->AddComponent<Animator>();
	p1Sunflower2Animator->SetAnimatorController(plantAnimationSource);
	p1Sunflower2Animator->Play(L"sunflower2");

	auto p2Sunflower1GO = new GameObject(L"Sunflower1");
	auto p2Sunflower1Sprite = p2Sunflower1GO->AddComponent<SpriteRenderer>();
	p2Sunflower1Sprite->SetRenderLayer(1 << 2);
	p2Sunflower1Sprite->SetRenderOrder(-2499);
	p2Sunflower1GO->GetTransform()->SetLossyScale({ 0.84f, 0.84f });
	p2Sunflower1GO->GetTransform()->SetPosition({ -595,-97 });
	auto p2Sunflower1Animator = p2Sunflower1GO->AddComponent<Animator>();
	p2Sunflower1Animator->SetAnimatorController(plantAnimationSource);
	p2Sunflower1Animator->Play(L"sunflower1");

	auto p2Sunflower2GO = new GameObject(L"Sunflower2");
	auto p2Sunflower2Sprite = p2Sunflower2GO->AddComponent<SpriteRenderer>();
	p2Sunflower2Sprite->SetRenderLayer(1 << 2);
	p2Sunflower2Sprite->SetRenderOrder(-2499);
	p2Sunflower2GO->GetTransform()->SetLossyScale({ 0.71f, 0.71f });
	p2Sunflower2GO->GetTransform()->SetPosition({ 760,-180 });
	auto p2Sunflower2Animator = p2Sunflower2GO->AddComponent<Animator>();
	p2Sunflower2Animator->SetAnimatorController(plantAnimationSource);
	p2Sunflower2Animator->Play(L"sunflower2");


	//메인 캔버스
	auto canvas = new GameObject(L"Canvas");
	canvas->AddComponent<Canvas>();

	//화면 가운데 분할용 선분
	//auto splitImageGO = new GameObject(L"split img");
	//auto splitImage = splitImageGO->AddComponent<Image>();
	//splitImage->GetRectTransform()->SetParent(canvas->GetTransform(), false);
	//splitImage->SetSprite(L"../Resources/Demo/-5.png");
	//splitImage->GetRectTransform()->SetAnchoredPosition({ Screen::GetWidth() * 0.5f,Screen::GetHeight() * 0.5f });
	//splitImage->GetRectTransform()->SetPivot({ 0.5f,0.5f });

	auto itemManager = new GameObject(L"아이템매니저");
	itemManager->AddComponent<ItemManager>();
	auto gameManager = new GameObject(L"게임매니저");
	gameManager->AddComponent<GameManager>();
	auto gimmickManager = new GameObject(L"기믹매니저");
	gimmickManager->AddComponent<GimmickManager>();
	EnemySpawnManager::instance->SetEGameType(E_Game_Type::GAME1);
}
