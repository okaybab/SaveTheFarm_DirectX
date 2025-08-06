#include "GameManager2.h"
#include "TutorialImage2.h"
#include <AnimationCurve.h>
#include <time.h>

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
		warningImage->GetRectTransform()->SetAnchoredPosition({ Screen::GetWidth() * 0.25f, Screen::GetHeight() * 0.3f });
		warningImage->GetRectTransform()->SetSizeDelta({ Screen::GetWidth() * 0.5f, Screen::GetHeight() * 0.6f });
		warningImage->SetSprite(nullptr);
		warningsprite = Resource::Load<Sprite>(L"../Resources/artResource/UI/Warring/WARNNING_2.png");
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
	}
	else
	{
		Destroy(GetGameObject());
	}
}

void GOTOEngine::GameManager2::Start()
{
}

void GameManager2::OnDestroy() {
	if (instance == this)
		instance = nullptr;
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
			for (int i = 0; i < 3; ++i) {
				if (GameTimer <= waveTiming[i]) {
					wave = i + 1;
					waveTiming[i] = -1.0f;
				}
			}
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
			if ((GameTimer <= 142.0f && GameTimer > 140.0f) || (GameTimer <= 82.0f && GameTimer > 80.0f) || (GameTimer <= 22.0f && GameTimer > 20.0f)) {
				warningImage->SetSprite(warningsprite);
				warningAniTime += TIME_GET_DELTATIME();
				if (warningAniTime > 1.0f) {
					warningAniTime = 0.0f;
				}
				float animValue = warninganimation->Evaluate(warningAniTime);
				warningImage->GetRectTransform()->SetSizeDelta({ Screen::GetWidth() * 0.5f + animValue * 100.0f, Screen::GetHeight() * 0.6f + animValue * 100.0f });
			}
			else {
				warningImage->SetSprite(nullptr);
				warningImage->GetRectTransform()->SetSizeDelta({ Screen::GetWidth() * 0.5f, Screen::GetHeight() * 0.6f});
			}
			if (CropGauge == 0.0f) {
				GameTimer = 0.0f;
			}
			if (GameTimer <= 0.0f) {
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
			if (CropGauge > 0.0f) {
				winner = 1;
			}
			else {
				winner = 0;
			}
			if (endingTimer <= 0.0f) {
				SCENE_CHANGE_SCENE(L"StartScene");
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
	minutes = totalSeconds / 60;
	seconds = totalSeconds % 60;
	Timetext->text = std::to_wstring(minutes) + L":" + (seconds < 10 ? L"0" : L"") + std::to_wstring(seconds);
}