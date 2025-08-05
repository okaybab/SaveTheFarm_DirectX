#pragma once
#include <ScriptBehaviour.h>
#include <Transform.h>
#include "CrosshairInteractButton.h"
#include <SpriteRenderer.h>
#include <Engine.h>
namespace GOTOEngine
{
	class StartMenu : public ScriptBehaviour
	{
	public:
    StartMenu()
    {
        REGISTER_BEHAVIOUR_MESSAGE(Update);
    }
		Transform* startButton;
		Transform* optionsButton;
		Transform* exitButton;

		GameObject* optionWindow = nullptr; // 옵션 윈도우 오브젝트

		std::vector<CrosshairInteractButton*> p1InteractButtons;
		std::vector<CrosshairInteractButton*> p2InteractButtons;

		CrosshairInteractButton* lastP1InteractButton = nullptr;
		CrosshairInteractButton* lastP2InteractButton = nullptr;

		void Update()
		{
			//매 프레임 마다 스타트 메뉴 확인
			CrosshairInteractButton* currentP1InteractButtons = nullptr;
			CrosshairInteractButton* currentP2InteractButtons = nullptr;

			float lastTime = 0.0f;
			for (auto* button : p1InteractButtons)
			{
				//가장 마지막에 누른 버튼찾기
				CrosshairInteractButton* prevButton = currentP1InteractButtons;
				if (button->isInteracted && button->interactedTime > lastTime)
				{
					lastTime = button->interactedTime;
					currentP1InteractButtons = button;
				}
			}

			if (currentP1InteractButtons != lastP1InteractButton)
			{
				if (IsValidObject(lastP1InteractButton))
				{
					// 이전 버튼의 상호작용 상태 초기화
					lastP1InteractButton->isInteracted = false; 
					auto lastButtonP1Sprite = lastP1InteractButton->parentButton->GetComponent<SpriteRenderer>();
					if (IsValidObject(lastButtonP1Sprite))
					{
						lastButtonP1Sprite->SetSprite(L"../Resources/Demo/MenuButton.png");
					}
				}
				lastP1InteractButton = currentP1InteractButtons;

				if (IsValidObject(currentP1InteractButtons))
				{
					// 현재 버튼의 상호작용 상태 설정
					currentP1InteractButtons->isInteracted = true;
					auto currentButtonP1Sprite = currentP1InteractButtons->parentButton->GetComponent<SpriteRenderer>();
					if (IsValidObject(currentButtonP1Sprite))
					{
						currentButtonP1Sprite->SetSprite(L"../Resources/Demo/MenuButton_Select_P1.png");
					}
				}
			}

			lastTime = 0.0f;
			for (auto* button : p2InteractButtons)
			{
				//가장 마지막에 누른 버튼찾기
				CrosshairInteractButton* prevButton = currentP2InteractButtons;
				if (button->isInteracted && button->interactedTime > lastTime)
				{
					lastTime = button->interactedTime;
					currentP2InteractButtons = button;
				}
			}

			if (currentP2InteractButtons != lastP2InteractButton)
			{
				if (IsValidObject(lastP2InteractButton))
				{
					// 이전 버튼의 상호작용 상태 초기화
					lastP2InteractButton->isInteracted = false;
					auto lastButtonP2Sprite = lastP2InteractButton->parentButton->GetComponent<SpriteRenderer>();
					if (IsValidObject(lastButtonP2Sprite))
					{
						lastButtonP2Sprite->SetSprite(L"../Resources/Demo/MenuButton.png");
					}
				}
				lastP2InteractButton = currentP2InteractButtons;

				if (IsValidObject(currentP2InteractButtons))
				{
					// 현재 버튼의 상호작용 상태 설정
					currentP2InteractButtons->isInteracted = true;
					auto currentButtonP2Sprite = currentP2InteractButtons->parentButton->GetComponent<SpriteRenderer>();
					if (IsValidObject(currentButtonP2Sprite))
					{
						currentButtonP2Sprite->SetSprite(L"../Resources/Demo/MenuButton_Select_P2.png");
					}
				}
			}

			bool menuIsValidInteract = false;
			if (IsValidObject(currentP1InteractButtons)
				&& IsValidObject(currentP2InteractButtons) 
				&& currentP1InteractButtons->parentButton == currentP2InteractButtons->parentButton)
			{
				// 싱글 플레이어 모드 선택
				if (currentP1InteractButtons->parentButton == startButton)
				{
					// 게임 시작 로직
					SCENE_CHANGE_SCENE(L"PlayScene");
				}
				else if (currentP1InteractButtons->parentButton == optionsButton)
				{
					// 옵션 메뉴 로직
					optionWindow->SetActive(true);
				}
				else if (currentP1InteractButtons->parentButton == exitButton)
				{
					// 게임 종료 로직
					ENGINE_QUIT();
				}

				menuIsValidInteract = true;
			}
			else if ((IsValidObject(currentP1InteractButtons) 
				&& currentP1InteractButtons->parentButton == optionsButton)
				|| (IsValidObject(currentP2InteractButtons)
				&& currentP2InteractButtons->parentButton == optionsButton))
			{
				//옵션 선택
				menuIsValidInteract = true;
				optionWindow->SetActive(true);
			}


			// 두 플레이어가 같은 버튼을 누른 경우
			if (menuIsValidInteract)
			{
				for (auto* button : p1InteractButtons)
				{
					button->interactedTime = 0.0f; // 모든 버튼의 상호작용 시간 초기화
					button->isInteracted = false; // 모든 버튼의 상호작용 상태 초기화
				}
				for (auto* button : p2InteractButtons)
				{
					button->interactedTime = 0.0f; // 모든 버튼의 상호작용 시간 초기화
					button->isInteracted = false; // 모든 버튼의 상호작용 상태 초기화
				}

				auto buttonSprite = startButton->GetComponent<SpriteRenderer>();
				if (IsValidObject(buttonSprite))
				{
					buttonSprite->SetSprite(L"../Resources/Demo/MenuButton.png"); // 모든 버튼의 스프라이트 초기화
				}
				buttonSprite = optionsButton->GetComponent<SpriteRenderer>();
				if (IsValidObject(buttonSprite))
				{
					buttonSprite->SetSprite(L"../Resources/Demo/MenuButton.png"); // 모든 버튼의 스프라이트 초기화
				}
				buttonSprite = exitButton->GetComponent<SpriteRenderer>();
				if (IsValidObject(buttonSprite))
				{
					buttonSprite->SetSprite(L"../Resources/Demo/MenuButton.png"); // 모든 버튼의 스프라이트 초기화
				}
			}
		}
	};
}