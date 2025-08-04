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
	class GameManager2 : public ScriptBehaviour
	{
	private:
		bool p1active = false;
		bool p2active = false;

		Text* Timetext;

		int totalSeconds;
		int minutes;
		int seconds;

	public:
    GameManager2()
    {
        REGISTER_BEHAVIOUR_MESSAGE(Awake);
        REGISTER_BEHAVIOUR_MESSAGE(OnDestroy);
        REGISTER_BEHAVIOUR_MESSAGE(Start);
        REGISTER_BEHAVIOUR_MESSAGE(Update);
    }
		int winner;
		float GameTimer = 120.0f;
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