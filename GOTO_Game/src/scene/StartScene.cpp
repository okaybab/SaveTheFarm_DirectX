#include "StartScene.h"
#include "CrosshairPrefab.h"
#include "StartMenuPrefab.h"
#include "FadeInOutFXManagerPrefab.h"
#include <Camera.h>
#include "GamepadRumbleManager.h"
#include "SoundManager.h"
#include "FadeInOutFXManager.h"
#include "SpriteRenderer.h"
#include "CrosshairMove.h"

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
	}
	if (Object::IsValidObject(p2))
	{
		p2->GetComponent<CrosshairMove>()->cam = mainCam->GetComponent<Camera>();
	}

	auto BG = new GameObject(L"BG");
	BG->AddComponent<SpriteRenderer>()->SetSprite(L"../Resources/artResource/Background/Background1.png");
	BG->GetComponent<SpriteRenderer>()->SetRenderOrder(-120000);
	BG->GetTransform()->SetLocalScale({ 0.5f,0.5f });

	auto Title = new GameObject(L"Title");
	Title->AddComponent<SpriteRenderer>()->SetSprite(L"../Resources/artResource/Title/타이틀로고ver2.png");
	Title->GetTransform()->SetPosition({ 0.0f,400.0f });

	auto RumbleManagerGO = new GameObject(L"GamePadRumbleManager");
	RumbleManagerGO->AddComponent<GamepadRumbleManager>();

	StartMenuPrefab::CreateStartMenu();

	FadeInOutFXManagerPrefab::CreateFadeInOutFXPrefab();

	if (FadeInOutFXManager::instance)
		FadeInOutFXManager::instance->FadeIn();

	auto soundManager = new GameObject(L"SoundManager");
	soundManager->AddComponent<SoundManager>();
}
