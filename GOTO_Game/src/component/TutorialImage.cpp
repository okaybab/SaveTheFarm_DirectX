#include "TutorialImage.h"
#include "SoundManager.h"
#include <AnimationCurve.h>

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

	buttonClickedCurve = new AnimationCurve(R"({
    "keyframes": [
        {
            "time": 0.0,
            "value": 1.0,
            "in_tangent": 0.0,
            "out_tangent": 0.0,
            "tangent_mode": 1
        },
        {
            "time": 0.075,
            "value": 0.6275,
            "in_tangent": -0.35789550789593677,
            "out_tangent": -0.35789550789593677,
            "tangent_mode": 1
        },
        {
            "time": 0.155,
            "value": 1.1775,
            "in_tangent": 0,
            "out_tangent": 0,
            "tangent_mode": 1
        },
        {
            "time": 0.215,
            "value": 0.8775,
            "in_tangent": 0,
            "out_tangent": 0,
            "tangent_mode": 1
        },
        {
            "time": 0.275,
            "value": 1.0925,
            "in_tangent": -0.3499999999999993,
            "out_tangent": -0.3499999999999993,
            "tangent_mode": 1
        },
        {
            "time": 0.355,
            "value": 0.9975,
            "in_tangent": 0,
            "out_tangent": 0,
            "tangent_mode": 1
        },
        {
            "time": 0.93,
            "value": 1.0,
            "in_tangent": 0.0,
            "out_tangent": 0.0,
            "tangent_mode": 1
        }
    ]
})");


	buttonSizeDelta = { 55.0f, 55.0f };

	auto canvas = GameObject::Find(L"Canvas");
	auto expobject = new GameObject;
	Tutorialobject.push_back(expobject);
	expobject->GetTransform()->SetParent(canvas->GetTransform());
	explanation = expobject->AddComponent<Image>();
	explanation->GetRectTransform()->SetAnchoredPosition({
		Screen::GetWidth() * 0.2f, Screen::GetHeight() * 0.2f });
	explanation->GetRectTransform()->SetSizeDelta({ 
		Screen::GetWidth() * 0.6f, Screen::GetHeight() * 0.6f });
	auto p1bBackObject = new GameObject;
	Tutorialobject.push_back(p1bBackObject);
	p1bBackObject->GetTransform()->SetParent(canvas->GetTransform());
	p1buttonBack = p1bBackObject->AddComponent<Image>();
	p1buttonBack->GetRectTransform()->SetAnchoredPosition({
		Screen::GetWidth() * 0.25f + (55.0f * 0.5f) , Screen::GetHeight() * 0.23f + (55.0f * 0.5f)});
	p1buttonBack->GetRectTransform()->SetSizeDelta(buttonSizeDelta);
	auto p1bobject = new GameObject;
	Tutorialobject.push_back(p1bobject);
	p1bobject->GetTransform()->SetParent(canvas->GetTransform());
	p1button = p1bobject->AddComponent<Image>();
	p1button->GetRectTransform()->SetAnchoredPosition({
		Screen::GetWidth() * 0.25f, Screen::GetHeight() * 0.23f });
	p1button->GetRectTransform()->SetSizeDelta(buttonSizeDelta);
	p1button->SetSortOrder(1);
	auto p2bBackObject = new GameObject;
	Tutorialobject.push_back(p2bBackObject);
	p2bBackObject->GetTransform()->SetParent(canvas->GetTransform());
	p2buttonBack = p2bBackObject->AddComponent<Image>();
	p2buttonBack->GetRectTransform()->SetAnchoredPosition({
		Screen::GetWidth() * 0.72f + (55.0f * 0.5f), Screen::GetHeight() * 0.23f + (55.0f * 0.5f) });
	p2buttonBack->GetRectTransform()->SetSizeDelta(buttonSizeDelta);
	auto p2bobject = new GameObject;
	Tutorialobject.push_back(p2bobject);
	p2bobject->GetTransform()->SetParent(canvas->GetTransform());
	p2button = p2bobject->AddComponent<Image>();
	p2button->GetRectTransform()->SetAnchoredPosition({
		Screen::GetWidth() * 0.72f, Screen::GetHeight() * 0.23f });
	p2button->GetRectTransform()->SetSizeDelta(buttonSizeDelta);
	p1button->SetSprite(L"../Resources/artResource/UI/Tutorial/OKButton_basic.png");
	p2button->SetSprite(L"../Resources/artResource/UI/Tutorial/OKButton_basic.png");
	p2button->SetSortOrder(1);

	p1buttonBack->SetSprite(L"../Resources/artResource/UI/Tutorial/OKButton_1.png");
	p2buttonBack->SetSprite(L"../Resources/artResource/UI/Tutorial/OKButton_2.png");
	p1buttonBack->SetImageType(ImageType::RadialFill);
	p2buttonBack->SetImageType(ImageType::RadialFill);
	p1buttonBack->SetClockwise(false);
	p2buttonBack->SetClockwise(false);

	p1buttonBack->GetRectTransform()->SetPivot({ 0.5f,0.5f });
	p2buttonBack->GetRectTransform()->SetPivot({ 0.5f,0.5f });

	tutorial1 = Resource::Load<Sprite>(L"../Resources/artResource/UI/Tutorial/Tutorial_1.png");
	tutorial1->IncreaseRefCount();
	tutorial2 = Resource::Load<Sprite>(L"../Resources/artResource/UI/Tutorial/Tutorial_2.png");
	tutorial2->IncreaseRefCount();
	tutorial3 = Resource::Load<Sprite>(L"../Resources/artResource/UI/Tutorial/Tutorial_3.png");
	tutorial3->IncreaseRefCount();
	tutorial4 = Resource::Load<Sprite>(L"../Resources/artResource/UI/Tutorial/Tutorial_4.png");
	tutorial4->IncreaseRefCount();
	tutorial5 = Resource::Load<Sprite>(L"../Resources/artResource/UI/Tutorial/Tutorial_5.png");
	tutorial5->IncreaseRefCount();
}

void TutorialImage::OnDestroy() {
	for (auto obj : Tutorialobject) {
		Destroy(obj);
		obj = nullptr;
	}
	delete buttonClickedCurve;
	if (IsValidObject(tutorial1))
		tutorial1->DecreaseRefCount();
	if (IsValidObject(tutorial2))
		tutorial2->DecreaseRefCount();
	if (IsValidObject(tutorial3))
		tutorial3->DecreaseRefCount();
	if (IsValidObject(tutorial4))
		tutorial4->DecreaseRefCount();
	if (IsValidObject(tutorial5))
		tutorial5->DecreaseRefCount();
}

void TutorialImage::Update() {
	TriggerPressedCheckReset();
	TriggerPressedCheck();

	switch (explainnum) {
	case 1:
		explanation->SetSprite(tutorial1);
		if (INPUT_GET_KEYDOWN(KeyCode::RightShift) || 
			rightTriggerPressed[0] || 
			rightTriggerPressed[1]) {
			SoundManager::instance->PlaySFX("Button");
			explainnum = 2;
		}
		break;
	case 2:
		explanation->SetSprite(tutorial2);
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
		explanation->SetSprite(tutorial3);
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
		explanation->SetSprite(tutorial4);
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
		explanation->SetSprite(tutorial5);
		if (INPUT_GET_KEYDOWN(KeyCode::LeftShift) ||
			leftTriggerPressed[0] ||
			leftTriggerPressed[1]) {
			SoundManager::instance->PlaySFX("Button");
			explainnum = 4;
		}
		break;
	}

	auto buttonMaxTimer = 1.25f;

	auto p1ButtonPress = INPUT_GET_KEY(KeyCode::Z) || INPUT_GET_GAMEPAD_BUTTON(0,GamepadButton::ButtonWest);
	auto p2ButtonPress = INPUT_GET_KEY(KeyCode::M) || INPUT_GET_GAMEPAD_BUTTON(1, GamepadButton::ButtonWest);

	auto lastButton1Timer = button1Timer;
	auto lastButton2Timer = button2Timer;

	if (button1Timer < buttonMaxTimer && p1ButtonPress) {
		button1Timer += TIME_GET_DELTATIME();
	}
	else if (button1Timer < buttonMaxTimer && !p1ButtonPress)
	{
		button1Timer = 0.0f;
	}
	else if (button1Timer >= buttonMaxTimer)
	{
		button1Timer = buttonMaxTimer;
		button1AnimTimer += TIME_GET_DELTATIME();
	}
	
	if (p1button->GetEnabled() && button1Timer == buttonMaxTimer)
	{
		SoundManager::instance->PlaySFX("Button");
		p1button->SetEnabled(false);
	}

	if (button2Timer < buttonMaxTimer && p2ButtonPress) {
		button2Timer += TIME_GET_DELTATIME();
	}
	else if (button2Timer < buttonMaxTimer && !p2ButtonPress)
	{
		button2Timer = 0.0f;
	}
	else if (button2Timer >= buttonMaxTimer)
	{
		button2Timer = buttonMaxTimer;
		button2AnimTimer += TIME_GET_DELTATIME();
	}

	if (p2button->GetEnabled() && button2Timer == buttonMaxTimer)
	{
		SoundManager::instance->PlaySFX("Button");
		p2button->SetEnabled(false);
	}

	p1buttonBack->SetFillAmount((button1Timer / buttonMaxTimer));
	p2buttonBack->SetFillAmount((button2Timer / buttonMaxTimer));

	p1buttonBack->GetRectTransform()->SetSizeDelta(buttonSizeDelta * buttonClickedCurve->Evaluate(button1AnimTimer));
	p2buttonBack->GetRectTransform()->SetSizeDelta(buttonSizeDelta * buttonClickedCurve->Evaluate(button2AnimTimer));
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