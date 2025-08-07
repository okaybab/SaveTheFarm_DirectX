#pragma once
#include <ScriptBehaviour.h>
#include <InputManager.h>
#include <RenderManager.h>
#include <SpriteRenderer.h>

#include "Canvas.h"
#include "Image.h"
#include "RectTransform.h"
#include "Screen.h"

namespace GOTOEngine
{
	class AnimationCurve;
	class TutorialImage : public ScriptBehaviour
	{
	private:
		Image* explanation;
		Image* p1button;
		Image* p2button;
		Image* p1buttonBack;
		Image* p2buttonBack;

		AnimationCurve* buttonClickedCurve;

		Sprite* tutorial1;
		Sprite* tutorial2;
		Sprite* tutorial3;
		Sprite* tutorial4;
		Sprite* tutorial5;
		Sprite* tutorial6;

		int explainnum = 1;
		std::vector<GameObject*> Tutorialobject;
		bool rightTriggerCheckTrigger[2];
		bool rightTriggerPressed[2];
		bool leftTriggerCheckTrigger[2];
		bool leftTriggerPressed[2];

		Vector2 buttonSizeDelta;

		float button1Timer = 0.0f;
		float button2Timer = 0.0f;
		float maxButtonTimer = 1.25f;

		float button1AnimTimer = 0.0f;
		float button2AnimTimer = 0.0f;

		void TriggerPressedCheck();

		void TriggerPressedCheckReset();

	public:
    TutorialImage()
    {
        REGISTER_BEHAVIOUR_MESSAGE(Awake);
        REGISTER_BEHAVIOUR_MESSAGE(OnDestroy);
        REGISTER_BEHAVIOUR_MESSAGE(Update);
    }
		//static TutorialImage* instance;
		void Awake();
		void Update();
		void OnDestroy();

		float GetMaxButtonTimer() { return maxButtonTimer; }
		float GetButton1Timer() { return button1Timer; }
		float GetButton2Timer() { return button2Timer; }
	};
}

