#pragma once
#include <ScriptBehaviour.h>
#include <GameObject.h>
#include <Delegate.h>
#include <CrosshairCollide.h>
#include "IAttackAble.h"
#include "GamepadRumbleManager.h"
#include "RumbleAnimationClip.h"
#include <RadialSpriteRenderer.h>
#include "CameraShaker.h"

namespace GOTOEngine
{
	class CrosshairFire : public ScriptBehaviour
	{
	private:
		CrosshairCollide* m_collider = nullptr; // Collider2D 컴포넌트
		float m_fireCooldown = 0.0f; // 발사 쿨타임

		bool m_RightTriggerCheckTrigger;
		bool m_RightTriggerPressed;

		CameraShaker* m_shaker = nullptr;

		static RumbleAnimationClip* s_pfireRumbleClip;
		static int s_crosshairCount;
	public:
    CrosshairFire()
    {
        SetExecutionOrder(10);
        REGISTER_BEHAVIOUR_MESSAGE(Awake);
        REGISTER_BEHAVIOUR_MESSAGE(OnDestroy);
        REGISTER_BEHAVIOUR_MESSAGE(OnEnable);
        REGISTER_BEHAVIOUR_MESSAGE(OnSceneLoaded);
        REGISTER_BEHAVIOUR_MESSAGE(Update);
    }

		float fireRate = 0.5f; // 발사 간격
		int id = 0;
		Delegate<void,int> onFire;
		Delegate<void,int> onCharge;

		RadialSpriteRenderer* gageSprite;

		void Awake();

		void OnEnable();

		void OnDestroy();
		
		void TriggerPressedCheck();

		void TriggerPressedCheckReset();

		void Update();

		void OnSceneLoaded();
	};
}