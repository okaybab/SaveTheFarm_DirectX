#pragma once
#include "BaseEnemyObject.h"

#include <SpriteRenderer.h>
#include <Collider2D.h>

#include "ItemManager.h"

namespace GOTOEngine
{
	enum E_Item_Enemy_Type
	{
		iceCrow,	// 얼음새
		bombCrow,	// 폭탄새
		goldCrow,	// 황금새
		item_type_count
	};

	class ItemEnemy : public BaseEnemyObject
	{
		E_Item_Enemy_Type m_itemEnemyType;
		ItemType m_itemType;

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
				ItemManager::instance->AddItem(m_layer, m_itemType);
			}
		}
		void Initialize(std::any param) override
		{
			if (param.type() == typeid(E_Item_Enemy_Type)) m_itemEnemyType = std::any_cast<E_Item_Enemy_Type>(param);
		}
		void Awake()
		{
			__super::Awake();

			m_enemyType = E_EnemyType::itemspawn;
			m_isMoveLoop = true;
			
			switch (m_itemEnemyType)
			{
			case iceCrow:
				m_moveFlag = 0b1000;
				m_disPoneTime = 10.0f;
				m_itemType = ItemType::Icebomb;
				GetGameObject()->name = L"얼음새";
				AddComponent<SpriteRenderer>();
				SetRandomYPosition(0.15f, 0.4f);
				break;
			case bombCrow:
				m_moveFlag = 0b1000;
				m_disPoneTime = 10.0f;
				m_itemType = ItemType::Bomb;
				GetGameObject()->name = L"폭탄새";
				AddComponent<SpriteRenderer>();
				SetRandomYPosition(0.15f, 0.4f);
				break;
			case goldCrow:
				m_moveFlag = 0b0010;
				m_disPoneTime = 10.0f;
				m_itemType = ItemType::Ticket;
				GetGameObject()->name = L"황금새";
				AddComponent<SpriteRenderer>();
				SetRandomYPosition(0.15f, 0.4f);
				break;
			}
			AddComponent<Animator>()->SetAnimatorController(EnemySpawner::instance->GetAnimation(GetGameObject()->name));
			GetComponent<SpriteRenderer>()->SetRenderLayer((1 << m_layer));
			GetTransform()->SetLossyScale({ 0.2f, 0.2f });

			auto spriteRect = EnemySpawner::instance->GetSprite(GetGameObject()->name)->GetRect();
			auto localScale = GetTransform()->GetLossyScale();
			auto collider = AddComponent<Collider2D>();

			collider->SetSize({ spriteRect.width * localScale.x , spriteRect.height * localScale.y });

			SetMovementComponents(0.15f, 0.4f);
		}

		int GetType() { return static_cast<int>(m_itemEnemyType); }

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