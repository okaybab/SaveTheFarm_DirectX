#pragma once
#include "BaseEnemyObject.h"

#include <SpriteRenderer.h>
#include <Collider2D.h>

#include "ItemManager.h"
#include "FadeComponent.h"

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
				SetRandomYPosition(0.15f, 0.4f);
				GetTransform()->SetLossyScale({ 0.3f, 0.3f });
				break;
			case bombCrow:
				m_moveFlag = 0b1000;
				m_disPoneTime = 10.0f;
				m_itemType = ItemType::Bomb;
				GetGameObject()->name = L"폭탄새";
				SetRandomYPosition(0.15f, 0.4f);
				GetTransform()->SetLossyScale({ 0.3f, 0.3f });
				break;
			case goldCrow:
				m_moveFlag = 0b0010;
				m_disPoneTime = 10.0f;
				m_itemType = ItemType::Ticket;
				GetGameObject()->name = L"황금새";
				SetRandomYPosition(0.15f, 0.4f);
				GetTransform()->SetLossyScale({ 0.3f, 0.3f });
				break;
			}
			AddComponent<SpriteRenderer>()->SetRenderLayer(m_layer);
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

			SetMovementComponents(0.15f, 0.4f);
		}

		int GetType() { return static_cast<int>(m_itemEnemyType); }

		void OnDie(int attackerID) override
		{
			__super::OnDie(attackerID);
			EnemySpawner::instance->SetDeleteEnemy(m_layer, GetGameObject());
		}
		void OnDispone() override
		{
			__super::OnDispone();
			EnemySpawner::instance->SetDeleteEnemy(m_layer, GetGameObject());
		}
	};
}