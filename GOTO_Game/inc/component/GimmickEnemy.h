#pragma once
#include "BaseEnemyObject.h"

#include <SpriteRenderer.h>
#include <Collider2D.h>

#include "GimmickManager.h"

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
				if (m_layer == 1)
				{
					GameManager::instance->P1Score -= 1;
				}
				else if (m_layer == 2)
				{
					GameManager::instance->P2Score -= 1;
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
				AddComponent<SpriteRenderer>();
				SetRandomYPosition(-0.4f, -0.1f);
				GetTransform()->SetLossyScale({ 0.3f, 0.3f });
				break;
			case squirrel:
				m_moveFlag = 0b1001;
				m_disPoneTime = 9.0f;
				GetGameObject()->name = L"다람쥐";
				AddComponent<SpriteRenderer>();
				SetRandomYPosition(-0.4f, -0.1f);
				GetTransform()->SetLossyScale({ 0.2f, 0.2f });
				break;
			case thiefmole:
				m_moveFlag = 0b0001;
				m_disPoneTime = 9.0f;
				GetGameObject()->name = L"도둑두더지";
				AddComponent<SpriteRenderer>();
				SetRandomYPosition(-0.4f, -0.1f);
				GetTransform()->SetLossyScale({ 0.12f, 0.12f });
				break;
			}
			AddComponent<Animator>()->SetAnimatorController(EnemySpawner::instance->GetAnimation(GetGameObject()->name));
			GetComponent<SpriteRenderer>()->SetRenderLayer((1 << m_layer));

			auto spriteRect = EnemySpawner::instance->GetSprite(GetGameObject()->name)->GetRect();
			auto localScale = GetTransform()->GetLossyScale();
			auto collider = AddComponent<Collider2D>();

			collider->SetSize({ spriteRect.width * localScale.x , spriteRect.height * localScale.y });

			SetMovementComponents();
		}

		int GetType() override { return static_cast<int>(m_gimmickEnemyType); }

		void OnBulletDie() override
		{
			__super::OnBulletDie();
			GetGameObject()->GetComponent<Animator>()->SetEnabled(false);
			GetGameObject()->GetComponent<SpriteRenderer>()->SetSprite(EnemySpawner::instance->GetSprite(GetGameObject()->name));

			// 죽는 애니메이션 필요
			Destroy(GetGameObject(), 0.5f);
		}
	};
}