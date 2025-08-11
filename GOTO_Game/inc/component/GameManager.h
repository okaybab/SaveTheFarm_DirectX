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
	class EnemySpawnManager;
	class TutorialImage;
	class AnimationCurve;
	class GameManager : public ScriptBehaviour
	{
	private:
		bool p1active = false;
		bool p2active = false;
		float NormalTiming = 125.0f;
		float GimmickTiming = 120.0f;
		float ItemTiming[6] = { 100.0f, 80.0f, 60.0f, 40.0f, 20.0f, 10.0f };
		float GoldTiming[2] = { 95.0f, 50.0f };
		float BGMTiming[3] = { 120.0f, 30.0f, 0.0f };
		float scoreredTiming = 30.0f;

		Image* scoreboard;
		Image* timerscreen;
		Image* p1score;
		Image* p2score;
		Text* P1sctext;
		Text* P2sctext;
		Text* Timetext;

		Sprite* winpannel;
		Sprite* losepannel;
		Sprite* deucepannel;
		Image* p1pannel;
		Image* p2pannel;
		Text* p1finalsctext;
		Text* p2finalsctext;
		Text* p1losttext;
		Text* p2losttext;
		Text* p1catchtext;
		Text* p2catchtext;
		Text* p1timetext;
		Text* p2timetext;

		int p1itemchange;
		int p2itemchange;

		int totalSeconds;
		int minutes;
		int seconds;

		float tutorialCheckTime = 0.0f;
		TutorialImage* Tutorial;
		AnimationCurve* scoreeffect;
		float p1scoreAniTime = 0.0f;
		bool p1scoreup = false;
		float p2scoreAniTime = 0.0f;
		bool p2scoreup = false;

		EnemySpawnManager* EnemySpawnManager;

	public:
    GameManager()
    {
        REGISTER_BEHAVIOUR_MESSAGE(Awake);
        REGISTER_BEHAVIOUR_MESSAGE(OnDestroy);
        REGISTER_BEHAVIOUR_MESSAGE(Start);
        REGISTER_BEHAVIOUR_MESSAGE(Update);
    }
		int P1Score = 0;
		int P2Score = 0;
		int P1Bonus = 1;
		int P2Bonus = 1;
		int P1Lost = 0;
		int P2Lost = 0;
		int P1Catch = 0;
		int P2Catch = 0;
		int winner;
		float GameTimer = 120.0f;
		float endingTimer = 5.0f;
		static GameManager* instance;
		bool setactive = false;
		void Awake();
		void Start();
		void Update();
		void OnDestroy();
		void PointChange(int player, int point);
	};
}