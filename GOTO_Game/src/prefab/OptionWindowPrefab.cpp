#include "OptionWindowPrefab.h"
#include "StartMenu.h"
#include "CrosshairInteractButton.h"
#include "OptionWindowSystem.h"
#include "ButtonAnimation.h"
#include "SliderSprite.h"

#include <GameObject.h>
#include <Canvas.h>
#include <SpriteRenderer.h>
#include <TextRenderer.h>
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

	// 공용 리소스 (텍스쳐)
	Texture2D* gageBGTexture = Resource::Load<Texture2D>(L"../Resources/artResource/UI/Setting/배경음 슬라이더.png");
	Texture2D* gageBarTexture = Resource::Load<Texture2D>(L"../Resources/artResource/UI/Setting/감도 슬라이더.png");
	Texture2D* gageHandleTexture = Resource::Load<Texture2D>(L"../Resources/artResource/UI/Setting/슬라이더 조절 표시기.png");

	// 항목들
	//---- 항목 옵션
	auto gageYSpace = 150.0f;
	auto gageYStartPos = 255.0f;

	auto optionTitleBarTextGO = new GameObject(L"titlebar text");
	auto optionTitleBarText = optionTitleBarTextGO->AddComponent<TextRenderer>();
	optionTitleBarText->SetRenderOrder(505);
	optionTitleBarText->color = { 0,0,0,255 };
	optionTitleBarText->size = 65;
	optionTitleBarText->rect = { 0,0,450,300 };
	optionTitleBarText->text = L"환경 설정";
	optionTitleBarText->GetTransform()->SetParent(baseWindow->GetTransform());
	optionTitleBarText->SetFont(L"../Resources/Maplestory Bold.ttf");
	optionTitleBarText->GetTransform()->SetLocalPosition({ 0.0f,505 });

	
	//---- 배경음
	auto BGMGageSpriteGO = new GameObject(L"BG Gage");
	BGMGageSpriteGO->GetTransform()->SetLocalScale({ 0.32f,0.32f });
	auto BGMGageSprite = BGMGageSpriteGO->AddComponent<SliderSprite>();
	BGMGageSpriteGO->GetTransform()->SetParent(baseWindow->GetTransform());

	optionWindowSystem->sliderSprites[0] = BGMGageSprite;

	auto BGMGageBackGO = new GameObject(L"BG Gage - BackGround");
	BGMGageBackGO->GetTransform()->SetParent(BGMGageSpriteGO->GetTransform());
	auto BGMGageBackGORenderer = BGMGageBackGO->AddComponent<SpriteRenderer>();
	BGMGageSprite->backgroundRenderer = BGMGageBackGORenderer;

	BGMGageSpriteGO->GetTransform()->SetLocalPosition({ -1587.0f * 0.32f * 0.3f,gageYStartPos });

	auto BGMGageBarGO = new GameObject(L"BG Gage - GageBar");
	BGMGageBarGO->GetTransform()->SetParent(BGMGageSpriteGO->GetTransform());
	auto BGMGageBarGORenderer = BGMGageBarGO->AddComponent<SpriteRenderer>();
	BGMGageSprite->sliderBarRenderer = BGMGageBarGORenderer;

	auto BGMGageHandleGO = new GameObject(L"BG Gage - GageHandle");
	BGMGageHandleGO->GetTransform()->SetParent(BGMGageSpriteGO->GetTransform());
	auto BGMGageHandleGORenderer = BGMGageHandleGO->AddComponent<SpriteRenderer>();
	BGMGageSprite->sliderHandleRenderer = BGMGageHandleGORenderer;

	BGMGageSprite->SetBackgroundTexture(gageBGTexture);
	BGMGageSprite->SetSliderBarTexture(gageBarTexture);
	BGMGageSprite->SetSliderHandleTexture(gageHandleTexture);

	auto BGMTextGO = new GameObject(L"BGM 텍스트");
	auto BGMText = BGMTextGO->AddComponent<TextRenderer>();
	BGMText->SetRenderOrder(505);
	BGMText->color = { 0,0,0,255 };
	BGMText->size = 65;
    BGMText->rect = { 0,0,450,300 };
	BGMText->text = L"배경음";
	BGMTextGO->GetTransform()->SetParent(baseWindow->GetTransform());
	BGMText->SetFont(L"../Resources/Maplestory Bold.ttf");
	BGMTextGO->GetTransform()->SetLocalPosition({ -285.0f,gageYStartPos });


	//---- 효과음
	auto SEGageSpriteGO = new GameObject(L"SE Gage");
	SEGageSpriteGO->GetTransform()->SetLocalScale({ 0.32f,0.32f });
	auto SEGageSprite = SEGageSpriteGO->AddComponent<SliderSprite>();
	SEGageSpriteGO->GetTransform()->SetParent(baseWindow->GetTransform());

	optionWindowSystem->sliderSprites[1] = SEGageSprite;

	auto SEGageBackGO = new GameObject(L"SE Gage - BackGround");
	SEGageBackGO->GetTransform()->SetParent(SEGageSpriteGO->GetTransform());
	auto SEGageBackGORenderer = SEGageBackGO->AddComponent<SpriteRenderer>();
	SEGageSprite->backgroundRenderer = SEGageBackGORenderer;

	SEGageSpriteGO->GetTransform()->SetLocalPosition({ -1587.0f * 0.32f * 0.3f,gageYStartPos - gageYSpace });

	auto SEGageBarGO = new GameObject(L"SE Gage - GageBar");
	SEGageBarGO->GetTransform()->SetParent(SEGageSpriteGO->GetTransform());
	auto SEGageBarGORenderer = SEGageBarGO->AddComponent<SpriteRenderer>();
	SEGageSprite->sliderBarRenderer = SEGageBarGORenderer;

	auto SEGageHandleGO = new GameObject(L"SE Gage - GageHandle");
	SEGageHandleGO->GetTransform()->SetParent(SEGageSpriteGO->GetTransform());
	auto SEGageHandleGORenderer = SEGageHandleGO->AddComponent<SpriteRenderer>();
	SEGageSprite->sliderHandleRenderer = SEGageHandleGORenderer;

	SEGageSprite->SetBackgroundTexture(gageBGTexture);
	SEGageSprite->SetSliderBarTexture(gageBarTexture);
	SEGageSprite->SetSliderHandleTexture(gageHandleTexture);

	auto SETextGO = new GameObject(L"SE 텍스트");
	auto SEText = SETextGO->AddComponent<TextRenderer>();
	SEText->SetRenderOrder(505);
	SEText->color = { 0,0,0,255 };
	SEText->size = 65;
	SEText->rect = { 0,0,450,300 };
	SEText->text = L"효과음";
	SETextGO->GetTransform()->SetParent(baseWindow->GetTransform());
	SEText->SetFont(L"../Resources/Maplestory Bold.ttf");
	SETextGO->GetTransform()->SetLocalPosition({ -285.0f,gageYStartPos - gageYSpace });
	
	//---- 1P 감도
	auto P1SensGageSpriteGO = new GameObject(L"P1 Sensitivity Gage");
	P1SensGageSpriteGO->GetTransform()->SetLocalScale({ 0.32f,0.32f });
	auto P1SensGageSprite = P1SensGageSpriteGO->AddComponent<SliderSprite>();
	P1SensGageSpriteGO->GetTransform()->SetParent(baseWindow->GetTransform());

	optionWindowSystem->sliderSprites[2] = P1SensGageSprite;

	auto P1SensGageBackGO = new GameObject(L"P1 Sensitivity Gage - BackGround");
	P1SensGageBackGO->GetTransform()->SetParent(P1SensGageSpriteGO->GetTransform());
	auto P1SensGageBackGORenderer = P1SensGageBackGO->AddComponent<SpriteRenderer>();
	P1SensGageSprite->backgroundRenderer = P1SensGageBackGORenderer;

	P1SensGageSpriteGO->GetTransform()->SetLocalPosition({ -1587.0f * 0.32f * 0.3f,gageYStartPos - gageYSpace * 2 });

	auto P1SensGageBarGO = new GameObject(L"P1 Sensitivity Gage - GageBar");
	P1SensGageBarGO->GetTransform()->SetParent(P1SensGageSpriteGO->GetTransform());
	auto P1SensGageBarGORenderer = P1SensGageBarGO->AddComponent<SpriteRenderer>();
	P1SensGageSprite->sliderBarRenderer = P1SensGageBarGORenderer;

	auto P1SensGageHandleGO = new GameObject(L"P1 Sensitivity Gage - GageHandle");
	P1SensGageHandleGO->GetTransform()->SetParent(P1SensGageSpriteGO->GetTransform());
	auto P1SensGageHandleGORenderer = P1SensGageHandleGO->AddComponent<SpriteRenderer>();
	P1SensGageSprite->sliderHandleRenderer = P1SensGageHandleGORenderer;

	P1SensGageSprite->SetBackgroundTexture(gageBGTexture);
	P1SensGageSprite->SetSliderBarTexture(gageBarTexture);
	P1SensGageSprite->SetSliderHandleTexture(gageHandleTexture);

	auto P1SensTextGO = new GameObject(L"1P감도 텍스트");
	auto P1SensText = P1SensTextGO->AddComponent<TextRenderer>();
	P1SensText->SetRenderOrder(505);
	P1SensText->color = { 0,0,0,255 };
	P1SensText->size = 65;
	P1SensText->rect = { 0,0,450,300 };
	P1SensText->text = L"1P 감도";
	P1SensTextGO->GetTransform()->SetParent(baseWindow->GetTransform());
	P1SensText->SetFont(L"../Resources/Maplestory Bold.ttf");
	P1SensTextGO->GetTransform()->SetLocalPosition({ -285.0f,gageYStartPos - gageYSpace * 2});
	
	//---- 2P 감도
	auto P2SensGageSpriteGO = new GameObject(L"P2 Sensitivity Gage");
	P2SensGageSpriteGO->GetTransform()->SetLocalScale({ 0.32f,0.32f });
	auto P2SensGageSprite = P2SensGageSpriteGO->AddComponent<SliderSprite>();
	P2SensGageSpriteGO->GetTransform()->SetParent(baseWindow->GetTransform());

	optionWindowSystem->sliderSprites[3] = P2SensGageSprite;

	auto P2SensGageBackGO = new GameObject(L"P2 Sensitivity Gage - BackGround");
	P2SensGageBackGO->GetTransform()->SetParent(P2SensGageSpriteGO->GetTransform());
	auto P2SensGageBackGORenderer = P2SensGageBackGO->AddComponent<SpriteRenderer>();
	P2SensGageSprite->backgroundRenderer = P2SensGageBackGORenderer;

	P2SensGageSpriteGO->GetTransform()->SetLocalPosition({ -1587.0f * 0.32f * 0.3f,gageYStartPos - gageYSpace * 3 });

	auto P2SensGageBarGO = new GameObject(L"P2 Sensitivity Gage - GageBar");
	P2SensGageBarGO->GetTransform()->SetParent(P2SensGageSpriteGO->GetTransform());
	auto P2SensGageBarGORenderer = P2SensGageBarGO->AddComponent<SpriteRenderer>();
	P2SensGageSprite->sliderBarRenderer = P2SensGageBarGORenderer;

	auto P2SensGageHandleGO = new GameObject(L"P2 Sensitivity Gage - GageHandle");
	P2SensGageHandleGO->GetTransform()->SetParent(P2SensGageSpriteGO->GetTransform());
	auto P2SensGageHandleGORenderer = P2SensGageHandleGO->AddComponent<SpriteRenderer>();
	P2SensGageSprite->sliderHandleRenderer = P2SensGageHandleGORenderer;

	P2SensGageSprite->SetBackgroundTexture(gageBGTexture);
	P2SensGageSprite->SetSliderBarTexture(gageBarTexture);
	P2SensGageSprite->SetSliderHandleTexture(gageHandleTexture);

	auto P2SensTextGO = new GameObject(L"2P감도 텍스트");
	auto P2SensText = P2SensTextGO->AddComponent<TextRenderer>();
	P2SensText->SetRenderOrder(505);
	P2SensText->color = { 0,0,0,255 };
	P2SensText->size = 65;
	P2SensText->rect = { 0,0,450,300 };
	P2SensText->text = L"2P 감도";
	P2SensTextGO->GetTransform()->SetParent(baseWindow->GetTransform());
	P2SensText->SetFont(L"../Resources/Maplestory Bold.ttf");
	P2SensTextGO->GetTransform()->SetLocalPosition({ -285.0f,gageYStartPos - gageYSpace * 3 });

	// 닫기 버튼

	auto exitButtonGO = new GameObject(L"exit button");
	auto exitButtonSprite = exitButtonGO->AddComponent<SpriteRenderer>();
	exitButtonSprite->SetSprite(L"../Resources/artResource/UI/Setting/환경설정 X 버튼.png");
	exitButtonSprite->SetRenderOrder(505);
	exitButtonGO->GetTransform()->SetParent(baseWindow->GetTransform(), false);
	exitButtonGO->GetTransform()->SetLocalScale({ 0.15f,0.15f });
	exitButtonGO->GetTransform()->SetLocalPosition({ 0.0f, gageYStartPos - gageYSpace * 4 - 20 });

	optionWindowSystem->exitButtonSprite = exitButtonSprite;

	// 포커스 UI
	auto focusUI = new GameObject(L"FocusUI");
	auto focusUISprite = focusUI->AddComponent<SpriteRenderer>();
	focusUISprite->SetSprite(L"../Resources/Demo/FocusUI.png");
	focusUISprite->SetRenderOrder(501);
	focusUI->GetTransform()->SetParent(baseWindow->GetTransform(), false);

	optionWindowSystem->focusUITransform = focusUI->GetTransform();

    return GO;
}
