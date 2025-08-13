#include "GameManager2.h"
#include "TutorialImage2.h"
#include "SoundManager.h"
#include "FadeInOutFXManager.h"
#include <AnimationCurve.h>
#include <Texture2D.h>
#include "DefenseModeCameraManager.h"
#include "CrosshairController.h"

#include "EnemySpawnManager.h"
#include "BaseSpawnerObject.h"

using namespace GOTOEngine;
GameManager2* GameManager2::instance = nullptr;

void GameManager2::Awake() {
	if (!instance)
	{
		instance = this;
		auto canvas = GameObject::Find(L"Canvas");

		auto TutorialObject = new GameObject;
		Tutorial = TutorialObject->AddComponent<TutorialImage2>();

		auto warningitem = new GameObject;
		warningitem->GetTransform()->SetParent(canvas->GetTransform());
		warningImage = warningitem->AddComponent<Image>();
		warningImage->GetRectTransform()->SetAnchoredPosition({ Screen::GetWidth()*0.5f, Screen::GetHeight()*0.5f });
		warningImage->GetRectTransform()->SetPivot({ 0.5f, 0.5f });
		warningImage->GetRectTransform()->SetSizeDelta({ Screen::GetWidth(), Screen::GetHeight()});
		warningImage->SetSprite(nullptr);
		warningsprite = Resource::Load<Sprite>(L"../Resources/artResource/UI/Warring/WARNING_2.png");
		warningsprite->IncreaseRefCount();
		warninganimation = new AnimationCurve({ R"({
     "keyframes": [
        {
            "time": 0.0,
            "value": 1.0,
            "in_tangent": 0.0,
            "out_tangent": 1.0,
            "tangent_mode": 1
        },
        {
            "time": 0.25,
            "value": 0.8,
            "in_tangent": 0.0,
            "out_tangent": 0.0,
            "tangent_mode": 0
        },
        {
            "time": 0.5,
            "value": 1.0,
            "in_tangent": 0.0,
            "out_tangent": 0.0,
            "tangent_mode": 1
        },
        {
            "time": 0.75,
            "value": 0.8,
            "in_tangent": 0.0,
            "out_tangent": 0.0,
            "tangent_mode": 1
        },
        {
            "time": 1.0,
            "value": 1.0,
            "in_tangent": 1.0,
            "out_tangent": 0.0,
            "tangent_mode": 1
        }
    ]
})" });
		auto waveobject = new GameObject;
		waveobject->GetTransform()->SetParent(canvas->GetTransform());
		waveImage = waveobject->AddComponent<Image>();
		waveImage->GetRectTransform()->SetAnchoredPosition({ Screen::GetWidth() *0.5f, Screen::GetHeight()*0.95f });
		waveImage->GetRectTransform()->SetPivot({ 0.5f, 0.5f });
		waveImage->GetRectTransform()->SetSizeDelta({ Screen::GetWidth()*0.1f, Screen::GetHeight()*0.1f });
		wave1sprite = Resource::Load<Sprite>(L"../Resources/artResource/UI/Wave/Wave1.png");
		wave1sprite->IncreaseRefCount();
		wave2sprite = Resource::Load<Sprite>(L"../Resources/artResource/UI/Wave/Wave2.png");
		wave2sprite->IncreaseRefCount();
		wave3sprite = Resource::Load<Sprite>(L"../Resources/artResource/UI/Wave/Wave3.png");
		wave3sprite->IncreaseRefCount();
		waveanimation = new AnimationCurve({ R"({
     "keyframes": [
        {
            "time": 0,
            "value": 0,
            "in_tangent": 0,
            "out_tangent": 1,
            "tangent_mode": 1
        },
        {
            "time": 0.5,
            "value": 2.0,
            "in_tangent": 0.0,
            "out_tangent": 0.0,
            "tangent_mode": 1
        },
        {
            "time": 1.5,
            "value": 2.0,
            "in_tangent": 0.0,
            "out_tangent": 0.0,
            "tangent_mode": 1
        },
        {
            "time": 2.0,
            "value": 0.05,
            "in_tangent": 1.0,
            "out_tangent": 0.0,
            "tangent_mode": 1
        }
    ]
})" });
		auto pannelobject = new GameObject;
		pannelobject->GetTransform()->SetParent(canvas->GetTransform());
		endpannel = pannelobject->AddComponent<Image>();
		endpannel->GetRectTransform()->SetSizeDelta({ Screen::GetWidth() * 0.3f, Screen::GetHeight() * 0.7f });
		endpannel->GetRectTransform()->SetPivot({ 0.5f,0.5f });
		endpannel->GetRectTransform()->SetAnchoredPosition({ Screen::GetWidth() * 0.5f, Screen::GetHeight() * 0.55f });
		endpannel->SetSprite(nullptr);
		winpannel = Resource::Load<Sprite>(L"../Resources/artResource/UI/Endgame/협동 모드 게임 종료 UI/승리! UI 창.png");
		winpannel->IncreaseRefCount();
		losepannel = Resource::Load<Sprite>(L"../Resources/artResource/UI/Endgame/협동 모드 게임 종료 UI/게임 오버 UI 창.png");
		losepannel->IncreaseRefCount();
		auto catchobject = new GameObject;
		catchtext = catchobject->AddComponent<Text>();
		catchobject->GetTransform()->SetParent(canvas->GetTransform());
		catchtext->horizontalAlign = TextHoriAlign::Center;
		catchtext->GetRectTransform()->SetPivot({ 0.5f,0.5f });
		catchtext->size = 60;
		catchtext->SetFont(L"../Resources/Maplestory Light.ttf");
		catchtext->SetColor({ 0,0,0,255 });
		catchtext->GetRectTransform()->SetAnchoredPosition({
				Screen::GetWidth() * 0.57f, Screen::GetHeight() * 0.67f });
		catchtext->GetRectTransform()->SetSizeDelta({ 200,100 });
		auto croptextobject = new GameObject;
		croptext = croptextobject->AddComponent<Text>();
		croptextobject->GetTransform()->SetParent(canvas->GetTransform());
		croptext->horizontalAlign = TextHoriAlign::Center;
		croptext->GetRectTransform()->SetPivot({ 0.5f,0.5f });
		croptext->size = 60;
		croptext->SetFont(L"../Resources/Maplestory Light.ttf");
		croptext->SetColor({ 0,0,0,255 });
		croptext->GetRectTransform()->SetAnchoredPosition({
				Screen::GetWidth() * 0.57f, Screen::GetHeight() * 0.56f });
		croptext->GetRectTransform()->SetSizeDelta({ 200,100 });
		auto focusobject = new GameObject;
		focusobject->GetTransform()->SetParent(canvas->GetTransform());
		focus = focusobject->AddComponent<Image>();
		focus->GetRectTransform()->SetSizeDelta({ Screen::GetWidth() * 0.25f, Screen::GetHeight() * 0.1f });
		focus->GetRectTransform()->SetPivot({ 0.5f,0.5f });
		focus->GetRectTransform()->SetAnchoredPosition({ Screen::GetWidth() * 0.5f, Screen::GetHeight() * 0.45f });
		focus->SetSprite(nullptr);
		focusui = Resource::Load<Sprite>(L"../Resources/Demo/FocusUI.png");
		focusui->IncreaseRefCount();
		auto retryobject = new GameObject;
		retryobject->GetTransform()->SetParent(canvas->GetTransform());
		retry = retryobject->AddComponent<Image>();
		retry->GetRectTransform()->SetSizeDelta({ Screen::GetWidth() * 0.2f, Screen::GetHeight() * 0.1f });
		retry->GetRectTransform()->SetPivot({ 0.5f,0.5f });
		retry->GetRectTransform()->SetAnchoredPosition({ Screen::GetWidth() * 0.5f, Screen::GetHeight() * 0.45f });
		retry->SetSprite(nullptr);
		retrybutton = Resource::Load<Sprite>(L"../Resources/artResource/UI/Endgame/협동 모드 게임 종료 UI/다시하기.png");
		retrybutton->IncreaseRefCount();
		auto totitleobject = new GameObject;
		totitleobject->GetTransform()->SetParent(canvas->GetTransform());
		totitle = totitleobject->AddComponent<Image>();
		totitle->GetRectTransform()->SetSizeDelta({ Screen::GetWidth() * 0.2f, Screen::GetHeight() * 0.1f });
		totitle->GetRectTransform()->SetPivot({ 0.5f,0.5f });
		totitle->GetRectTransform()->SetAnchoredPosition({ Screen::GetWidth() * 0.5f, Screen::GetHeight() * 0.32f });
		totitle->SetSprite(nullptr);
		totitlebutton = Resource::Load<Sprite>(L"../Resources/artResource/UI/Endgame/협동 모드 게임 종료 UI/타이틀로.png");
		totitlebutton->IncreaseRefCount();

		auto barobject1 = new GameObject;
		barobject1->GetTransform()->SetParent(canvas->GetTransform());
		wavebarout = barobject1->AddComponent<Image>();
		wavebarout->GetRectTransform()->SetSizeDelta({ Screen::GetWidth() * 0.4f, Screen::GetHeight() * 0.1f });
		wavebarout->GetRectTransform()->SetPivot({ 0.5f,0.5f });
		wavebarout->GetRectTransform()->SetAnchoredPosition({ Screen::GetWidth() * 0.5f, Screen::GetHeight() * 0.95f });
		wavebarout->SetSprite(L"../Resources/artResource/UI/Ingame/Wave bar out.png");
		auto barobject2 = new GameObject;
		barobject2->GetTransform()->SetParent(canvas->GetTransform());
		wavebarin = barobject2->AddComponent<Image>();
		wavebarin->GetRectTransform()->SetSizeDelta({ Screen::GetWidth() * 0.38f, Screen::GetHeight() * 0.08f });
		wavebarin->GetRectTransform()->SetPivot({ 1.0f,0.5f });
		wavebarin->GetRectTransform()->SetAnchoredPosition({ Screen::GetWidth() * 0.69f, Screen::GetHeight() * 0.95f });
		wave1bar = Resource::Load<Sprite>(L"../Resources/artResource/UI/Ingame/wave 1 bar in.png");
		wave1bar->IncreaseRefCount();
		wave2bar = Resource::Load<Sprite>(L"../Resources/artResource/UI/Ingame/wave 2 bar in.png");
		wave2bar->IncreaseRefCount();
		wave3bar = Resource::Load<Sprite>(L"../Resources/artResource/UI/Ingame/wave 3 bar in.png");
		wave3bar->IncreaseRefCount();
		wavebarin->SetSprite(L"../Resources/artResource/UI/Ingame/wave 1 bar in.png");
		texRect = wavebarin->GetSprite()->GetTexture()->GetRect();
		auto wave1checkobject = new GameObject;
		wave1checkobject->GetTransform()->SetParent(canvas->GetTransform());
		wave1check = wave1checkobject->AddComponent<Image>();
		wave1check->GetRectTransform()->SetSizeDelta({ Screen::GetWidth() * 0.05f, Screen::GetHeight() * 0.1f });
		wave1check->GetRectTransform()->SetPivot({ 0.5f,0.5f });
		wave1check->GetRectTransform()->SetAnchoredPosition({ Screen::GetWidth() * 0.32f, Screen::GetHeight() * 0.95f });
		wave1check->SetSprite(L"../Resources/artResource/UI/Ingame/Wave 1 UI.png");
		auto wave2checkobject = new GameObject;
		wave2checkobject->GetTransform()->SetParent(canvas->GetTransform());
		wave2check = wave2checkobject->AddComponent<Image>();
		wave2check->GetRectTransform()->SetSizeDelta({ Screen::GetWidth() * 0.05f, Screen::GetHeight() * 0.1f });
		wave2check->GetRectTransform()->SetPivot({ 0.5f,0.5f });
		wave2check->GetRectTransform()->SetAnchoredPosition({ Screen::GetWidth() * 0.45f, Screen::GetHeight() * 0.95f });
		wave2check->SetSprite(L"../Resources/artResource/UI/Ingame/Wave 2 UI.png");
		auto wave3checkobject = new GameObject;
		wave3checkobject->GetTransform()->SetParent(canvas->GetTransform());
		wave3check = wave3checkobject->AddComponent<Image>();
		wave3check->GetRectTransform()->SetSizeDelta({ Screen::GetWidth() * 0.05f, Screen::GetHeight() * 0.1f });
		wave3check->GetRectTransform()->SetPivot({ 0.5f,0.5f });
		wave3check->GetRectTransform()->SetAnchoredPosition({ Screen::GetWidth() * 0.58f, Screen::GetHeight() * 0.95f });
		wave3check->SetSprite(L"../Resources/artResource/UI/Ingame/Wave 3 UI.png");

		auto crop1object = new GameObject;
		crop1 = crop1object->AddComponent<SpriteRenderer>();
		crop1->GetTransform()->SetPosition({Screen::GetWidth()*(-0.3f), Screen::GetHeight()*(-0.35f)});
		crop1->GetTransform()->SetLocalScale({1.0f,1.0f});
		crop1->SetSprite(L"../Resources/artResource/Sprint/Vegetable1/Vegetable1_4.png");
		crop1->SetRenderLayer(1 << 0);
		crop14 = Resource::Load<Sprite>(L"../Resources/artResource/Sprint/Vegetable1/Vegetable1_4.png");
		crop14->IncreaseRefCount();
		crop13 = Resource::Load<Sprite>(L"../Resources/artResource/Sprint/Vegetable1/Vegetable1_3.png");
		crop13->IncreaseRefCount();
		crop12 = Resource::Load<Sprite>(L"../Resources/artResource/Sprint/Vegetable1/Vegetable1_2.png");
		crop12->IncreaseRefCount();
		crop11 = Resource::Load<Sprite>(L"../Resources/artResource/Sprint/Vegetable1/Vegetable1_1.png");
		crop11->IncreaseRefCount();
		auto crop2object = new GameObject;
		crop2 = crop2object->AddComponent<SpriteRenderer>();
		crop2->GetTransform()->SetPosition({ Screen::GetWidth() * 0.3f, Screen::GetHeight() * (-0.35f) });
		crop2->GetTransform()->SetLocalScale({1.0f,1.0f});
		crop2->SetSprite(L"../Resources/artResource/Sprint/Vegetable2/Vegetable2_4.png");
		crop2->SetRenderLayer(1 << 0);
		crop24 = Resource::Load<Sprite>(L"../Resources/artResource/Sprint/Vegetable2/Vegetable2_4.png");
		crop24->IncreaseRefCount();
		crop23 = Resource::Load<Sprite>(L"../Resources/artResource/Sprint/Vegetable2/Vegetable2_3.png");
		crop23->IncreaseRefCount();
		crop22 = Resource::Load<Sprite>(L"../Resources/artResource/Sprint/Vegetable2/Vegetable2_2.png");
		crop22->IncreaseRefCount();
		crop21 = Resource::Load<Sprite>(L"../Resources/artResource/Sprint/Vegetable2/Vegetable2_1.png");
		crop21->IncreaseRefCount();

		auto p1 = GameObject::Find(L"Player1");
		auto p2 = GameObject::Find(L"Player2");
		p1->GetComponent<CrosshairController>()->ChangeType(CrosshairType::HoldingGun);
		p2->GetComponent<CrosshairController>()->ChangeType(CrosshairType::HoldingGun);
	}
	else
	{
		Destroy(GetGameObject());
	}
}

void GOTOEngine::GameManager2::Start()
{
	EnemySpawnManager = EnemySpawnManager::instance;
	FadeInOutFXManager::instance->FadeIn();
}

void GameManager2::OnDestroy() {
	if (instance == this)
		instance = nullptr;
	EnemySpawnManager = nullptr;
	delete(warninganimation);
	if (IsValidObject(warningsprite))
		warningsprite->DecreaseRefCount();
	delete(waveanimation);
	if (IsValidObject(wave1sprite))
		wave1sprite->DecreaseRefCount();
	if (IsValidObject(wave2sprite))
		wave2sprite->DecreaseRefCount();
	if (IsValidObject(wave3sprite))
		wave3sprite->DecreaseRefCount();
	if (IsValidObject(winpannel))
		winpannel->DecreaseRefCount();
	if (IsValidObject(losepannel))
		losepannel->DecreaseRefCount();
	if (IsValidObject(wave1bar))
		wave1bar->DecreaseRefCount();
	if (IsValidObject(wave2bar))
		wave2bar->DecreaseRefCount();
	if (IsValidObject(wave3bar))
		wave3bar->DecreaseRefCount();
	if (IsValidObject(crop14))
		crop14->DecreaseRefCount();
	if (IsValidObject(crop13))
		crop13->DecreaseRefCount();
	if (IsValidObject(crop12))
		crop12->DecreaseRefCount();
	if (IsValidObject(crop11))
		crop11->DecreaseRefCount();
	if (IsValidObject(crop24))
		crop24->DecreaseRefCount();
	if (IsValidObject(crop23))
		crop23->DecreaseRefCount();
	if (IsValidObject(crop22))
		crop22->DecreaseRefCount();
	if (IsValidObject(crop21))
		crop21->DecreaseRefCount();
	if (IsValidObject(focusui))
		focusui->DecreaseRefCount();
	if (IsValidObject(retrybutton))
		retrybutton->DecreaseRefCount();
	if (IsValidObject(totitlebutton))
		totitlebutton->DecreaseRefCount();
}

void GameManager2::Update() {
	StickPressedCheckReset();
	StickPressedCheck();

	if (setactive) {
		if (IsValidObject(Tutorial))
		{
			Destroy(Tutorial->GetGameObject());
			Tutorial = nullptr;
		}
		if (BGMTiming[1] == 120.0f) {
			SoundManager::instance->PlayBGM("Battle2");
			BGMTiming[1] = -1.0f;
		}
		if (GameTimer > 0.0f) {
			GameTimer -= TIME_GET_DELTATIME();
			if ((GameTimer <= 140.0f && GameTimer > 120.0f) || (GameTimer <= 80.0f && GameTimer > 60.0f) || (GameTimer <=20.0f && GameTimer> 0.0f)) {
				warningon = true;
			}
			else if ((GameTimer <= 180.0f && GameTimer > 140.0f) || (GameTimer <= 120.0f && GameTimer > 80.0f) || (GameTimer <= 60.0f && GameTimer > 20.0f)) {
				warningon = false;
			}

			if (GameTimer <= waveTiming[0]) {
				wave = 1;
				waveImage->SetSprite(wave1sprite);
				wavebarin->SetSprite(wave1bar);
				SoundManager::instance->PlaySFX("Round");
				waveTiming[0] = -1.0f;
			}
			if (GameTimer <= waveTiming[1]) {
				wave = 2;
				waveImage->SetSprite(wave2sprite);
				wavebarin->SetSprite(wave2bar);
				SoundManager::instance->PlaySFX("Round");
				DefenseModeCameraManager::instance->Close();
				waveTiming[1] = -1.0f;
			}
			if (GameTimer <= waveTiming[2]) {
				wave = 3;
				waveImage->SetSprite(wave3sprite);
				wavebarin->SetSprite(wave3bar);
				SoundManager::instance->PlaySFX("Round");
				DefenseModeCameraManager::instance->Close();
				waveTiming[2] = -1.0f;
			}
			if ((GameTimer <= 180.0f && GameTimer >= 178.0f) || (GameTimer <= 120.0f && GameTimer >= 118.0f) || (GameTimer <= 60.0f && GameTimer >= 58.0f)) {
				waveAniTime += TIME_GET_DELTATIME();
				float animValue = waveanimation->Evaluate(waveAniTime);
				waveImage->GetRectTransform()->SetAnchoredPosition({ Screen::GetWidth() * 0.5f, Screen::GetHeight() * 0.95f - animValue*50.0f});
			}
			else {
				waveAniTime = 0.0f;
				waveImage->GetRectTransform()->SetAnchoredPosition({ Screen::GetWidth() * 0.5f, Screen::GetHeight() * 0.95f});
			}

		    wavebarin->GetSprite()->SetRect({ texRect.width * (1 - (GameTimer / 180.0f)),0,texRect.width * (GameTimer / 180.0f),texRect.height });
			wavebarin->GetRectTransform()->SetSizeDelta({ Screen::GetWidth() * 0.38f * (GameTimer / 180.0f), Screen::GetHeight() * 0.08f });

			if (NormalTiming - GameTimer >= 2.0f) {
				if (!warningon) {
					if (wave == 1) {
						//1번 웨이브 일반몬스터
					}
					else if (wave == 2) {
						//2번 웨이브 일반몬스터
					}
					else if (wave == 3) {
						//3번 웨이브 일반몬스터
					}
				}
				NormalTiming -= 2.0f;
			}
			if (GimmickTiming - GameTimer >= 10.0f) {
				if (!warningon) {
					if (wave == 1) {
						//1번 웨이브 지상기믹몬스터
						EnemySpawnManager->CreateDefenseEnemey(1, E_Defense_Fly_Type::ground1, E_Defense_Gimmick_Type::defense_gimmick);
					}
					else if (wave == 2) {
						//2번 웨이브 지상기믹몬스터
						EnemySpawnManager->CreateDefenseEnemey(2, E_Defense_Fly_Type::ground1, E_Defense_Gimmick_Type::defense_gimmick);
					}
					else if (wave == 3) {
						//3번 웨이브 지상기믹몬스터
						EnemySpawnManager->CreateDefenseEnemey(3, E_Defense_Fly_Type::ground1, E_Defense_Gimmick_Type::defense_gimmick);
					}
				}
				GimmickTiming -= 10.0f;
			}
			if (Gimmick2Timing - GameTimer >= 15.0f) {
				if (!warningon) {
					if (wave == 1) {
						//1번 웨이브 공중기믹몬스터
						EnemySpawnManager->CreateDefenseEnemey(1, E_Defense_Fly_Type::fly, E_Defense_Gimmick_Type::defense_gimmick);
					}
					else if (wave == 2) {
						//2번 웨이브 공중기믹몬스터
						EnemySpawnManager->CreateDefenseEnemey(2, E_Defense_Fly_Type::fly, E_Defense_Gimmick_Type::defense_gimmick);
					}
					else if (wave == 3) {
						//3번 웨이브 공중기믹몬스터
						EnemySpawnManager->CreateDefenseEnemey(3, E_Defense_Fly_Type::fly, E_Defense_Gimmick_Type::defense_gimmick);
					}
				}
				Gimmick2Timing -= 15.0f;
			}
			if (NormalWarningTiming - GameTimer >= 1.0f) {
				if (warningon) {
					if (wave == 1) {
						//1번 웨이브 일반몬스터
					}
					else if (wave == 2) {
						//2번 웨이브 일반몬스터
					}
					else if (wave == 3) {
						//3번 웨이브 일반몬스터
					}
				}
				NormalWarningTiming -= 1.0f;
			}
			if (GimmickWarningTiming - GameTimer >= 8.0f) {
				if (warningon) {
					if (wave == 1) {
						//1번 웨이브 지상기믹몬스터
						EnemySpawnManager->CreateDefenseEnemey(1, E_Defense_Fly_Type::ground1, E_Defense_Gimmick_Type::defense_gimmick);
					}
					else if (wave == 2) {
						//2번 웨이브 지상기믹몬스터
						EnemySpawnManager->CreateDefenseEnemey(2, E_Defense_Fly_Type::ground1, E_Defense_Gimmick_Type::defense_gimmick);
					}
					else if (wave == 3) {
						//3번 웨이브 지상기믹몬스터
						EnemySpawnManager->CreateDefenseEnemey(1, E_Defense_Fly_Type::ground1, E_Defense_Gimmick_Type::defense_gimmick);
					}
				}
				GimmickWarningTiming -= 8.0f;
			}
			if (Gimmick2WarningTiming - GameTimer >= 10.0f) {
				if (warningon) {
					if (wave == 1) {
						//1번 웨이브 공중기믹몬스터
						EnemySpawnManager->CreateDefenseEnemey(1, E_Defense_Fly_Type::fly, E_Defense_Gimmick_Type::defense_gimmick);
					}
					else if (wave == 2) {
						//2번 웨이브 공중기믹몬스터
						EnemySpawnManager->CreateDefenseEnemey(2, E_Defense_Fly_Type::fly, E_Defense_Gimmick_Type::defense_gimmick);
					}
					else if (wave == 3) {
						//3번 웨이브 공중기믹몬스터
						EnemySpawnManager->CreateDefenseEnemey(3, E_Defense_Fly_Type::fly, E_Defense_Gimmick_Type::defense_gimmick);
					}
				}
				Gimmick2WarningTiming -= 10.0f;
			}

			if ((GameTimer <= 141.0f && GameTimer > 140.0f) || (GameTimer <= 81.0f && GameTimer > 80.0f) || (GameTimer <= 21.0f && GameTimer > 20.0f)) {
				warningImage->SetSprite(warningsprite);
				warningAniTime += TIME_GET_DELTATIME();
				if (warningAniTime > 1.0f) {
					warningAniTime = 0.0f;
				}
				float animValue = warninganimation->Evaluate(warningAniTime);
				warningImage->GetRectTransform()->SetSizeDelta({ Screen::GetWidth() + animValue * 100.0f, Screen::GetHeight() + animValue * 100.0f });
			}
			else {
				warningImage->SetSprite(nullptr);
				warningImage->GetRectTransform()->SetSizeDelta({ Screen::GetWidth(), Screen::GetHeight()});
			}
			for (int i = 0; i < 3; ++i) {
				if (GameTimer <= warningsoundTiming[i]) {
					SoundManager::instance->PlaySFX("Warning");
					DefenseModeCameraManager::instance->Open();
					warningsoundTiming[i] = -1.0f;
				}
			}
			if (CropGauge == 8 || CropGauge == 7) {
				crop1->SetSprite(crop14);
				crop2->SetSprite(crop24);
			}
			if (CropGauge == 6 || CropGauge == 5) {
				crop1->SetSprite(crop13);
				crop2->SetSprite(crop23);
			}
			if (CropGauge == 4 || CropGauge == 3) {
				crop1->SetSprite(crop12);
				crop2->SetSprite(crop22);
			}
			if (CropGauge == 2 || CropGauge == 1) {
				crop1->SetSprite(crop11);
				crop2->SetSprite(crop21);
			}
			if (CropGauge == 0) {
				crop1->SetSprite(nullptr);
				crop2->SetSprite(nullptr);
				GameTimer = 0.0f;
			}
			if (GameTimer <= 0.0f || INPUT_GET_KEYDOWN(KeyCode::Space)) {
				GameTimer = 0.0f;
			}
		}
		else if (GameTimer == 0.0f) {
			setactive = false;
		}
	}
	else {
		if (GameTimer == 0.0f) {
			if (CropGauge > 0 && !pannelopen) {
				winner = 1;
				if (BGMTiming[2] == 0.0f) {
					SoundManager::instance->StopBGM();
					SoundManager::instance->PlaySFX("GameWin");
					BGMTiming[2] = -1.0f;
				}
				focuschoice = 2;
				endpannel->SetSprite(winpannel);
				catchtext->text = std::to_wstring(animalcatch);
				croptext->text = std::to_wstring(8-CropGauge);
				pannelopen = true;
			}
			else if(CropGauge<=0 && !pannelopen) {
				winner = 0;
				if (BGMTiming[2] == 0.0f) {
					SoundManager::instance->StopBGM();
					SoundManager::instance->PlaySFX("GameOver");
					BGMTiming[2] = -1.0f;
				}
				focuschoice = 1;
				endpannel->SetSprite(losepannel);
				pannelopen = true;
			}
			focus->SetSprite(focusui);
			retry->SetSprite(retrybutton);
			totitle->SetSprite(totitlebutton);
			if ((INPUT_GET_KEYDOWN(KeyCode::DownArrow) || INPUT_GET_KEYDOWN(KeyCode::F) || m_YstickPressedDown[0]
				|| m_YstickPressedDown[1]) && focuschoice == 1) {
				SoundManager::instance->PlaySFX("Button");
				focuschoice = 2;
			}
			if ((INPUT_GET_KEYDOWN(KeyCode::UpArrow) || INPUT_GET_KEYDOWN(KeyCode::R) || m_YstickPressedUp[0]
				|| m_YstickPressedUp[1]) && focuschoice == 2) {
				SoundManager::instance->PlaySFX("Button");
				focuschoice = 1;
			}
			if (focuschoice == 1) {
				focus->GetRectTransform()->SetAnchoredPosition({ Screen::GetWidth() * 0.5f, Screen::GetHeight() * 0.45f });
			}
			else if (focuschoice == 2) {
				focus->GetRectTransform()->SetAnchoredPosition({ Screen::GetWidth() * 0.5f, Screen::GetHeight() * 0.32f });
			}
			if ((INPUT_GET_KEYDOWN(KeyCode::LeftShift) || INPUT_GET_KEYDOWN(KeyCode::RightShift) || INPUT_GET_GAMEPAD_BUTTONDOWN(0, GamepadButton::ButtonSouth)
				|| INPUT_GET_GAMEPAD_BUTTONDOWN(1, GamepadButton::ButtonSouth)
				|| INPUT_GET_GAMEPAD_BUTTONDOWN(0, GamepadButton::ButtonR1)
				|| INPUT_GET_GAMEPAD_BUTTONDOWN(1, GamepadButton::ButtonR1)) && focuschoice == 1) {
				SoundManager::instance->PlaySFX("Button");
				FadeInOutFXManager::instance->FadeOut();
				reset = true;
			}
			if ((INPUT_GET_KEYDOWN(KeyCode::LeftShift) || INPUT_GET_KEYDOWN(KeyCode::RightShift) || INPUT_GET_GAMEPAD_BUTTONDOWN(0, GamepadButton::ButtonSouth)
				|| INPUT_GET_GAMEPAD_BUTTONDOWN(1, GamepadButton::ButtonSouth)
				|| INPUT_GET_GAMEPAD_BUTTONDOWN(0, GamepadButton::ButtonR1)
				|| INPUT_GET_GAMEPAD_BUTTONDOWN(1, GamepadButton::ButtonR1)) && focuschoice == 2) {
				SoundManager::instance->PlaySFX("Button");
				FadeInOutFXManager::instance->FadeOut();
				changestart = true;
			}
			if (FadeInOutFXManager::instance->IsPerfectlyFadeOut()&&reset) {
				SCENE_CHANGE_SCENE(L"DefenseScene");
			}
			if (FadeInOutFXManager::instance->IsPerfectlyFadeOut()&&changestart) {
				SCENE_CHANGE_SCENE(L"StartScene");
			}
		}
		else {
			if (BGMTiming[0] == 120.0f) {
				SoundManager::instance->PlayBGM("Tutorial");
				BGMTiming[0] = -1.0f;
			}
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
		}
	}
}

void GameManager2::OnSceneLoaded() {
	std::memset(m_YpressedUpTrigger, false, sizeof(m_YpressedUpTrigger));
	std::memset(m_YpressedDownTrigger, false, sizeof(m_YpressedDownTrigger));
	StickPressedCheckReset();
}

void GameManager2::StickPressedCheck()
{
	for (int i = 0; i < 2; i++)
	{
		auto currentYstick = INPUT_GET_GAMEPAD_AXIS(i, GamepadAxis::LeftStickY);
		if (!m_YpressedUpTrigger[i] && !m_YpressedDownTrigger[i])
		{
			if (currentYstick > 0.89f)
			{
				m_YpressedUpTrigger[i] = true;
				m_YstickPressedUp[i] = true;
				return;
			}

			if (currentYstick < -0.89f)
			{
				m_YpressedDownTrigger[i] = true;
				m_YstickPressedDown[i] = true;
				return;
			}
		}
		else if ((m_YpressedDownTrigger[i] && currentYstick > -0.2f)
			|| (m_YpressedUpTrigger[i] && currentYstick < 0.2f))
		{
			m_YpressedDownTrigger[i] = false;
			m_YpressedUpTrigger[i] = false;
		}
	}
}

void GameManager2::StickPressedCheckReset()
{
	std::memset(m_YstickPressedUp, false, sizeof(m_YstickPressedUp));
	std::memset(m_YstickPressedDown, false, sizeof(m_YstickPressedDown));
}