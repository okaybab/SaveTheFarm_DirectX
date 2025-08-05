#include "TutorialImage.h"
#include "SoundManager.h"

using namespace GOTOEngine;
//TutorialImage* TutorialImage::instance = nullptr;

void TutorialImage::Awake() {
	/*if (!instance)
	{
		instance = this;
	}
	else
	{
		Destroy(GetGameObject());
	}*/
	auto canvas = GameObject::Find(L"Canvas");
	auto expobject = new GameObject;
	Tutorialobject.push_back(expobject);
	expobject->GetTransform()->SetParent(canvas->GetTransform());
	explanation = expobject->AddComponent<Image>();
	explanation->GetRectTransform()->SetAnchoredPosition({
		Screen::GetWidth() * 0.2f, Screen::GetHeight() * 0.2f });
	explanation->GetRectTransform()->SetSizeDelta({ 
		Screen::GetWidth() * 0.6f, Screen::GetHeight() * 0.6f });
	auto p1bobject = new GameObject;
	Tutorialobject.push_back(p1bobject);
	p1bobject->GetTransform()->SetParent(canvas->GetTransform());
	p1button = p1bobject->AddComponent<Image>();
	p1button->GetRectTransform()->SetAnchoredPosition({
		Screen::GetWidth() * 0.25f, Screen::GetHeight() * 0.23f });
	p1button->GetRectTransform()->SetSizeDelta({
		Screen::GetWidth() * 0.021f, Screen::GetHeight() * 0.035f });
	auto p2bobject = new GameObject;
	Tutorialobject.push_back(p2bobject);
	p2bobject->GetTransform()->SetParent(canvas->GetTransform());
	p2button = p2bobject->AddComponent<Image>();
	p2button->GetRectTransform()->SetAnchoredPosition({
		Screen::GetWidth() * 0.72f, Screen::GetHeight() * 0.23f });
	p2button->GetRectTransform()->SetSizeDelta({
		Screen::GetWidth() * 0.021f, Screen::GetHeight() * 0.035f });
	p1button->SetSprite(L"../Resources/artResource/UI/Tutorial/OKButton_basic.png");
	p2button->SetSprite(L"../Resources/artResource/UI/Tutorial/OKButton_basic.png");
}

void TutorialImage::OnDestroy() {
	for (auto obj : Tutorialobject) {
		Destroy(obj);
		obj = nullptr;
	}
}

void TutorialImage::Update() {
	TriggerPressedCheckReset();
	TriggerPressedCheck();

	switch (explainnum) {
	case 1:
		explanation->SetSprite(L"../Resources/artResource/UI/Tutorial/Tutorial_1.png");
		if (INPUT_GET_KEYDOWN(KeyCode::RightShift) || 
			rightTriggerPressed[0] || 
			rightTriggerPressed[1]) {
			SoundManager::instance->PlaySFX("Button");
			explainnum = 2;
		}
		break;
	case 2:
		explanation->SetSprite(L"../Resources/artResource/UI/Tutorial/Tutorial_2.png");
		if (INPUT_GET_KEYDOWN(KeyCode::LeftShift) || 
			leftTriggerPressed[0] ||
			leftTriggerPressed[1]) {
			SoundManager::instance->PlaySFX("Button");
			explainnum = 1;
		}
		if (INPUT_GET_KEYDOWN(KeyCode::RightShift) ||
			rightTriggerPressed[0] ||
			rightTriggerPressed[1]) {
			SoundManager::instance->PlaySFX("Button");
			explainnum = 3;
		}
		break;
	case 3:
		explanation->SetSprite(L"../Resources/artResource/UI/Tutorial/Tutorial_3.png");
		if (INPUT_GET_KEYDOWN(KeyCode::LeftShift) ||
			leftTriggerPressed[0] ||
			leftTriggerPressed[1]) {
			SoundManager::instance->PlaySFX("Button");
			explainnum = 2;
		}
		if (INPUT_GET_KEYDOWN(KeyCode::RightShift) ||
			rightTriggerPressed[0] ||
			rightTriggerPressed[1]) {
			SoundManager::instance->PlaySFX("Button");
			explainnum = 4;
		}
		break;
	case 4:
		explanation->SetSprite(L"../Resources/artResource/UI/Tutorial/Tutorial_4.png");
		if (INPUT_GET_KEYDOWN(KeyCode::LeftShift) ||
			leftTriggerPressed[0] ||
			leftTriggerPressed[1]) {
			SoundManager::instance->PlaySFX("Button");
			explainnum = 3;
		}
		if (INPUT_GET_KEYDOWN(KeyCode::RightShift) ||
			rightTriggerPressed[0] ||
			rightTriggerPressed[1]) {
			SoundManager::instance->PlaySFX("Button");
			explainnum = 5;
		}
		break;
	case 5:
		explanation->SetSprite(L"../Resources/artResource/UI/Tutorial/Tutorial_5.png");
		if (INPUT_GET_KEYDOWN(KeyCode::LeftShift) ||
			leftTriggerPressed[0] ||
			leftTriggerPressed[1]) {
			SoundManager::instance->PlaySFX("Button");
			explainnum = 4;
		}
		break;
	}
	if (INPUT_GET_KEYDOWN(KeyCode::Alpha1) ||
		INPUT_GET_GAMEPAD_BUTTONDOWN(0,GamepadButton::ButtonWest)) {
		SoundManager::instance->PlaySFX("Button");
		p1button->SetSprite(L"../Resources/artResource/UI/Tutorial/OKButton_1.png");
	}
	if (INPUT_GET_KEYDOWN(KeyCode::Alpha0) ||
		INPUT_GET_GAMEPAD_BUTTONDOWN(1, GamepadButton::ButtonWest)) {
		SoundManager::instance->PlaySFX("Button");
		p2button->SetSprite(L"../Resources/artResource/UI/Tutorial/OKButton_2.png");
	}
}

void TutorialImage::TriggerPressedCheck()  {
	for (int i = 0; i < 2; i++) {
		auto currentRightTrigger = INPUT_GET_GAMEPAD_AXIS(i, GamepadAxis::RightTrigger);
		if (!rightTriggerCheckTrigger[i]) {
			if (currentRightTrigger > 0.89f) {
				rightTriggerCheckTrigger[i] = true;
				rightTriggerPressed[i] = true;
				return;
			}
		}
		else if ((rightTriggerCheckTrigger[i] && currentRightTrigger < 0.2f)) {
			rightTriggerCheckTrigger[i] = false;
		}
	}

	for (int i = 0; i < 2; i++) {
		auto currentLeftTrigger = INPUT_GET_GAMEPAD_AXIS(i, GamepadAxis::LeftTrigger);
		if (!leftTriggerCheckTrigger[i]) {
			if (currentLeftTrigger > 0.89f) {
				leftTriggerCheckTrigger[i] = true;
				leftTriggerPressed[i] = true;
				return;
			}
		}
		else if ((leftTriggerCheckTrigger[i] && currentLeftTrigger < 0.2f)) {
			leftTriggerCheckTrigger[i] = false;
		}
	}
}


void TutorialImage::TriggerPressedCheckReset() {
	rightTriggerPressed[0] = false;
	rightTriggerPressed[1] = false;
	leftTriggerPressed[0] = false;
	leftTriggerPressed[1] = false;
}