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
	class TutorialImage : public ScriptBehaviour
	{
	private:
		Image* explanation;
		Image* p1button;
		Image* p2button;
		int explainnum = 1;
		std::vector<GameObject*> Tutorialobject;
		bool rightTriggerCheckTrigger[2];
		bool rightTriggerPressed[2];
		bool leftTriggerCheckTrigger[2];
		bool leftTriggerPressed[2];

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
	};
}

