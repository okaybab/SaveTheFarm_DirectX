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
	class EnemySpawner;
	class TutorialImage;
	class GameManager : public ScriptBehaviour
	{
	private:
		bool p1active = false;
		bool p2active = false;
		float NormalTiming = 125.0f;
		float GimmickTiming = 120.0f;
		float ItemTiming[6] = { 100.0f, 80.0f, 60.0f, 40.0f, 20.0f, 10.0f };

		Image* scoreboard;
		Image* timerscreen;
		Image* p1score;
		Image* p2score;
		Text* P1sctext;
		Text* P2sctext;
		Text* Timetext;
		Text* P1wintext;
		Text* P2wintext;

		int p1itemchange;
		int p2itemchange;

		int totalSeconds;
		int minutes;
		int seconds;

		float tutorialCheckTime = 0.0f;
		TutorialImage* Tutorial;
		EnemySpawner* EnemySpawner;

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
		int winner;
		float GameTimer = 120.0f;
		float endingTimer = 3.0f;
		static GameManager* instance;
		bool setactive = false;
		void Awake();
		void Start();
		void Update();
		void OnDestroy();
		void PointChange(int player, int point);
	};
}