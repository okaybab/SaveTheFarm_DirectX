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
	class EnemySpawnManager;
	class GameManager2 : public ScriptBehaviour
	{
	private:
		bool p1active = false;
		bool p2active = false;

		Image* warningImage;
		Sprite* warningsprite;

		Image* waveImage;
		Sprite* wave1sprite;
		Sprite* wave2sprite;
		Sprite* wave3sprite;

		AnimationCurve* warninganimation;
		float warningAniTime = 0.0f;
		AnimationCurve* waveanimation;
		float waveAniTime = 0.0f;

		Image* endpannel;
		Sprite* winpannel;
		Sprite* losepannel;
		Text* catchtext;
		Text* croptext;
		Image* focus;
		Sprite* focusui;
		Image* retry;
		Image* totitle;
		Sprite* retrybutton;
		Sprite* totitlebutton;
		int focuschoice = 1;
		bool pannelopen = false;
		bool changestart = false;
		bool reset = false;

		Image* wavebarout;
		Image* wavebarin;
		Rect texRect;
		Image* wave1check;
		Image* wave2check;
		Image* wave3check;
		Sprite* wave1bar;
		Sprite* wave2bar;
		Sprite* wave3bar;

		float NormalTiming = 180.0f;
		float GimmickTiming = 180.0f;
		float Gimmick2Timing = 180.0f;
		float NormalWarningTiming = 180.0f;
		float GimmickWarningTiming = 180.0f;
		float Gimmick2WarningTiming = 180.0f;

		int wave = 0;
		float waveTiming[3] = { 180.0f, 120.0f, 60.0f };
		float BGMTiming[3] = { 180.0f, 180.0f, 0.0f };
		float warningsoundTiming[3] = {141.0f, 81.0f, 21.0f};

		bool warningon = false;

		float tutorialCheckTime = 0.0f;
		TutorialImage2* Tutorial;

		SpriteRenderer* crop1;
		SpriteRenderer* crop2;
		Sprite* crop14;
		Sprite* crop13;
		Sprite* crop12;
		Sprite* crop11;
		Sprite* crop24;
		Sprite* crop23;
		Sprite* crop22;
		Sprite* crop21;

		bool m_YpressedUpTrigger[2];
		bool m_YpressedDownTrigger[2];

		bool m_YstickPressedUp[2];
		bool m_YstickPressedDown[2];

		EnemySpawnManager* EnemySpawnManager;

	public:
    GameManager2()
    {
        REGISTER_BEHAVIOUR_MESSAGE(Awake);
        REGISTER_BEHAVIOUR_MESSAGE(OnDestroy);
        REGISTER_BEHAVIOUR_MESSAGE(Start);
        REGISTER_BEHAVIOUR_MESSAGE(Update);
		REGISTER_BEHAVIOUR_MESSAGE(OnSceneLoaded);
    }
		int winner;
		float GameTimer = 180.0f;
		int CropGauge = 8;
		int animalcatch;
		static GameManager2* instance;
		bool setactive = false;
		void Awake();
		void Start();
		void Update();
		void OnDestroy();
		void OnSceneLoaded();
		void StickPressedCheck();
		void StickPressedCheckReset();
	};
}