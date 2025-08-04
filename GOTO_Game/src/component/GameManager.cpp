#include "GameManager.h"
#include "TutorialImage.h"

#include "EnemySpawner.h"
#include "BaseEnemyObject.h"
#include "ItemEnemy.h"

#include <time.h>

using namespace GOTOEngine;
GameManager* GameManager::instance = nullptr;

void GameManager::Awake(){
	if (!instance)
	{
		instance = this;
	}
	else
	{
		Destroy(GetGameObject());
	}
	auto canvas = GameObject::Find(L"Canvas");
	auto p1scitem = new GameObject;
	auto p2scitem = new GameObject;
	auto timeitem = new GameObject;
	auto p1winitem = new GameObject;
	auto p2winitem = new GameObject;
	P1sctext = p1scitem->AddComponent<Text>();
	P2sctext = p2scitem->AddComponent<Text>();
	Timetext = timeitem->AddComponent<Text>();
	P1wintext = p1winitem->AddComponent<Text>();
	P2wintext = p2winitem->AddComponent<Text>();

	p1scitem->GetTransform()->SetParent(canvas->GetTransform());
	p2scitem->GetTransform()->SetParent(canvas->GetTransform());
	timeitem->GetTransform()->SetParent(canvas->GetTransform());
	p1winitem->GetTransform()->SetParent(canvas->GetTransform());
	p2winitem->GetTransform()->SetParent(canvas->GetTransform());

	P1sctext->GetRectTransform()->SetAnchoredPosition({
			Screen::GetWidth() * 0.2f, Screen::GetHeight() * 0.8f });
	P2sctext->GetRectTransform()->SetAnchoredPosition({
			Screen::GetWidth() * 0.8f, Screen::GetHeight() * 0.8f });
	Timetext->GetRectTransform()->SetAnchoredPosition({
			Screen::GetWidth() * 0.5f, Screen::GetHeight() * 0.8f });
	P1wintext->GetRectTransform()->SetAnchoredPosition({
			Screen::GetWidth() * 0.2f, Screen::GetHeight() * 0.5f });
	P2wintext->GetRectTransform()->SetAnchoredPosition({
			Screen::GetWidth() * 0.8f, Screen::GetHeight() * 0.5f });

	P1sctext->SetFont(L"../Resources/Maplestory Light.ttf");
	P2sctext->SetFont(L"../Resources/Maplestory Light.ttf");
	Timetext->SetFont(L"../Resources/Maplestory Light.ttf");
	P1wintext->SetFont(L"../Resources/Maplestory Light.ttf");
	P2wintext->SetFont(L"../Resources/Maplestory Light.ttf");
	P1sctext->SetColor({ 255,0,0,255 });
	P2sctext->SetColor({ 255,0,0,255 });
	Timetext->SetColor({ 255,0,0,255 });
	P1wintext->SetColor({ 255,0,0,255 });
	P2wintext->SetColor({ 255,0,0,255 });
	Tutorial = new GameObject;
	Tutorial->AddComponent<TutorialImage>();

	srand(time(NULL));
	p1itemchange = rand() % 4 + 1;
	p2itemchange = rand() % 4 + 1;
	P1wintext->text = std::wstring(L"");
	P2wintext->text = std::wstring(L"");
}

void GOTOEngine::GameManager::Start()
{
	EnemySpawner = EnemySpawner::instance;
}

void GameManager::OnDestroy() {
	if (instance == this)
		instance = nullptr;
}

void GameManager::Update() {
	if (setactive) {
		if(IsValidObject(Tutorial))
		{
			Destroy(Tutorial);
			Tutorial = nullptr;
		}
		if (GameTimer > 0.0f) {
			GameTimer -= TIME_GET_DELTATIME();
			if (NormalTiming - GameTimer >= 5.0f) {
				//일반 몬스터 랜덤 생성
				EnemySpawner->CreateEnemy(E_EnemyType::move, 1);
				EnemySpawner->CreateEnemy(E_EnemyType::move, 2);
				NormalTiming -= 5.0f;
			}
			if (GimmickTiming - GameTimer >= 15.0f) {
				//기믹 몬스터 랜덤 생성
				EnemySpawner->CreateEnemy(E_EnemyType::gimmick, 1);
				EnemySpawner->CreateEnemy(E_EnemyType::gimmick, 2);
				GimmickTiming -= 15.0f;
			}
			
			if (GameTimer <= ItemTiming[0]) {
				if (p1itemchange == 1 || p1itemchange == 4) {
					//p1 얼음새
					EnemySpawner->CreateEnemy(E_EnemyType::itemspawn, 0, 1);
				}
				else if (p1itemchange == 2 || p1itemchange == 3) {
					//p1 폭탄새
					EnemySpawner->CreateEnemy(E_EnemyType::itemspawn, 1, 1);
				}
				if (p2itemchange == 1 || p2itemchange == 4) {
					//p2 얼음새
					EnemySpawner->CreateEnemy(E_EnemyType::itemspawn, 0, 2);
				}
				else if (p2itemchange == 2 || p2itemchange == 3) {
					//p2 폭탄새
					EnemySpawner->CreateEnemy(E_EnemyType::itemspawn, 1, 2);
				}
				ItemTiming[0] = -1.0f;
			}
			if (GameTimer <= ItemTiming[1]) {
				if (p1itemchange == 1 || p1itemchange == 4) {
					//p1 폭탄새
					EnemySpawner->CreateEnemy(E_EnemyType::itemspawn, 1, 1);
				}
				else if (p1itemchange == 2) {
					//p1 얼음새
					EnemySpawner->CreateEnemy(E_EnemyType::itemspawn, 0, 1);
				}
				else if (p1itemchange == 3) {
					//p1 황금새
					EnemySpawner->CreateEnemy(E_EnemyType::itemspawn, 2, 1);
				}
				if (p2itemchange == 1 || p2itemchange == 4) {
					//p2 폭탄새
					EnemySpawner->CreateEnemy(E_EnemyType::itemspawn, 1, 2);
				}
				else if (p2itemchange == 2) {
					//p2 얼음새
					EnemySpawner->CreateEnemy(E_EnemyType::itemspawn, 0, 2);
				}
				else if (p2itemchange == 3) {
					//p2 황금새
					EnemySpawner->CreateEnemy(E_EnemyType::itemspawn, 2, 2);
				}
				ItemTiming[1] = -1.0f;
			}
			if (GameTimer <= ItemTiming[2]) {
				if (p1itemchange == 1) {
					//p1 황금새
					EnemySpawner->CreateEnemy(E_EnemyType::itemspawn, 2, 1);
				}
				else if (p1itemchange == 2 || p1itemchange == 4) {
					//p1 폭탄새
					EnemySpawner->CreateEnemy(E_EnemyType::itemspawn, 1, 1);
				}
				else if (p1itemchange == 3) {
					//p1 얼음새
					EnemySpawner->CreateEnemy(E_EnemyType::itemspawn, 0, 1);
				}
				if (p2itemchange == 1) {
					//p2 황금새
					EnemySpawner->CreateEnemy(E_EnemyType::itemspawn, 2, 2);
				}
				else if (p2itemchange == 2 || p2itemchange == 4) {
					//p2 폭탄새
					EnemySpawner->CreateEnemy(E_EnemyType::itemspawn, 1, 2);
				}
				else if (p2itemchange == 3) {
					//p2 얼음새
					EnemySpawner->CreateEnemy(E_EnemyType::itemspawn, 0, 2);
				}
				ItemTiming[2] = -1.0f;
			}
			if (GameTimer <= ItemTiming[3]) {
				if (p1itemchange == 1 || p1itemchange == 3) {
					//p1 폭탄새
					EnemySpawner->CreateEnemy(E_EnemyType::itemspawn, 1, 1);
				}
				else if (p1itemchange == 2 || p1itemchange == 4) {
					//p1 황금새
					EnemySpawner->CreateEnemy(E_EnemyType::itemspawn, 2, 1);
				}
				if (p2itemchange == 1 || p2itemchange == 3) {
					//p2 폭탄새
					EnemySpawner->CreateEnemy(E_EnemyType::itemspawn, 1, 2);
				}
				else if (p2itemchange == 2 || p2itemchange == 4) {
					//p2 황금새
					EnemySpawner->CreateEnemy(E_EnemyType::itemspawn, 2, 2);
				}
				ItemTiming[3] = -1.0f;
			}
			if (GameTimer <= ItemTiming[4]) {
				if (p1itemchange == 1 || p1itemchange == 4) {
					//p1 얼음새
					EnemySpawner->CreateEnemy(E_EnemyType::itemspawn, 0, 1);
				}
				else if (p1itemchange == 2 || p1itemchange == 3) {
					//p1 폭탄새
					EnemySpawner->CreateEnemy(E_EnemyType::itemspawn, 1, 1);
				}
				if (p2itemchange == 1 || p2itemchange == 4) {
					//p2 얼음새
					EnemySpawner->CreateEnemy(E_EnemyType::itemspawn, 0, 2);
				}
				else if (p2itemchange == 2 || p2itemchange == 3) {
					//p2 폭탄새
					EnemySpawner->CreateEnemy(E_EnemyType::itemspawn, 1, 2);
				}
				ItemTiming[4] = -1.0f;
			}
			if (GameTimer <= ItemTiming[5]) {
				if (p1itemchange == 1 || p1itemchange == 4) {
					//p1 폭탄새
					EnemySpawner->CreateEnemy(E_EnemyType::itemspawn, 1, 1);
				}
				else if (p1itemchange == 2 || p1itemchange == 3) {
					//p1 얼음새
					EnemySpawner->CreateEnemy(E_EnemyType::itemspawn, 0, 1);
				}
				if (p2itemchange == 1 || p2itemchange == 4) {
					//p2 폭탄새
					EnemySpawner->CreateEnemy(E_EnemyType::itemspawn, 1, 2);
				}
				else if (p2itemchange == 2 || p2itemchange == 3) {
					//p2 얼음새
					EnemySpawner->CreateEnemy(E_EnemyType::itemspawn, 0, 2);
				}
				ItemTiming[5] = -1.0f;
			}
			if (GameTimer <= 0.0f) {
				GameTimer = 0.0f;
			}
		}
		else if (GameTimer == 0.0f) {
			endingTimer -= TIME_GET_DELTATIME();
			if (P1Score > P2Score) {
				winner = 1;
				P1wintext->text = std::wstring(L"Win");
				P2wintext->text = std::wstring(L"Lose");
			}
			else if (P1Score < P2Score) {
				winner = 2;
				P2wintext->text = std::wstring(L"Win");
				P1wintext->text = std::wstring(L"Lose");
			}
			else {
				winner = 0;
				P1wintext->text = std::wstring(L"Deuce");
				P2wintext->text = std::wstring(L"Deuce");
			}
			if (endingTimer <= 0.0f) {
				SCENE_CHANGE_SCENE(L"StartScene");
			}
		}
		P1sctext->text = std::to_wstring(P1Score);
		P2sctext->text = std::to_wstring(P2Score);
		Timetext->text = std::to_wstring(static_cast<int>(floor(GameTimer)));
	}
	else {
		if (INPUT_GET_KEYDOWN(KeyCode::Alpha1) ||
			INPUT_GET_GAMEPAD_BUTTONDOWN(0, GamepadButton::ButtonWest)) {
			p1active = true;
		}
		if (INPUT_GET_KEYDOWN(KeyCode::Alpha0) ||
			INPUT_GET_GAMEPAD_BUTTONDOWN(1, GamepadButton::ButtonWest)) {
			p2active = true;
		}
		if (p1active && p2active) {
			setactive = true;
		}
	}
}

int GOTOEngine::GameManager::winner = 0;