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
		void Dispose() override
		{
			__super::Dispose();
		}
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
				AddComponent<SpriteRenderer>()->SetSprite(L"../Resources/artResource/Sprint/Mole.png");
				SetRandomYPosition(-0.4f, -0.1f);
				GetTransform()->SetLossyScale({ 0.12f, 0.12f });
				break;
			case crow_1:
				m_moveFlag = 0b0001;
				m_disPoneTime = 10.0f;
				GetGameObject()->name = L"까마귀";
				AddComponent<SpriteRenderer>()->SetSprite(L"../Resources/artResource/Sprint/Crow.png");
				SetRandomYPosition(0.15f, 0.4f);
				GetTransform()->SetLossyScale({ 0.2f, 0.2f });
				break;
			case crow_2:
				m_moveFlag = 0b0010;
				m_disPoneTime = 10.0f;
				GetGameObject()->name = L"까마귀";
				AddComponent<SpriteRenderer>()->SetSprite(L"../Resources/artResource/Sprint/Crow.png");
				SetRandomYPosition(0.15f, 0.4f);
				GetTransform()->SetLossyScale({ 0.2f, 0.2f });
				break;
			}
			AddComponent<Animator>()->SetAnimatorController(EnemySpawner::instance->GetAnimation(GetGameObject()->name));
			GetComponent<SpriteRenderer>()->SetRenderLayer((1 << m_layer));
			
			auto spriteRect = GetComponent<SpriteRenderer>()->GetSprite()->GetRect();
			auto localScale = GetTransform()->GetLossyScale();
			auto collider = AddComponent<Collider2D>();

			collider->SetSize({ spriteRect.width * localScale.x , spriteRect.height * localScale.y });

			SetMovementComponents(0.15f, 0.4f);
		}

		int GetType() { return static_cast<int>(m_moveEnemyType); }
		
		void OnBulletDie() override
		{
			__super::OnBulletDie();

			// 죽는 애니메이션 필요
			Destroy(GetGameObject(), 0.5f);
		}
	};
}