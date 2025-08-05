#pragma once
#include <ScriptBehaviour.h>
#include <TimeManager.h>
#include <RenderManager.h>
#include <SpriteRenderer.h>
#include <InputManager.h>
#include "Canvas.h"
#include "Image.h"
#include "RectTransform.h"
#include "Screen.h"

namespace GOTOEngine
{
	class GimmickManager : public ScriptBehaviour
	{
	private:
		float p1gimmick1Timer = 0.0f;
		float p2gimmick1Timer = 0.0f;
		float p1gimmick2Timer = 0.0f;
		float p2gimmick2Timer = 0.0f;
		const float timelimit = 3.0f;
		Image* p1gimmick2Image;
		Image* p2gimmick2Image;
	public:
    GimmickManager()
    {
        REGISTER_BEHAVIOUR_MESSAGE(Awake);
        REGISTER_BEHAVIOUR_MESSAGE(OnDestroy);
        REGISTER_BEHAVIOUR_MESSAGE(Update);
    }
		static GimmickManager* instance;
		void Awake();
		void Update();
		void OnDestroy();
		void GimmickOn(int player, int gimmick);
	};
}