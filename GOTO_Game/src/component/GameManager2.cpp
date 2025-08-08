#include "GameManager2.h"
#include "TutorialImage2.h"

#include "FadeInOutFXManager.h"
#include <AnimationCurve.h>
#include <time.h>
#include <Texture2D.h>

using namespace GOTOEngine;
GameManager2* GameManager2::instance = nullptr;

void GameManager2::Awake() {
	if (!instance)
	{
		instance = this;
		auto canvas = GameObject::Find(L"Canvas");

		srand(time(NULL));
		itemchange = rand() % 4 + 1;

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
		wavebarin->SetSprite(L"../Resources/artResource/UI/Ingame/Wave bar in.png");
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

		auto timeitem = new GameObject;
		timeitem->GetTransform()->SetParent(canvas->GetTransform());
		Timetext = timeitem->AddComponent<Text>();
		Timetext->horizontalAlign = TextHoriAlign::Center;
		Timetext->GetRectTransform()->SetAnchoredPosition({
				Screen::GetWidth() * 0.1f, Screen::GetHeight() * 0.88f });
		Timetext->size = 43;
		Timetext->SetColor({ 0,0,0,255 });
	}
	else
	{
		Destroy(GetGameObject());
	}
}

void GOTOEngine::GameManager2::Start()
{
	FadeInOutFXManager::instance->FadeIn();
}

void GameManager2::OnDestroy() {
	if (instance == this)
		instance = nullptr;
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
}

void GameManager2::Update() {
	if (setactive) {
		if (IsValidObject(Tutorial))
		{
			Destroy(Tutorial->GetGameObject());
			Tutorial = nullptr;
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
				waveTiming[0] = -1.0f;
			}
			if (GameTimer <= waveTiming[1]) {
				wave = 2;
				waveImage->SetSprite(wave2sprite);
				waveTiming[1] = -1.0f;
			}
			if (GameTimer <= waveTiming[2]) {
				wave = 3;
				waveImage->SetSprite(wave3sprite);
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
			if (GimmickTiming - GameTimer >= 6.0f) {
				if (!warningon) {
					if (wave == 1) {
						//1번 웨이브 기믹몬스터
					}
					else if (wave == 2) {
						//2번 웨이브 기믹몬스터
					}
					else if (wave == 3) {
						//3번 웨이브 기믹몬스터
					}
				}
				GimmickTiming -= 6.0f;
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
			if (GimmickWarningTiming - GameTimer >= 3.0f) {
				if (warningon) {
					if (wave == 1) {
						//1번 웨이브 기믹몬스터
					}
					else if (wave == 2) {
						//2번 웨이브 기믹몬스터
					}
					else if (wave == 3) {
						//3번 웨이브 기믹몬스터
					}
				}
				GimmickWarningTiming -= 3.0f;
			}
			if (GameTimer <= ItemTiming[0]) {
				if (itemchange == 1 || itemchange == 3) {
					//폭탄새
				}
				else if (itemchange == 2) {
					//얼음새
				}
				else if (itemchange == 4) {
					//버섯새
				}
				ItemTiming[0] = -1.0f;
			}
			if (GameTimer <= ItemTiming[1]) {
				if (itemchange == 2 || itemchange == 4) {
					//폭탄새
				}
				else if (itemchange == 1) {
					//버섯새
				}
				else if (itemchange == 3) {
					//얼음새
				}
				ItemTiming[1] = -1.0f;
			}
			if (GameTimer <= ItemTiming[2]) {
				if (itemchange == 3 || itemchange == 4) {
					//얼음새
				}
				else if (itemchange == 1) {
					//폭탄새
				}
				else if (itemchange == 2) {
					//버섯새
				}
				ItemTiming[2] = -1.0f;
			}
			if (GameTimer <= ItemTiming[3]) {
				if (itemchange == 2 || itemchange == 3 || itemchange == 4) {
					//폭탄새
				}
				else if (itemchange == 1) {
					//얼음새
				}
				ItemTiming[3] = -1.0f;
			}
			if (GameTimer <= ItemTiming[4]) {
				if (itemchange == 1 || itemchange == 3) {
					//버섯새
				}
				else if (itemchange == 2) {
					//얼음새
				}
				else if (itemchange == 4) {
					//폭탄새
				}
				ItemTiming[4] = -1.0f;
			}
			if (GameTimer <= ItemTiming[5]) {
				if (itemchange == 1 || itemchange == 2 || itemchange == 3) {
					//폭탄새
				}
				else if (itemchange == 4) {
					//얼음새
				}
				ItemTiming[5] = -1.0f;
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
			if (CropGauge == 0) {
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
			endingTimer -= TIME_GET_DELTATIME();
			if (CropGauge > 0 ) {
				winner = 1;
				endpannel->SetSprite(winpannel);
			}
			else {
				winner = 0;
				endpannel->SetSprite(losepannel);
			}
			if (endingTimer <= 0.0f) {
				FadeInOutFXManager::instance->FadeOut();
				if (FadeInOutFXManager::instance->IsPerfectlyFadeOut()) {
					SCENE_CHANGE_SCENE(L"StartScene");
				}
			}
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
	totalSeconds = static_cast<int>(floor(GameTimer));
	Timetext->text = std::to_wstring(totalSeconds);
}