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

		P1sctext = p1sctextitem->AddComponent<Text>();
		P2sctext = p2sctextitem->AddComponent<Text>();
		Timetext = timeitem->AddComponent<Text>();

		p1sctextitem->GetTransform()->SetParent(canvas->GetTransform());
		p2sctextitem->GetTransform()->SetParent(canvas->GetTransform());
		timeitem->GetTransform()->SetParent(canvas->GetTransform());

		P1sctext->horizontalAlign = TextHoriAlign::Center;
		P2sctext->horizontalAlign = TextHoriAlign::Center;
		Timetext->horizontalAlign = TextHoriAlign::Center;

		P1sctext->GetRectTransform()->SetAnchoredPosition({
				Screen::GetWidth() * 0.315f, Screen::GetHeight() * 0.88f });
		P2sctext->GetRectTransform()->SetAnchoredPosition({
				Screen::GetWidth() * 0.635f, Screen::GetHeight() * 0.88f });
		Timetext->GetRectTransform()->SetAnchoredPosition({
				Screen::GetWidth() * 0.475f, Screen::GetHeight() * 0.88f });


		P1sctext->size = 43;
		P2sctext->size = 43;
		Timetext->size = 43;

		P1sctext->SetFont(L"../Resources/Maplestory Light.ttf");
		P2sctext->SetFont(L"../Resources/Maplestory Light.ttf");
		Timetext->SetFont(L"../Resources/Maplestory Light.ttf");
		P1sctext->SetColor({ 0,0,0,255 });
		P2sctext->SetColor({ 0,0,0,255 });
		Timetext->SetColor({ 0,0,0,255 });
		auto TutorialObject = new GameObject;
		Tutorial = TutorialObject->AddComponent<TutorialImage>();

		winpannel = Resource::Load<Sprite>(L"../Resources/artResource/UI/Endgame/경쟁 모드 게임 종료 UI/승리 UI.png");
		winpannel->IncreaseRefCount();
		losepannel = Resource::Load<Sprite>(L"../Resources/artResource/UI/Endgame/경쟁 모드 게임 종료 UI/패배 UI.png");
		losepannel->IncreaseRefCount();
		deucepannel = Resource::Load<Sprite>(L"../Resources/artResource/UI/Endgame/경쟁 모드 게임 종료 UI/무승부 UI.png");
		deucepannel->IncreaseRefCount();
		auto p1pannelobject = new GameObject;
		p1pannel = p1pannelobject->AddComponent<Image>();
		p1pannelobject->GetTransform()->SetParent(canvas->GetTransform());
		p1pannel->GetRectTransform()->SetAnchoredPosition({ Screen::GetWidth() * 0.1f, Screen::GetHeight() * 0.2f });
		p1pannel->GetRectTransform()->SetSizeDelta({ Screen::GetWidth() * 0.3f, Screen::GetHeight() * 0.7f });
		p1pannel->SetSprite(nullptr);
		auto p2pannelobject = new GameObject;
		p2pannel = p2pannelobject->AddComponent<Image>();
		p2pannelobject->GetTransform()->SetParent(canvas->GetTransform());
		p2pannel->GetRectTransform()->SetAnchoredPosition({ Screen::GetWidth() * 0.6f, Screen::GetHeight() * 0.2f });
		p2pannel->GetRectTransform()->SetSizeDelta({ Screen::GetWidth() * 0.3f, Screen::GetHeight() * 0.7f });
		p2pannel->SetSprite(nullptr);
		auto p1final1 = new GameObject;
		p1finalsctext = p1final1->AddComponent<Text>();
		p1final1->GetTransform()->SetParent(canvas->GetTransform());
		p1finalsctext->horizontalAlign = TextHoriAlign::Center;
		p1finalsctext->size = 70;
		p1finalsctext->SetFont(L"../Resources/Maplestory Light.ttf");
		p1finalsctext->SetColor({ 0,0,0,255 });
		p1finalsctext->GetRectTransform()->SetAnchoredPosition({
				Screen::GetWidth() * 0.294f, Screen::GetHeight() * 0.36f });
		auto p2final1 = new GameObject;
		p2finalsctext = p2final1->AddComponent<Text>();
		p2final1->GetTransform()->SetParent(canvas->GetTransform());
		p2finalsctext->horizontalAlign = TextHoriAlign::Center;
		p2finalsctext->size = 70;
		p2finalsctext->SetFont(L"../Resources/Maplestory Light.ttf");
		p2finalsctext->SetColor({ 0,0,0,255 });
		p2finalsctext->GetRectTransform()->SetAnchoredPosition({
				Screen::GetWidth() * 0.794f, Screen::GetHeight() * 0.36f });
		auto p1final2 = new GameObject;
		p1losttext = p1final2->AddComponent<Text>();
		p1final2->GetTransform()->SetParent(canvas->GetTransform());
		p1losttext->horizontalAlign = TextHoriAlign::Center;
		p1losttext->size = 70;
		p1losttext->SetFont(L"../Resources/Maplestory Light.ttf");
		p1losttext->SetColor({ 0,0,0,255 });
		p1losttext->GetRectTransform()->SetAnchoredPosition({
				Screen::GetWidth() * 0.294f, Screen::GetHeight() * 0.505f });
		auto p2final2 = new GameObject;
		p2losttext = p2final2->AddComponent<Text>();
		p2final2->GetTransform()->SetParent(canvas->GetTransform());
		p2losttext->horizontalAlign = TextHoriAlign::Center;
		p2losttext->size = 70;
		p2losttext->SetFont(L"../Resources/Maplestory Light.ttf");
		p2losttext->SetColor({ 0,0,0,255 });
		p2losttext->GetRectTransform()->SetAnchoredPosition({
				Screen::GetWidth() * 0.794f, Screen::GetHeight() * 0.505f });
		auto p1final3 = new GameObject;
		p1catchtext = p1final3->AddComponent<Text>();
		p1final3->GetTransform()->SetParent(canvas->GetTransform());
		p1catchtext->horizontalAlign = TextHoriAlign::Center;
		p1catchtext->size = 70;
		p1catchtext->SetFont(L"../Resources/Maplestory Light.ttf");
		p1catchtext->SetColor({ 0,0,0,255 });
		p1catchtext->GetRectTransform()->SetAnchoredPosition({
				Screen::GetWidth() * 0.294f, Screen::GetHeight() * 0.65f });
		auto p2final3 = new GameObject;
		p2catchtext = p2final3->AddComponent<Text>();
		p2final3->GetTransform()->SetParent(canvas->GetTransform());
		p2catchtext->horizontalAlign = TextHoriAlign::Center;
		p2catchtext->size = 70;
		p2catchtext->SetFont(L"../Resources/Maplestory Light.ttf");
		p2catchtext->SetColor({ 0,0,0,255 });
		p2catchtext->GetRectTransform()->SetAnchoredPosition({
				Screen::GetWidth() * 0.794f, Screen::GetHeight() * 0.65f });
		auto p1final4 = new GameObject;
		p1timetext = p1final4->AddComponent<Text>();
		p1final4->GetTransform()->SetParent(canvas->GetTransform());
		p1timetext->horizontalAlign = TextHoriAlign::Center;
		p1timetext->size = 40;
		p1timetext->SetFont(L"../Resources/Maplestory Light.ttf");
		p1timetext->SetColor({ 0,0,0,255 });
		p1timetext->GetRectTransform()->SetAnchoredPosition({
				Screen::GetWidth() * 0.13f, Screen::GetHeight() * 0.24f });
		auto p2final4 = new GameObject;
		p2timetext = p2final4->AddComponent<Text>();
		p2final4->GetTransform()->SetParent(canvas->GetTransform());
		p2timetext->horizontalAlign = TextHoriAlign::Center;
		p2timetext->size = 40;
		p2timetext->SetFont(L"../Resources/Maplestory Light.ttf");
		p2timetext->SetColor({ 0,0,0,255 });
		p2timetext->GetRectTransform()->SetAnchoredPosition({
				Screen::GetWidth() * 0.63f, Screen::GetHeight() * 0.24f });

		srand(time(NULL));
		p1itemchange = rand() % 4 + 1;
		p2itemchange = rand() % 4 + 1;
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
	if (IsValidObject(winpannel))
		winpannel->DecreaseRefCount();
	if (IsValidObject(losepannel))
		losepannel->DecreaseRefCount();
	if (IsValidObject(deucepannel))
		deucepannel->DecreaseRefCount();
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
				EnemySpawner->CreateEnemy(E_EnemyType::move, 1 << 1);
				EnemySpawner->CreateEnemy(E_EnemyType::move, 1 << 2);
				NormalTiming -= 5.0f;
			}
			if (GimmickTiming - GameTimer >= 15.0f) {
				//기믹 몬스터 랜덤 생성
				EnemySpawner->CreateEnemy(E_EnemyType::gimmick, 1 << 1);
				EnemySpawner->CreateEnemy(E_EnemyType::gimmick, 1 << 2);
				GimmickTiming -= 15.0f;
			}
			
			if (GameTimer <= ItemTiming[0]) {
				if (p1itemchange == 1 || p1itemchange == 4) {
					//p1 얼음새
					EnemySpawner->CreateEnemy(E_EnemyType::itemspawn, 0, 1 << 1);
				}
				else if (p1itemchange == 2 || p1itemchange == 3) {
					//p1 폭탄새
					EnemySpawner->CreateEnemy(E_EnemyType::itemspawn, 1, 1 << 1);
				}
				if (p2itemchange == 1 || p2itemchange == 4) {
					//p2 얼음새
					EnemySpawner->CreateEnemy(E_EnemyType::itemspawn, 0, 1 << 2);
				}
				else if (p2itemchange == 2 || p2itemchange == 3) {
					//p2 폭탄새
					EnemySpawner->CreateEnemy(E_EnemyType::itemspawn, 1, 1 << 2);
				}
				ItemTiming[0] = -1.0f;
			}
			if (GameTimer <= ItemTiming[1]) {
				if (p1itemchange == 1 || p1itemchange == 4) {
					//p1 폭탄새
				    EnemySpawner->CreateEnemy(E_EnemyType::itemspawn, 1, 1 << 1);
				}
				else if (p1itemchange == 2) {
					//p1 얼음새
					EnemySpawner->CreateEnemy(E_EnemyType::itemspawn, 0, 1 << 1);
				}
				else if (p1itemchange == 3) {
					//p1 황금새
					EnemySpawner->CreateEnemy(E_EnemyType::itemspawn, 2, 1 << 1);
				}
				if (p2itemchange == 1 || p2itemchange == 4) {
					//p2 폭탄새
					EnemySpawner->CreateEnemy(E_EnemyType::itemspawn, 1, 1 << 2);
				}
				else if (p2itemchange == 2) {
					//p2 얼음새
					EnemySpawner->CreateEnemy(E_EnemyType::itemspawn, 0, 1 << 2);
				}
				else if (p2itemchange == 3) {
					//p2 황금새
					EnemySpawner->CreateEnemy(E_EnemyType::itemspawn, 2, 1 << 2);
				}
				ItemTiming[1] = -1.0f;
			}
			if (GameTimer <= ItemTiming[2]) {
				if (p1itemchange == 1) {
					//p1 황금새
					EnemySpawner->CreateEnemy(E_EnemyType::itemspawn, 2, 1 << 1);
				}
				else if (p1itemchange == 2 || p1itemchange == 4) {
					//p1 폭탄새
					EnemySpawner->CreateEnemy(E_EnemyType::itemspawn, 1, 1 << 1);
				}
				else if (p1itemchange == 3) {
					//p1 얼음새
					EnemySpawner->CreateEnemy(E_EnemyType::itemspawn, 0, 1 << 1);
				}
				if (p2itemchange == 1) {
					//p2 황금새
					EnemySpawner->CreateEnemy(E_EnemyType::itemspawn, 2, 1 << 2);
				}
				else if (p2itemchange == 2 || p2itemchange == 4) {
					//p2 폭탄새
					EnemySpawner->CreateEnemy(E_EnemyType::itemspawn, 1, 1 << 2);
				}
				else if (p2itemchange == 3) {
					//p2 얼음새
					EnemySpawner->CreateEnemy(E_EnemyType::itemspawn, 0, 1 << 2);
				}
				ItemTiming[2] = -1.0f;
			}
			if (GameTimer <= ItemTiming[3]) {
				if (p1itemchange == 1 || p1itemchange == 3) {
					//p1 폭탄새
					EnemySpawner->CreateEnemy(E_EnemyType::itemspawn, 1, 1 << 1);
				}
				else if (p1itemchange == 2 || p1itemchange == 4) {
					//p1 황금새
					EnemySpawner->CreateEnemy(E_EnemyType::itemspawn, 2, 1 << 1);
				}
				if (p2itemchange == 1 || p2itemchange == 3) {
					//p2 폭탄새
					EnemySpawner->CreateEnemy(E_EnemyType::itemspawn, 1, 1 << 2);
				}
				else if (p2itemchange == 2 || p2itemchange == 4) {
					//p2 황금새
					EnemySpawner->CreateEnemy(E_EnemyType::itemspawn, 2, 1 << 2);
				}
				ItemTiming[3] = -1.0f;
			}
			if (GameTimer <= ItemTiming[4]) {
				if (p1itemchange == 1 || p1itemchange == 4) {
					//p1 얼음새
					EnemySpawner->CreateEnemy(E_EnemyType::itemspawn, 0, 1 << 1);
				}
				else if (p1itemchange == 2 || p1itemchange == 3) {
					//p1 폭탄새
					EnemySpawner->CreateEnemy(E_EnemyType::itemspawn, 1, 1 << 1);
				}
				if (p2itemchange == 1 || p2itemchange == 4) {
					//p2 얼음새
					EnemySpawner->CreateEnemy(E_EnemyType::itemspawn, 0, 1 << 2);
				}
				else if (p2itemchange == 2 || p2itemchange == 3) {
					//p2 폭탄새
					EnemySpawner->CreateEnemy(E_EnemyType::itemspawn, 1, 1 << 2);
				}
				ItemTiming[4] = -1.0f;
			}
			if (GameTimer <= ItemTiming[5]) {
				if (p1itemchange == 1 || p1itemchange == 4) {
					//p1 폭탄새
					EnemySpawner->CreateEnemy(E_EnemyType::itemspawn, 1, 1 << 1);
				}
				else if (p1itemchange == 2 || p1itemchange == 3) {
					//p1 얼음새
					EnemySpawner->CreateEnemy(E_EnemyType::itemspawn, 0, 1 << 1);
				}
				if (p2itemchange == 1 || p2itemchange == 4) {
					//p2 폭탄새
					EnemySpawner->CreateEnemy(E_EnemyType::itemspawn, 1, 1 << 2);
				}
				else if (p2itemchange == 2 || p2itemchange == 3) {
					//p2 얼음새
					EnemySpawner->CreateEnemy(E_EnemyType::itemspawn, 0, 1 << 2);
				}
				ItemTiming[5] = -1.0f;
			}
			if (GameTimer <= GoldTiming[0]) {
				//황금두더지 생성
				EnemySpawner->CreateGoleMole();
				GoldTiming[0] = -1.0f;
			}
			if (GameTimer <= GoldTiming[1]) {
				//황금두더지 생성
				EnemySpawner->CreateGoleMole();
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
			EnemySpawner->DestroyGoldMole();
			p1finalsctext->text = std::to_wstring(P1Score);
			p2finalsctext->text = std::to_wstring(P2Score);
			p1losttext->text = std::to_wstring(P1Lost);
			p2losttext->text = std::to_wstring(P2Lost);
			p1catchtext->text = std::to_wstring(P1Catch);
			p2catchtext->text = std::to_wstring(P2Catch);
			p1timetext->text = std::to_wstring(static_cast<int>(endingTimer)+1);
			p2timetext->text = std::to_wstring(static_cast<int>(endingTimer)+1);
			if (P1Score > P2Score) {
				winner = 1;
				p1pannel->SetSprite(winpannel);
				p2pannel->SetSprite(losepannel);
			}
			else if (P1Score < P2Score) {
				winner = 2;
				p2pannel->SetSprite(winpannel);
				p1pannel->SetSprite(losepannel);
			}
			else {
				winner = 0;
				p1pannel->SetSprite(deucepannel);
				p2pannel->SetSprite(deucepannel);
			}
			if (endingTimer <= 0.0f) {
				p1timetext->text = std::to_wstring(0);
				p2timetext->text = std::to_wstring(0);
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