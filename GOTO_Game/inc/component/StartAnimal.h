#pragma once
#include <ScriptBehaviour.h>
#include <InputManager.h>
#include <RenderManager.h>
#include <SpriteRenderer.h>
#include <Animator.h>
#include "Screen.h"

namespace GOTOEngine
{
	class StartAnimal : public ScriptBehaviour
	{
	private:
		GameObject* crow1;
		GameObject* crow2;
		GameObject* mole;
		Animator* crow;

	public:
		StartAnimal()
		{
			REGISTER_BEHAVIOUR_MESSAGE(Awake);
			REGISTER_BEHAVIOUR_MESSAGE(OnDestroy);
			REGISTER_BEHAVIOUR_MESSAGE(Update);
		}
		void Awake();
		void Update();
		void OnDestroy();
	};
}