#pragma once
#include <ScriptBehaviour.h>
#include <TimeManager.h>
#include <SpriteRenderer.h>
#include "Transform.h"
#include "Screen.h"

#include <string.h>
#include <iostream>
#include <any>

#include "IAttackAble.h"
#include "GameManager.h"
#include "EnemySpawner.h"

// move
#include "BaseMovement.h"
#include "MovementLeftRight.h"
#include "MovementUpDown.h"
#include "MovementCircle.h"
#include "MovementParabolic.h"
#include "SoundManager.h"

namespace GOTOEngine
{
	enum E_EnemyType
	{
		move,
		gimmick,
		itemspawn,
		competition
	};
	enum E_Enemy_Anim_State
	{
		IDLE,
		MOVE,
		DIE,
		ESCAPE
	};
	inline std::wstring StateToString(E_Enemy_Anim_State state)
	{
		switch (state)
		{
		case E_Enemy_Anim_State::IDLE:   return L"Idle";
		case E_Enemy_Anim_State::MOVE:   return L"Move";
		case E_Enemy_Anim_State::DIE:    return L"Die";
		case E_Enemy_Anim_State::ESCAPE: return L"Escape";
		default:                         return L"";
		}
	}

	class BaseEnemyObject : public ScriptBehaviour,
							public IAttackAble
	{
	private:
		// move
		std::vector<BaseMovement*> m_movementComponents;
		Vector2 m_currentPathPosition;	// 중심축 위치 변수

	protected:
		E_EnemyType m_enemyType = E_EnemyType::move;
		E_Enemy_Anim_State m_animState = E_Enemy_Anim_State::MOVE;

		// move
		int m_moveFlag;
		bool m_isMoveLoop = true;

		// hp, score
		float m_enemyHp = 1.0f;
		float m_DieScore = 1.0f;

		// 디스폰 시간
		float m_disPoneTime = 8.0f;

		// 상태 값
		bool m_isDie = false;
		bool m_isDeathByDispone = false;
		bool m_isFrozen = false;

		// player layer
		std::uint32_t m_layer = 1;

	public:
    BaseEnemyObject()
    {
        REGISTER_BEHAVIOUR_MESSAGE(Awake);
        REGISTER_BEHAVIOUR_MESSAGE(OnDestroy);
        REGISTER_BEHAVIOUR_MESSAGE(OnDisable);
        REGISTER_BEHAVIOUR_MESSAGE(OnEnable);
        REGISTER_BEHAVIOUR_MESSAGE(Update);
    }
		
	public:
		virtual ~BaseEnemyObject() = default;
		virtual void Awake() {}
		void Update()
		{
			if (m_isDeathByDispone || m_isDie || m_isFrozen || m_moveFlag & 0b0000 )
			{
				return;
			}

			float deltaTime = TimeManager::Get()->GetDeltaTime();
			m_disPoneTime -= deltaTime;

			if (m_disPoneTime <= 0.0f)
			{
				// 디스폰
				m_isDeathByDispone = true;
				SetState(E_Enemy_Anim_State::ESCAPE);
				return;
			}

			// 이번 프레임의 '중심축' 이동량
			Vector2 pathDisplacement = Vector2::Zero();
			// 이번 프레임의 '총 오프셋'
			Vector2 totalOffset = Vector2::Zero();

			for (BaseMovement* movement : m_movementComponents)
			{
				Vector2 moveVec = movement->Move(deltaTime);
				if (movement->GetRole() == E_Move_Role::PATH)
				{
					pathDisplacement += moveVec;
				}
				else // EMoveRole::OFFSET
				{
					totalOffset += moveVec;
				}
			}

			m_currentPathPosition += pathDisplacement;
			Vector2 newPos = m_currentPathPosition + totalOffset;

			GetGameObject()->GetTransform()->SetPosition(newPos);

		}
		virtual void OnEnable() 
		{
		}
		virtual void OnDisable() 
		{

		}
		virtual void OnDestroy() 
		{
			m_isDeathByDispone = true;
		}
		
		virtual void Initialize(std::any param) = 0;

		// Get
		bool IsEnemyDie() { return m_isDie; }
		virtual int GetType() { return static_cast<int>(m_enemyType); }

		// Set
		void SetMovementComponents(float _minY = 0.0f, float _maxY = 0.5f)
		{
			// flag 스크립트	부착
			if (m_moveFlag & MOVE_CIRCULAR) // 0b0100
			{
				AddComponent<MoveCircle>();
			}
			if ( m_moveFlag & MOVE_PARABOLIC ) // 0b1000
			{
				auto comp = AddComponent<MovementParabolic>();
				comp->OnFlipDirection.Add(this, &BaseEnemyObject::SetFlipXSprite);
				comp->Initialize(Screen::GetWidth() * -0.25f - 420.0f, Screen::GetWidth() * 0.25f + 420.0f);
			}
			if(!(m_moveFlag & MOVE_PARABOLIC && m_moveFlag & MOVE_LEFT_RIGHT && m_moveFlag & MOVE_UP_DOWN)) // 1011 == 1000
			{
				if (m_moveFlag & MOVE_LEFT_RIGHT) // 0b0001
				{
					auto comp = AddComponent<MovementLeftRight>();
					comp->OnFlipDirection.Add(this, &BaseEnemyObject::SetFlipXSprite);
					comp->Initialize(Screen::GetWidth() * -0.25f - 420.0f, Screen::GetWidth() * 0.25f + 420.0f);
				}
				if (m_moveFlag & MOVE_UP_DOWN) // 0b0010
				{
					auto comp = AddComponent<MovementUpDown>();
					comp->Initialize(Screen::GetHeight() * _minY, Screen::GetHeight() * _maxY);
				}
			}
			
			// 등록한 movement들 추가
			m_movementComponents = GetGameObject()->GetComponents<BaseMovement>();

			for (auto comp : m_movementComponents)
			{
				comp->Initialize(m_moveFlag, GetGameObject()->GetTransform()->GetPosition());
			}

		}
		void SetRandomYPosition(float minY, float maxY)
		{
			// 가로 크기 고정 X는 추후에 변동하면 추가
			float randomX = Screen::GetWidth() * EnemySpawner::GenerateRandom(-0.25f, 0.25f);
			float randomY = Screen::GetHeight() * EnemySpawner::GenerateRandom(minY, maxY);

			GetTransform()->SetPosition({ randomX, randomY });
			// OFFSET 경로의 기준선
			m_currentPathPosition = { randomX, randomY };
		}
		void SetFlipXSprite()
		{
			auto spriterenderer = GetComponent<SpriteRenderer>();
			if (spriterenderer) spriterenderer->SetFlipX(!spriterenderer->GetFlipX());
		}
		void SetEnemyFrozen(bool _frozen)
		{
			m_isFrozen = _frozen;
		}
		void SetEnemyLayer(std::uint32_t _player = 1 << 1)
		{
			m_layer = _player;
		}
		void SetState(E_Enemy_Anim_State state)
		{
			m_animState = state;
			GetGameObject()->GetComponent<Animator>()->GetRuntimeAnimatorController()->ForceChangeState(StateToString(state));
		}

		// 이벤트
		virtual void TakeDamage(int attackerID, float damage)
		{
			if (m_isDie || m_isDeathByDispone) return;
			m_enemyHp -= damage;
			if (m_enemyHp <= 0) OnBulletDie(attackerID);
		}
		void OnBulletDie(int attackerID)
		{
			m_isDie = true;

			GameManager::instance->PointChange(attackerID + 1, 1);
			int& point = attackerID == 0 ? GameManager::instance->P1Catch : GameManager::instance->P2Catch;
			point++;
			SoundManager::instance->PlaySFX("Hit");

			OnDie(attackerID + 1); // player는 0, 1값으로 들어옴
		}
		virtual void OnDie(int attackerID) = 0;
	};
}