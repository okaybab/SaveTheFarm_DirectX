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
#include <ButtonAnimation.h>

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
		ButtonAnimation* physAnimation;  //사실 버튼 애니메이션이 아닙니다.. 물리를 사용하는 애니메이션인데 버튼에만 쓰려고 클래스 이름을 지어놓았다가 낭패를 본 케이스입니다.. 물리애니메이션이에요

		void OnSceneLoaded();

		void Awake();

		void OnEnable();

		void OnDestroy();

		void Update();

		void TriggerModeUdpate();
		void HoldModeUdpate();
		void FullAutoModeUdpate();

		void ChangeMode(CrosshairFireMode mode);
		void OnEnter(CrosshairFireMode mode);
		void OnExit(CrosshairFireMode mode);
	};
}