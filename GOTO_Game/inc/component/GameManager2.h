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

		Image* waveImage;
		Sprite* wave1sprite;
		Sprite* wave2sprite;
		Sprite* wave3sprite;

		int totalSeconds;
		AnimationCurve* warninganimation;
		float warningAniTime = 0.0f;
		AnimationCurve* waveanimation;
		float waveAniTime = 0.0f;

		Image* endpannel;
		Sprite* winpannel;
		Sprite* losepannel;
		Text* catchtext;
		Text* croptext;

		Image* wavebarout;
		Image* wavebarin;
		Rect texRect;
		Image* wave1check;
		Image* wave2check;
		Image* wave3check;
		Sprite* wave1bar;
		Sprite* wave2bar;
		Sprite* wave3bar;

		int itemchange;
		float NormalTiming = 120.0f;
		float GimmickTiming = 120.0f;
		float Gimmick2Timing = 120.0f;
		float NormalWarningTiming = 120.0f;
		float GimmickWarningTiming = 120.0f;
		float Gimmick2WarningTiming = 120.0f;

		int wave = 0;
		float waveTiming[3] = { 180.0f, 120.0f, 60.0f };

		float warningsoundTiming[3] = {141.0f, 81.0f, 21.0f};

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
		int CropGauge = 8;
		float endingTimer = 3.0f;
		int animalcatch;
		static GameManager2* instance;
		bool setactive = false;
		void Awake();
		void Start();
		void Update();
		void OnDestroy();
	};
}