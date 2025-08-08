#pragma once
#include "BaseEnemyObject.h"

#include <SpriteRenderer.h>
#include <Collider2D.h>

namespace GOTOEngine
{
	enum E_Move_Enemy_Type
	{
		mole,	// 두더지
		crow_1,	// 까마귀1 (좌우 이동)
		crow_2,	// 까마귀2 (상하 이동)
		move_type_count
	};

	class MoveEnemy : public BaseEnemyObject
	{
		E_Move_Enemy_Type m_moveEnemyType;

	public:
		void Initialize(std::any param) override
		{
			if (param.type() == typeid(E_Move_Enemy_Type)) m_moveEnemyType = std::any_cast<E_Move_Enemy_Type>(param);
		}
		void Awake()
		{
			__super::Awake();

			m_enemyType = E_EnemyType::move;
			m_isMoveLoop = true;

			switch (m_moveEnemyType)
			{
			case mole:
				m_moveFlag = 0b0000;
				GetGameObject()->name = L"두더지";
				m_disPoneTime = 8.0f;
				SetRandomYPosition(-0.3f, -0.1f);
				GetTransform()->SetLossyScale({ 0.12f, 0.12f });
				break;
			case crow_1:
				m_moveFlag = 0b0001;
				m_disPoneTime = 10.0f;
				GetGameObject()->name = L"까마귀";
				SetRandomYPosition(0.15f, 0.4f);
				GetTransform()->SetLossyScale({ 0.3f, 0.3f });
				break;
			case crow_2:
				m_moveFlag = 0b0010;
				m_disPoneTime = 10.0f;
				GetGameObject()->name = L"까마귀";
				SetRandomYPosition(0.15f, 0.4f);
				GetTransform()->SetLossyScale({ 0.3f, 0.3f });
				break;
			}
			AddComponent<SpriteRenderer>()->SetRenderLayer(m_layer);
			AddComponent<Animator>()->SetAnimatorController(EnemySpawner::instance->GetAnimation(GetGameObject()->name));
			
			auto controller = GetComponent<Animator>()->GetRuntimeAnimatorController();
			controller->SetOnAnimationEnd([this, controller]() {
				if (m_animState == DIE || m_animState == ESCAPE)
				{
					controller->SetOnAnimationEnd(nullptr);
					GameObject::Destroy(GetGameObject());
				}
			});

			auto spriteRect = EnemySpawner::instance->GetSprite(GetGameObject()->name)->GetRect();
			auto localScale = GetTransform()->GetLossyScale();
			auto collider = AddComponent<Collider2D>();

			collider->SetSize({ spriteRect.width * localScale.x , spriteRect.height * localScale.y });

			SetMovementComponents(0.15f, 0.4f);
		}

		int GetType() { return static_cast<int>(m_moveEnemyType); }
		void OnDie(int attackerID) override
		{
			__super::OnDie(attackerID);
			EnemySpawner::instance->SetDeleteEnemy(m_layer, GetGameObject(), true);
		}
		void OnDispone() override
		{
			__super::OnDispone();
			EnemySpawner::instance->SetDeleteEnemy(m_layer, GetGameObject());
			if (m_layer & 1 << 1)
			{
				GameManager::instance->PointChange(1, -1);
				GameManager::instance->P1Lost++;
			}
			else if (m_layer & 1 << 2)
			{
				GameManager::instance->PointChange(2, -1);
				GameManager::instance->P2Lost++;
			}
		}
	};
}