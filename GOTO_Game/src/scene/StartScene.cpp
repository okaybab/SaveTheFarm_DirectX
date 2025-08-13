#include "StartScene.h"
#include "CrosshairPrefab.h"
#include "StartMenuPrefab.h"
#include "FadeInOutFXManagerPrefab.h"
#include <Camera.h>
#include <RigidBody2D.h>
#include <Collider2D.h>
#include <PhysicsManager.h>
#include "GamepadRumbleManager.h"
#include "SoundManager.h"
#include "EnemySpawnManager.h"
#include "FadeInOutFXManager.h"
#include "SpriteRenderer.h"
#include "CrosshairMove.h"
#include "InteractiveTitle.h"
#include "CameraShaker.h"
#include "CrosshairController.h"
#include "StartAnimal.h"
#include <Canvas.h>

void StartScene::Initialize()
{
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

	auto p1 = GameObject::Find(L"Player1");
	auto p2 = GameObject::Find(L"Player2");

	if (Object::IsValidObject(p1))
	{
		p1->GetComponent<CrosshairMove>()->cam = mainCam->GetComponent<Camera>();
		//p1->GetComponent<CrosshairCollide>()->commonCam = nullptr;
		//p1->GetComponent<CrosshairCollide>()->p1Cam = nullptr;
		//p1->GetComponent<CrosshairCollide>()->p2Cam = nullptr;
		p1->GetComponent<CrosshairCollide>()->ChangeCollideMode(CrosshairCollideMode::LocalScreen);
		p1->GetComponent<CrosshairController>()->ChangeType(CrosshairType::TriggerGun);
	}
	if (Object::IsValidObject(p2))
	{
		p2->GetComponent<CrosshairMove>()->cam = mainCam->GetComponent<Camera>();
		p2->GetComponent<CrosshairCollide>()->ChangeCollideMode(CrosshairCollideMode::LocalScreen);
		p2->GetComponent<CrosshairController>()->ChangeType(CrosshairType::TriggerGun);
	}

	auto BG = new GameObject(L"BG");
	BG->AddComponent<SpriteRenderer>()->SetSprite(L"../Resources/artResource/Background/Background1.png");
	BG->GetComponent<SpriteRenderer>()->SetRenderOrder(-120000);
	BG->GetTransform()->SetLocalScale({ 0.5f,0.5f });

	auto Title = new GameObject(L"Title");
	auto TitleSpriteGO = new GameObject(L"Title Sprite");
	TitleSpriteGO->AddComponent<SpriteRenderer>()->SetSprite(L"../Resources/artResource/Title/타이틀로고ver2.png");
	TitleSpriteGO->GetTransform()->SetParent(Title->GetTransform());
	TitleSpriteGO->GetTransform()->SetLocalPosition({ 0,100.0f });

	Title->GetTransform()->SetPosition({ 0.0f,300.0f });
	auto titleCol = Title->AddComponent<Collider2D>();
	auto titleRb = Title->AddComponent<RigidBody2D>();

	titleCol->SetSize({ 750.0f,250.0f });
	titleRb->SetPosition({ 0.0f,300.0f });
	titleRb->GetWrapperBody()->GetBody()->mass = 50.0f;
	//titleRb->SetMass({ 220.0f });

	Title->AddComponent<InteractiveTitle>();

	PhysicsManager::Get()->SetGravity({ 0.0f,-250.0f });


	auto RumbleManagerGO = new GameObject(L"GamePadRumbleManager");
	RumbleManagerGO->AddComponent<GamepadRumbleManager>();

	StartMenuPrefab::CreateStartMenu();

	FadeInOutFXManagerPrefab::CreateFadeInOutFXPrefab();

	if (FadeInOutFXManager::instance)
		FadeInOutFXManager::instance->FadeIn();

	auto soundManager = new GameObject(L"SoundManager");
	soundManager->AddComponent<SoundManager>();

	if (EnemySpawnManager::instance)
	{
		EnemySpawnManager::instance->SetEGameType(E_Game_Type::TITLE);
	}
	else
	{
		auto spawner = new GameObject(L"스폰매니저");
		spawner->AddComponent<EnemySpawnManager>()->SetEGameType(E_Game_Type::TITLE);
	}
	auto startanimal = new GameObject(L"스타스애니멀");
	startanimal->AddComponent<StartAnimal>();

	//메인 캔버스
	auto canvas = new GameObject(L"Canvas");
	canvas->AddComponent<Canvas>();
}
