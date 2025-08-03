#pragma once
#include <ScriptBehaviour.h>
#include <GameObject.h>
#include <Delegate.h>
#include <CrosshairCollide.h>
#include <RadialSpriteRenderer.h>
#include <ParticleSystem.h>
#include <TextRenderer.h>

#include "IAttackAble.h"
#include "GamepadRumbleManager.h"
#include "RumbleAnimationClip.h"


namespace GOTOEngine
{
	class EnhancedCrosshairFire : public ScriptBehaviour
	{
	private:
		CrosshairCollide* m_collider = nullptr; // Collider2D 컴포넌트
		float m_fireCooldown = 0.0f; // 발사 쿨타임

		bool m_RightTriggerCheckTrigger = false;
		bool m_RightTriggerPressed = false;

		float m_fireGage = 0.0f;

		bool m_startDrop = false;

		int m_strCount = 0;

		static RumbleAnimationClip* s_pfireRumbleClip;
		static int s_crosshairCount;
	public:
    EnhancedCrosshairFire()
    {
        SetExecutionOrder(10);
        REGISTER_BEHAVIOUR_MESSAGE(Awake);
        REGISTER_BEHAVIOUR_MESSAGE(OnDestroy);
        REGISTER_BEHAVIOUR_MESSAGE(OnEnable);
        REGISTER_BEHAVIOUR_MESSAGE(Update);
    }

		float fireRate = 0.5f; // 발사 간격
		float fireGageUpRate = 1.25f;
		int id = 0;
		Delegate<void, int> onFire;
		RadialSpriteRenderer* gageSprite = nullptr;
		ParticleSystem* dropParticleSys;
		TextRenderer* strText;
		void Awake();

		void OnEnable();

		void OnDestroy();

		void TriggerPressedCheck();

		void TriggerPressedCheckReset();

		void Update();
	};
}