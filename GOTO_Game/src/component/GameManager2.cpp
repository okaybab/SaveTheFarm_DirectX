#include "GameManager2.h"

using namespace GOTOEngine;
GameManager2* GameManager2::instance = nullptr;

void GameManager2::Awake() {
	if (!instance)
	{
		instance = this;
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
		
		if (GameTimer > 0.0f) {
			GameTimer -= TIME_GET_DELTATIME();
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
	totalSeconds = static_cast<int>(floor(GameTimer));
	minutes = totalSeconds / 60;
	seconds = totalSeconds % 60;
	Timetext->text = std::to_wstring(minutes) + L":" + (seconds < 10 ? L"0" : L"") + std::to_wstring(seconds);
}