#include "StartMenuPrefab.h"
#include "StartMenu.h"
#include "CrosshairInteractButton.h"
#include "ButtonAnimation.h"
#include "OptionWindowPrefab.h"

#include <GameObject.h>
#include <Canvas.h>
#include <SpriteRenderer.h>
#include <Collider2D.h>

using namespace GOTOEngine;
GameObject* StartMenuPrefab::CreateStartMenu()
{
	auto GO = new GameObject(L"StartMenu");
	auto startMenu = GO->AddComponent<StartMenu>();

	// optionWindow 오브젝트 생성
	startMenu->optionWindow = OptionWindowPrefab::CreateOptionWindow();
	startMenu->optionWindow->SetActive(false); // 초기에는 비활성화

	//===== Start 버튼 =====

	auto startButtonGO = new GameObject(L"StartButton");
	startButtonGO->GetTransform()->SetParent(GO->GetTransform());
	startMenu->startButton = startButtonGO->GetTransform();
	startMenu->startButton->SetLocalPosition({ 0.0f, -150.0f });
	startButtonGO->AddComponent<SpriteRenderer>()->SetSprite(L"../Resources/Demo/MenuButton.png");
	auto startButtonAnimation = startButtonGO->AddComponent<ButtonAnimation>();
	
	//버튼 콜라이더 추가
	auto startButtonP1ColGO = new GameObject(L"StartButtonP1Col");
	auto startButtonP1Col = startButtonP1ColGO->AddComponent<Collider2D>();
	auto startButtonP2ColGO = new GameObject(L"StartButtonP2Col");
	auto startButtonP2Col = startButtonP2ColGO->AddComponent<Collider2D>();

	auto startInteractButtonP1 = startButtonP1ColGO->AddComponent<CrosshairInteractButton>();
	startInteractButtonP1->parentButton = startButtonGO->GetTransform();
	startInteractButtonP1->onEnter.Add(startButtonAnimation, &ButtonAnimation::PlayRotateAnimation);
	startInteractButtonP1->onInteract.Add(startButtonAnimation, &ButtonAnimation::PlayScaleAnimaiton);
	startMenu->p1InteractButtons.push_back(startInteractButtonP1);

	auto startInteractButtonP2 = startButtonP2ColGO->AddComponent<CrosshairInteractButton>();
	startInteractButtonP2->parentButton = startButtonGO->GetTransform();
	startInteractButtonP2->onEnter.Add(startButtonAnimation, &ButtonAnimation::PlayRotateAnimation);
	startInteractButtonP2->onInteract.Add(startButtonAnimation, &ButtonAnimation::PlayScaleAnimaiton);
	startMenu->p2InteractButtons.push_back(startInteractButtonP2);

	auto spriteSize = startButtonGO->GetComponent<SpriteRenderer>()->GetSprite()->GetRect();
	startButtonP1Col->SetSize({ spriteSize.width, spriteSize.height });
	startButtonP2Col->SetSize({ spriteSize.width, spriteSize.height });

	startButtonP1ColGO->layer = (1 << 1);
	startButtonP2ColGO->layer = (1 << 2);

	startButtonP1ColGO->GetTransform()->SetParent(startButtonGO->GetTransform(), false);
	startButtonP2ColGO->GetTransform()->SetParent(startButtonGO->GetTransform(), false);
	

	//===== Option 버튼 =====

	auto optionsButtonGO = new GameObject(L"OptionsButton");
	optionsButtonGO->GetTransform()->SetParent(GO->GetTransform());
	startMenu->optionsButton = optionsButtonGO->GetTransform();
	startMenu->optionsButton->SetLocalPosition({ 0.0f, -300.0f });
	optionsButtonGO->AddComponent<SpriteRenderer>()->SetSprite(L"../Resources/Demo/MenuButton.png");
	auto optionsButtonAnimation = optionsButtonGO->AddComponent<ButtonAnimation>();

	//버튼 콜라이더 추가
	auto optionButtonP1ColGO = new GameObject(L"OptionButtonP1Col");
	auto optionButtonP1Col = optionButtonP1ColGO->AddComponent<Collider2D>();
	auto optionButtonP2ColGO = new GameObject(L"OptionButtonP2Col");
	auto optionButtonP2Col = optionButtonP2ColGO->AddComponent<Collider2D>();

	auto optionInteractButtonP1 = optionButtonP1ColGO->AddComponent<CrosshairInteractButton>();
	optionInteractButtonP1->parentButton = optionsButtonGO->GetTransform();
	optionInteractButtonP1->onEnter.Add(optionsButtonAnimation, &ButtonAnimation::PlayRotateAnimation);
	optionInteractButtonP1->onInteract.Add(optionsButtonAnimation, &ButtonAnimation::PlayScaleAnimaiton);
	startMenu->p1InteractButtons.push_back(optionInteractButtonP1);

	auto optionInteractButtonP2 = optionButtonP2ColGO->AddComponent<CrosshairInteractButton>();
	optionInteractButtonP2->parentButton = optionsButtonGO->GetTransform();
	optionInteractButtonP2->onEnter.Add(optionsButtonAnimation, &ButtonAnimation::PlayRotateAnimation);
	optionInteractButtonP2->onInteract.Add(optionsButtonAnimation, &ButtonAnimation::PlayScaleAnimaiton);
	startMenu->p2InteractButtons.push_back(optionInteractButtonP2);

	optionButtonP1Col->SetSize({ spriteSize.width, spriteSize.height });
	optionButtonP2Col->SetSize({ spriteSize.width, spriteSize.height });

	optionButtonP1ColGO->layer = (1 << 1);
	optionButtonP2ColGO->layer = (1 << 2);

	optionButtonP1ColGO->GetTransform()->SetParent(optionsButtonGO->GetTransform(), false);
	optionButtonP2ColGO->GetTransform()->SetParent(optionsButtonGO->GetTransform(), false);

	//===== Exit 버튼 =====

	auto exitButtonGO = new GameObject(L"ExitButton");
	exitButtonGO->GetTransform()->SetParent(GO->GetTransform());
	startMenu->exitButton = exitButtonGO->GetTransform();
	startMenu->exitButton->SetLocalPosition({ 0.0f, -450.0f });
	exitButtonGO->AddComponent<SpriteRenderer>()->SetSprite(L"../Resources/Demo/MenuButton.png");
	auto exitButtonAnimation = exitButtonGO->AddComponent<ButtonAnimation>();

	//버튼 콜라이더 추가
	auto exitButtonP1ColGO = new GameObject(L"ExitButtonP1Col");
	auto exitButtonP1Col = exitButtonP1ColGO->AddComponent<Collider2D>();
	auto exitButtonP2ColGO = new GameObject(L"ExitButtonP2Col");
	auto exitButtonP2Col = exitButtonP2ColGO->AddComponent<Collider2D>();

	exitButtonP1Col->SetSize({ spriteSize.width, spriteSize.height });
	exitButtonP2Col->SetSize({ spriteSize.width, spriteSize.height });

	exitButtonP1ColGO->layer = (1 << 1);
	exitButtonP2ColGO->layer = (1 << 2);

	auto exitInteractButtonP1 = exitButtonP1ColGO->AddComponent<CrosshairInteractButton>();
	exitInteractButtonP1->parentButton = exitButtonGO->GetTransform();
	exitInteractButtonP1->onEnter.Add(exitButtonAnimation, &ButtonAnimation::PlayRotateAnimation);
	exitInteractButtonP1->onInteract.Add(exitButtonAnimation, &ButtonAnimation::PlayScaleAnimaiton);
	startMenu->p1InteractButtons.push_back(exitInteractButtonP1);

	auto exitInteractButtonP2 = exitButtonP2ColGO->AddComponent<CrosshairInteractButton>();
	exitInteractButtonP2->parentButton = exitButtonGO->GetTransform();
	exitInteractButtonP2->onEnter.Add(exitButtonAnimation, &ButtonAnimation::PlayRotateAnimation);
	exitInteractButtonP2->onInteract.Add(exitButtonAnimation, &ButtonAnimation::PlayScaleAnimaiton);
	startMenu->p2InteractButtons.push_back(exitInteractButtonP2);

	exitButtonP1ColGO->GetTransform()->SetParent(exitButtonGO->GetTransform(), false);
	exitButtonP2ColGO->GetTransform()->SetParent(exitButtonGO->GetTransform(), false);

	return GO;
}
