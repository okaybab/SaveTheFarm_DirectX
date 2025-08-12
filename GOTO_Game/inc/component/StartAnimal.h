#pragma once
#include <ScriptBehaviour.h>
#include <InputManager.h>
#include <RenderManager.h>
#include <SpriteRenderer.h>
#include <Animator.h>
#include <Collider2D.h>
#include <AnimationCurve.h>
#include <ParticleSystem.h>
#include "Screen.h"

namespace GOTOEngine
{
	class StartAnimal : public ScriptBehaviour
	{
	private:
		GameObject* crow = new GameObject;
		GameObject* goldcrow = new GameObject;
		GameObject* thiefmole = new GameObject;
		AnimatorController* crowanimator;
		AnimatorController* goldcrowanimator;
		AnimatorController* thiefmoleanimator;
		Sprite* crowdie;
		Sprite* golddie;
		Sprite* thiefdie;
		bool crowup = false;
		bool thiefleft = false;
		bool crowhit = false;
		bool goldhit = false;
		bool thiefhit = false;
		static AnimationCurve wobbleXCurve;
		static AnimationCurve wobbleYCurve;

		Vector2 crowSavedPos;
		Vector2 goldcrowSavedPos;
		Vector2 thiefmoleSvaedPos;
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
		float crowhittimer = 0.0f;
		float goldhittimer = 0.0f;
		float thiefhittimer = 0.0f;

		ParticleSystem* crowParticle;
		ParticleSystem* goldParticle;
		ParticleSystem* thiefParticle;
	};
}