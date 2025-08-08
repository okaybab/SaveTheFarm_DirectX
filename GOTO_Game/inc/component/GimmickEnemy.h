#pragma once
#include "BaseEnemyObject.h"

#include <SpriteRenderer.h>
#include <Collider2D.h>

#include "GimmickManager.h"
#include "FadeComponent.h"

namespace GOTOEngine
{
	enum E_Gimmick_Enemy_Type
	{
		rabbit,		// 토끼
		squirrel,	// 다람쥐
		thiefmole,	// 도둑두더지
		gimmick_type_count
	};

	class GimmickEnemy : public BaseEnemyObject
	{
		E_Gimmick_Enemy_Type m_gimmickEnemyType;

	public:
		void Dispose()
		{
			if (!GameManager::instance->setactive) return;

			if (m_isDeathByDispone)
			{
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
			else
			{
				// 기믹 호출 GimmickManager::GimmickOn()
				GimmickManager::instance->GimmickOn(m_layer, m_gimmickEnemyType + 1);
			}
		}
		void Initialize(std::any param) override
		{
			if (param.type() == typeid(E_Gimmick_Enemy_Type)) m_gimmickEnemyType = std::any_cast<E_Gimmick_Enemy_Type>(param);
		}
		void Awake()
		{
			__super::Awake();

			m_enemyType = E_EnemyType::gimmick;
			m_isMoveLoop = true;

			switch (m_gimmickEnemyType)
			{
			case rabbit:
				m_moveFlag = 0b1001;
				m_disPoneTime = 9.0f;
				GetGameObject()->name = L"토끼";
				SetRandomYPosition(-0.3f, -0.1f);
				GetTransform()->SetLossyScale({ 0.24f, 0.24f });
				break;
			case squirrel:
				m_moveFlag = 0b1001;
				m_disPoneTime = 9.0f;
				GetGameObject()->name = L"다람쥐";
				SetRandomYPosition(-0.3f, -0.1f);
				GetTransform()->SetLossyScale({ 0.3f, 0.3f });
				break;
			case thiefmole:
				m_moveFlag = 0b0001;
				m_disPoneTime = 9.0f;
				GetGameObject()->name = L"도둑두더지";
				SetRandomYPosition(-0.3f, -0.1f);
				GetTransform()->SetLossyScale({ 0.15f, 0.15f });
				break;
			}
			AddComponent<SpriteRenderer>()->SetRenderLayer(m_layer);
			AddComponent<FadeComponent>();
			AddComponent<Animator>()->SetAnimatorController(EnemySpawner::instance->GetAnimation(GetGameObject()->name));

			auto controller = GetComponent<Animator>()->GetRuntimeAnimatorController();
			controller->SetOnAnimationEnd([this]() {
				if (m_animState == DIE)
				{
					Destroy(GetGameObject());
				}

				if (m_animState == ESCAPE)
				{
					Destroy(GetGameObject());
				}
			});
			
			auto spriteRect = EnemySpawner::instance->GetSprite(GetGameObject()->name)->GetRect();
			auto localScale = GetTransform()->GetLossyScale();
			auto collider = AddComponent<Collider2D>();

			collider->SetSize({ spriteRect.width * localScale.x , spriteRect.height * localScale.y });

			SetMovementComponents();
		}

		int GetType() override { return static_cast<int>(m_gimmickEnemyType); }
		void OnDie(int attackerID) override
		{
			__super::OnDie(attackerID);
			EnemySpawner::instance->SetDeleteEnemy(m_layer, GetGameObject());
		}
	};
}