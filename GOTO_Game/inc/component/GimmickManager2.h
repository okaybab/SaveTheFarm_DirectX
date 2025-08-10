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
	class GimmickManager2 : public ScriptBehaviour
	{
	private:
		float gimmick5Timer = 0.0f;

	public:
    GimmickManager2()
    {
        REGISTER_BEHAVIOUR_MESSAGE(Awake);
        REGISTER_BEHAVIOUR_MESSAGE(OnDestroy);
        REGISTER_BEHAVIOUR_MESSAGE(Update);
    }
		static GimmickManager2* instance;
		void Awake();
		void Update();
		void OnDestroy();
		void GimmickOn(int player, int gimmick);
	};
}