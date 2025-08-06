#include "GameManager.h"
#include "TutorialImage.h"

#include "EnemySpawner.h"
#include "BaseEnemyObject.h"
#include "ItemEnemy.h"

#include "FadeInOutFXManager.h"

#include <time.h>

using namespace GOTOEngine;
GameManager* GameManager::instance = nullptr;

void GameManager::Awake(){
	if (!instance)
	{
		instance = this;

		auto canvas = GameObject::Find(L"Canvas");
		auto scboarditem = new GameObject;
		scoreboard = scboarditem->AddComponent<Image>();
		scboarditem->GetTransform()->SetParent(canvas->GetTransform());
		scoreboard->GetRectTransform()->SetAnchoredPosition({ Screen::GetWidth() * 0.3f, Screen::GetHeight() * 0.9f });
		scoreboard->GetRectTransform()->SetSizeDelta({ Screen::GetWidth() * 0.4f, Screen::GetHeight() * 0.1f });
		scoreboard->SetSprite(L"../Resources/artResource/UI/Ingame/스코어보드, 타이머 배경.png");
		auto timeritem = new GameObject;
		timerscreen = timeritem->AddComponent<Image>();
		timeritem->GetTransform()->SetParent(canvas->GetTransform());
		timerscreen->GetRectTransform()->SetAnchoredPosition({ Screen::GetWidth() * 0.47f, Screen::GetHeight() * 0.9f });
		timerscreen->GetRectTransform()->SetSizeDelta({ Screen::GetWidth() * 0.06f, Screen::GetHeight() * 0.1f });
		timerscreen->SetSprite(L"../Resources/artResource/UI/Ingame/타이머 텍스트 올라가는 부분.png");
		auto p1item = new GameObject;
		p1score = p1item->AddComponent<Image>();
		p1item->GetTransform()->SetParent(canvas->GetTransform());
		p1score->GetRectTransform()->SetAnchoredPosition({ Screen::GetWidth() * 0.31f, Screen::GetHeight() * 0.9f });
		p1score->GetRectTransform()->SetSizeDelta({ Screen::GetWidth() * 0.06f, Screen::GetHeight() * 0.1f });
		p1score->SetSprite(L"../Resources/artResource/UI/Ingame/플레이어 스코어 텍스트 올라가는 부분.png");
		auto p2item = new GameObject;
		p2score = p2item->AddComponent<Image>();
		p2item->GetTransform()->SetParent(canvas->GetTransform());
		p2score->GetRectTransform()->SetAnchoredPosition({ Screen::GetWidth() * 0.63f, Screen::GetHeight() * 0.9f });
		p2score->GetRectTransform()->SetSizeDelta({ Screen::GetWidth() * 0.06f, Screen::GetHeight() * 0.1f });
		p2score->SetSprite(L"../Resources/artResource/UI/Ingame/플레이어 스코어 텍스트 올라가는 부분.png");

		auto p1sctextitem = new GameObject;
		auto p2sctextitem = new GameObject;
		auto timeitem = new GameObject;
		auto p1winitem = new GameObject;
		auto p2winitem = new GameObject;
		P1sctext = p1sctextitem->AddComponent<Text>();
		P2sctext = p2sctextitem->AddComponent<Text>();
		Timetext = timeitem->AddComponent<Text>();
		P1wintext = p1winitem->AddComponent<Text>();
		P2wintext = p2winitem->AddComponent<Text>();

		p1sctextitem->GetTransform()->SetParent(canvas->GetTransform());
		p2sctextitem->GetTransform()->SetParent(canvas->GetTransform());
		timeitem->GetTransform()->SetParent(canvas->GetTransform());
		p1winitem->GetTransform()->SetParent(canvas->GetTransform());
		p2winitem->GetTransform()->SetParent(canvas->GetTransform());

		P1sctext->horizontalAlign = TextHoriAlign::Center;
		P2sctext->horizontalAlign = TextHoriAlign::Center;
		Timetext->horizontalAlign = TextHoriAlign::Center;

		P1sctext->GetRectTransform()->SetAnchoredPosition({
				Screen::GetWidth() * 0.315f, Screen::GetHeight() * 0.88f });
		P2sctext->GetRectTransform()->SetAnchoredPosition({
				Screen::GetWidth() * 0.635f, Screen::GetHeight() * 0.88f });
		Timetext->GetRectTransform()->SetAnchoredPosition({
				Screen::GetWidth() * 0.475f, Screen::GetHeight() * 0.88f });
		P1wintext->GetRectTransform()->SetAnchoredPosition({
				Screen::GetWidth() * 0.2f, Screen::GetHeight() * 0.5f });
		P2wintext->GetRectTransform()->SetAnchoredPosition({
				Screen::GetWidth() * 0.7f, Screen::GetHeight() * 0.5f });

		P1sctext->size = 43;
		P2sctext->size = 43;
		Timetext->size = 43;
		P1wintext->size = 100;
		P1wintext->GetRectTransform()->SetSizeDelta({ 600, 120 });
		P2wintext->size = 100;
		P2wintext->GetRectTransform()->SetSizeDelta({ 600, 120 });

		P1sctext->SetFont(L"../Resources/Maplestory Light.ttf");
		P2sctext->SetFont(L"../Resources/Maplestory Light.ttf");
		Timetext->SetFont(L"../Resources/Maplestory Light.ttf");
		P1wintext->SetFont(L"../Resources/Maplestory Light.ttf");
		P2wintext->SetFont(L"../Resources/Maplestory Light.ttf");
		P1sctext->SetColor({ 0,0,0,255 });
		P2sctext->SetColor({ 0,0,0,255 });
		Timetext->SetColor({ 0,0,0,255 });
		P1wintext->SetColor({ 255,0,0,255 });
		P2wintext->SetColor({ 255,0,0,255 });
		auto TutorialObject = new GameObject;
		Tutorial = TutorialObject->AddComponent<TutorialImage>();

		srand(time(NULL));
		p1itemchange = rand() % 4 + 1;
		p2itemchange = rand() % 4 + 1;
		P1wintext->text = std::wstring(L"");
		P2wintext->text = std::wstring(L"");
	}
	else
	{
		Destroy(GetGameObject());
	}
	
}

void GOTOEngine::GameManager::Start()
{
	EnemySpawner = EnemySpawner::instance;
	FadeInOutFXManager::instance->FadeIn();
}

void GameManager::OnDestroy() {
	if (instance == this)
		instance = nullptr;

	EnemySpawner = nullptr;
}

void GameManager::Update() {
	if (setactive) {
		if(IsValidObject(Tutorial))
		{
			Destroy(Tutorial->GetGameObject());
			Tutorial = nullptr;
		}
		if (GameTimer > 0.0f) {
			GameTimer -= TIME_GET_DELTATIME();
			//*/ 디버깅용 주석
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
			if (GameTimer <= GoldTiming[0]) {
				//황금두더지 생성
				EnemySpawner->CreateEnemy(E_EnemyType::itemspawn, 3, 1);
				GoldTiming[0] = -1.0f;
			}
			if (GameTimer <= GoldTiming[1]) {
				//황금두더지 생성
				EnemySpawner->CreateEnemy(E_EnemyType::itemspawn, 3, 2);
				GoldTiming[1] = -1.0f;
			}
			if (GameTimer <= 0.0f|| INPUT_GET_KEYDOWN(KeyCode::Space)) {
				GameTimer = 0.0f;
			}
			//*/
		}
		else if (GameTimer == 0.0f) {
			setactive = false;
		}
	}
	else {
		if(GameTimer == 0.0f){
			endingTimer -= TIME_GET_DELTATIME();
			EnemySpawner->Setp1EnemyAllDestroy();
			EnemySpawner->Setp2EnemyAllDestroy();
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
				FadeInOutFXManager::instance->FadeOut();
				if (FadeInOutFXManager::instance->IsPerfectlyFadeOut()) {
					SCENE_CHANGE_SCENE(L"StartScene");
				}
			}
		}
		else {
			if (Tutorial)
			{
				p1active = Tutorial->GetButton1Timer() >= Tutorial->GetMaxButtonTimer();
				p2active = Tutorial->GetButton2Timer() >= Tutorial->GetMaxButtonTimer();
			}

			if (p1active && p2active && tutorialCheckTime == 0.0f) {
				tutorialCheckTime = TIME_GET_TOTALTIME() + 0.45f;
			}
			if (tutorialCheckTime != 0.0f && tutorialCheckTime < TIME_GET_TOTALTIME())
			{
				setactive = true;
			}
			//*/ 디버깅용 스페이스 바
			if (INPUT_GET_KEYDOWN(KeyCode::Space))
			{
				setactive = true;
			}
			//*/
		}
	}
	P1sctext->text = std::to_wstring(P1Score);
	P2sctext->text = std::to_wstring(P2Score);
	totalSeconds = static_cast<int>(floor(GameTimer));
	minutes = totalSeconds / 60;
	seconds = totalSeconds % 60;
	Timetext->text = std::to_wstring(minutes) + L":" + (seconds < 10 ? L"0" : L"") + std::to_wstring(seconds);
}

void GameManager::PointChange(int player, int point) {
	if (point > 0) {
		if (player == 1) {
			P1Score += point * P1Bonus;
		}
		else if (player == 2) {
			P2Score += point * P2Bonus;
		}
	}
	else {
		if (player == 1) {
			P1Score += point;
		}
		else if (player == 2) {
			P2Score += point;
		}
	}
}