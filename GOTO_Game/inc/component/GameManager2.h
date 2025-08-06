#pragma once
#include <ScriptBehaviour.h>
#include <RenderManager.h>
#include <SpriteRenderer.h>
#include <TimeManager.h>
#include <InputManager.h>
#include <Text.h>
#include "RectTransform.h"
#include "Screen.h"
#include "Image.h"

namespace GOTOEngine
{
	class AnimationCurve;
	class TutorialImage2;
	class GameManager2 : public ScriptBehaviour
	{
	private:
		bool p1active = false;
		bool p2active = false;

		Text* Timetext;
		Image* warningImage;
		Sprite* warningsprite;

		int totalSeconds;
		int minutes;
		int seconds;
		AnimationCurve* warninganimation;
		float warningAniTime = 0.0f;

		int itemchange;
		float NormalTiming = 122.0f;
		float GimmickTiming = 120.0f;
		float NormalWarningTiming = 120.0f;
		float GimmickWarningTiming = 120.0f;
		float ItemTiming[6] = { 150.0f, 130.0f, 90.0f, 70.0f, 30.0f, 10.0f };

		bool warningon = false;

		float tutorialCheckTime = 0.0f;
		TutorialImage2* Tutorial;

	public:
    GameManager2()
    {
        REGISTER_BEHAVIOUR_MESSAGE(Awake);
        REGISTER_BEHAVIOUR_MESSAGE(OnDestroy);
        REGISTER_BEHAVIOUR_MESSAGE(Start);
        REGISTER_BEHAVIOUR_MESSAGE(Update);
    }
		int winner;
		float GameTimer = 180.0f;
		float CropGauge = 100.0f;
		float endingTimer = 3.0f;
		static GameManager2* instance;
		bool setactive = false;
		void Awake();
		void Start();
		void Update();
		void OnDestroy();
	};
}