#include "OptionWindowPrefab.h"
#include "StartMenu.h"
#include "CrosshairInteractButton.h"
#include "OptionWindowSystem.h"
#include "ButtonAnimation.h"
#include "SliderSprite.h"

#include <GameObject.h>
#include <Canvas.h>
#include <SpriteRenderer.h>
#include <Collider2D.h>

using namespace GOTOEngine;
GameObject* GOTOEngine::OptionWindowPrefab::CreateOptionWindow()
{
    //루트 오브젝트
	auto GO = new GameObject(L"OptionWindow");

	auto optionWindowSystem = GO->AddComponent<OptionWindowSystem>();

	// 뒷 배경 창
	auto baseWindow = new GameObject(L"BaseWindow");
	auto baseWindowSprite = baseWindow->AddComponent<SpriteRenderer>();
	baseWindowSprite->SetSprite(L"../Resources/artResource/UI/Setting/환경 설정 UI.png");
	baseWindowSprite->SetRenderOrder(500);
	baseWindow->GetTransform()->SetParent(GO->GetTransform(), false);
	baseWindow->GetTransform()->SetLocalScale({ 0.0f, 0.0f });

	optionWindowSystem->openAnimScale = 0.85f;

	optionWindowSystem->baseWindow = baseWindow->GetTransform();
	optionWindowSystem->baseWindowSprite = baseWindowSprite;

	// 항목들
	//---- 항목 옵션
	auto gageYSpace = 100.0f;
	auto gageYStartPos = 180.0f;
	
	//---- 배경음
	auto BGMGageSpriteGO = new GameObject(L"BG Gage");
	auto BGMGageSprite = BGMGageSpriteGO->AddComponent<SliderSprite>();
	BGMGageSpriteGO->GetTransform()->SetParent(baseWindow->GetTransform());

	optionWindowSystem->sliderSprites[0] = BGMGageSprite;

	auto BGMGageBackGO = new GameObject(L"BG Gage - BackGround");
	BGMGageBackGO->GetTransform()->SetParent(BGMGageSpriteGO->GetTransform());
	auto BGMGageBackGORenderer = BGMGageBackGO->AddComponent<SpriteRenderer>();
	BGMGageSprite->backgroundRenderer = BGMGageBackGORenderer;

	BGMGageSpriteGO->GetTransform()->SetLocalPosition({ -580.0f * 0.5f,gageYStartPos });

	auto BGMGageBarGO = new GameObject(L"BG Gage - GageBar");
	BGMGageBarGO->GetTransform()->SetParent(BGMGageSpriteGO->GetTransform());
	auto BGMGageBarGORenderer = BGMGageBarGO->AddComponent<SpriteRenderer>();
	BGMGageSprite->sliderBarRenderer = BGMGageBarGORenderer;

	auto BGMGageHandleGO = new GameObject(L"BG Gage - GageHandle");
	BGMGageHandleGO->GetTransform()->SetParent(BGMGageSpriteGO->GetTransform());
	auto BGMGageHandleGORenderer = BGMGageHandleGO->AddComponent<SpriteRenderer>();
	BGMGageSprite->sliderHandleRenderer = BGMGageHandleGORenderer;
	

	//---- 효과음
	auto SEGageSpriteGO = new GameObject(L"SE Gage");
	auto SEGageSprite = SEGageSpriteGO->AddComponent<SliderSprite>();
	SEGageSpriteGO->GetTransform()->SetParent(baseWindow->GetTransform());

	optionWindowSystem->sliderSprites[1] = SEGageSprite;

	auto SEGageBackGO = new GameObject(L"SE Gage - BackGround");
	SEGageBackGO->GetTransform()->SetParent(SEGageSpriteGO->GetTransform());
	auto SEGageBackGORenderer = SEGageBackGO->AddComponent<SpriteRenderer>();
	SEGageSprite->backgroundRenderer = SEGageBackGORenderer;

	SEGageSpriteGO->GetTransform()->SetLocalPosition({ -580.0f * 0.5f,gageYStartPos - gageYSpace });

	auto SEGageBarGO = new GameObject(L"SE Gage - GageBar");
	SEGageBarGO->GetTransform()->SetParent(SEGageSpriteGO->GetTransform());
	auto SEGageBarGORenderer = SEGageBarGO->AddComponent<SpriteRenderer>();
	SEGageSprite->sliderBarRenderer = SEGageBarGORenderer;

	auto SEGageHandleGO = new GameObject(L"SE Gage - GageHandle");
	SEGageHandleGO->GetTransform()->SetParent(SEGageSpriteGO->GetTransform());
	auto SEGageHandleGORenderer = SEGageHandleGO->AddComponent<SpriteRenderer>();
	SEGageSprite->sliderHandleRenderer = SEGageHandleGORenderer;
	
	//---- 1P 감도
	auto P1SensGageSpriteGO = new GameObject(L"P1 Sensitivity Gage");
	auto P1SensGageSprite = P1SensGageSpriteGO->AddComponent<SliderSprite>();
	P1SensGageSpriteGO->GetTransform()->SetParent(baseWindow->GetTransform());

	optionWindowSystem->sliderSprites[2] = P1SensGageSprite;

	auto P1SensGageBackGO = new GameObject(L"P1 Sensitivity Gage - BackGround");
	P1SensGageBackGO->GetTransform()->SetParent(P1SensGageSpriteGO->GetTransform());
	auto P1SensGageBackGORenderer = P1SensGageBackGO->AddComponent<SpriteRenderer>();
	P1SensGageSprite->backgroundRenderer = P1SensGageBackGORenderer;

	P1SensGageSpriteGO->GetTransform()->SetLocalPosition({ -580.0f * 0.5f,gageYStartPos - gageYSpace * 2 });

	auto P1SensGageBarGO = new GameObject(L"P1 Sensitivity Gage - GageBar");
	P1SensGageBarGO->GetTransform()->SetParent(P1SensGageSpriteGO->GetTransform());
	auto P1SensGageBarGORenderer = P1SensGageBarGO->AddComponent<SpriteRenderer>();
	P1SensGageSprite->sliderBarRenderer = P1SensGageBarGORenderer;

	auto P1SensGageHandleGO = new GameObject(L"P1 Sensitivity Gage - GageHandle");
	P1SensGageHandleGO->GetTransform()->SetParent(P1SensGageSpriteGO->GetTransform());
	auto P1SensGageHandleGORenderer = P1SensGageHandleGO->AddComponent<SpriteRenderer>();
	P1SensGageSprite->sliderHandleRenderer = P1SensGageHandleGORenderer;
	
	//---- 2P 감도
	auto P2SensGageSpriteGO = new GameObject(L"P2 Sensitivity Gage");
	auto P2SensGageSprite = P2SensGageSpriteGO->AddComponent<SliderSprite>();
	P2SensGageSpriteGO->GetTransform()->SetParent(baseWindow->GetTransform());

	optionWindowSystem->sliderSprites[3] = P2SensGageSprite;

	auto P2SensGageBackGO = new GameObject(L"P2 Sensitivity Gage - BackGround");
	P2SensGageBackGO->GetTransform()->SetParent(P2SensGageSpriteGO->GetTransform());
	auto P2SensGageBackGORenderer = P2SensGageBackGO->AddComponent<SpriteRenderer>();
	P2SensGageSprite->backgroundRenderer = P2SensGageBackGORenderer;

	P2SensGageSpriteGO->GetTransform()->SetLocalPosition({ -580.0f * 0.5f,gageYStartPos - gageYSpace * 3 });

	auto P2SensGageBarGO = new GameObject(L"P2 Sensitivity Gage - GageBar");
	P2SensGageBarGO->GetTransform()->SetParent(P2SensGageSpriteGO->GetTransform());
	auto P2SensGageBarGORenderer = P2SensGageBarGO->AddComponent<SpriteRenderer>();
	P2SensGageSprite->sliderBarRenderer = P2SensGageBarGORenderer;

	auto P2SensGageHandleGO = new GameObject(L"P2 Sensitivity Gage - GageHandle");
	P2SensGageHandleGO->GetTransform()->SetParent(P2SensGageSpriteGO->GetTransform());
	auto P2SensGageHandleGORenderer = P2SensGageHandleGO->AddComponent<SpriteRenderer>();
	P2SensGageSprite->sliderHandleRenderer = P2SensGageHandleGORenderer;

	// 포커스 UI
	auto focusUI = new GameObject(L"FocusUI");
	auto focusUISprite = focusUI->AddComponent<SpriteRenderer>();
	focusUISprite->SetSprite(L"../Resources/Demo/FocusUI.png");
	focusUISprite->SetRenderOrder(501);
	focusUI->GetTransform()->SetParent(baseWindow->GetTransform(), false);

	optionWindowSystem->focusUITransform = focusUI->GetTransform();


    return GO;
}
