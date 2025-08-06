#pragma once
#include <ScriptBehaviour.h>
#include <GameObject.h>
#include <Delegate.h>
#include <CrosshairCollide.h>
#include <RadialSpriteRenderer.h>
#include <ParticleSystem.h>
#include <TextRenderer.h>
#include <Mathf.h>

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
		bool m_charged = false;

		int m_strCount = 0;

		int m_rumbleAnimID = -1;
		int m_holdingRumbleAnimID = -1;

		float m_nextPlayHoldingRumbleTime = 0.0f;

		CameraShaker* m_shaker = nullptr;

		Vector2 m_shakeMove;

		static RumbleAnimationClip* s_pfireRumbleClip;
		static RumbleAnimationClip* s_pholdRumbleClip;
		static RumbleAnimationClip* s_pholdLoopRumbleClip;
		static int s_crosshairCount;

		int m_noiseSeed = 1294;
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
		Delegate<void, int> onCharge;
		RadialSpriteRenderer* gageSprite = nullptr;
		ParticleSystem* dropParticleSys;
		TextRenderer* strText;

		int GetCurrentStrength() { return m_strCount; }

		void Awake();

		void OnEnable();

		void OnDestroy();

		void TriggerPressedCheck();

		void TriggerPressedCheckReset();

		void Update();

		void OnSceneLoaded();
	};
}