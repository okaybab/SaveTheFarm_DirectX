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
#include <ParticleSystem.h>
#include <TextRenderer.h>

namespace GOTOEngine
{
	enum class CrosshairFireMode
	{
		Trigger = 0,
		Hold,
		FullAuto,
		Count,  //마지막 다음 상태를 나타내는 용도, 절대 모드값으로 사용하지 않기
	};

	// ++ 연산자 오버로드
	CrosshairFireMode& operator++(CrosshairFireMode& state);

	class CrosshairFire : public ScriptBehaviour
	{
	private:
		//모드 공용 멤버
		CrosshairFireMode m_fireMode = CrosshairFireMode::Trigger;
		CrosshairCollide* m_collider = nullptr; // Collider2D 컴포넌트
		float m_fireCooldown = 0.0f; // 발사 쿨타임
		float m_fireGage = 0.0f;

		bool m_RightTriggerCheckTrigger;
		bool m_RightTriggerPressed;

		CameraShaker* m_shaker = nullptr;
		
		static RumbleAnimationClip* s_pfireRumbleClip;

		int m_rumbleAnimID = -1;

		//홀드 모드
		Vector2 m_shakeMove;
		bool m_charged = false;

		bool m_startDrop = false;
		int m_noiseSeed = 1294;
		int m_strCount = 0;

		int m_holdingRumbleAnimID = -1;

		float m_nextPlayHoldingRumbleTime = 0.0f;

		static RumbleAnimationClip* s_pholdRumbleClip;
		static RumbleAnimationClip* s_pholdLoopRumbleClip;
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
		float fireGageUpRate = 1.25f;

		int id = 0;
		Delegate<void,int> onFire;
		Delegate<void,int> onCharge;

		RadialSpriteRenderer* gageSprite;

		ParticleSystem* dropParticleSys;
		TextRenderer* strText;

		void OnSceneLoaded();

		void Awake();

		void OnEnable();

		void OnDestroy();
		
		void TriggerPressedCheck();
		void TriggerPressedCheckReset();

		void Update();

		void TriggerModeUdpate();
		void HoldModeUdpate();
		void FullAutoModeUdpate();

		void ChangeMode(CrosshairFireMode mode);
		void OnEnter(CrosshairFireMode mode);
		void OnExit(CrosshairFireMode mode);
	};
}