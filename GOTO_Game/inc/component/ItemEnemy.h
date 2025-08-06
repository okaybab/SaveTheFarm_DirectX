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
		goldMole,	// 황금 두더지
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

			if (m_itemEnemyType == E_Item_Enemy_Type::goldMole)
			{
				return;
			}

			if (m_isDeathByDispone)
			{
				if (m_layer & 1 << 1)
				{
					GameManager::instance->PointChange(1, -1);
				}
				else if (m_layer & 1 << 2)
				{
					GameManager::instance->PointChange(2, -1);
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
				GetTransform()->SetLossyScale({ 0.2f, 0.2f });
				break;
			case bombCrow:
				m_moveFlag = 0b1000;
				m_disPoneTime = 10.0f;
				m_itemType = ItemType::Bomb;
				GetGameObject()->name = L"폭탄새";
				SetRandomYPosition(0.15f, 0.4f);
				GetTransform()->SetLossyScale({ 0.2f, 0.2f });
				break;
			case goldCrow:
				m_moveFlag = 0b0010;
				m_disPoneTime = 10.0f;
				m_itemType = ItemType::Ticket;
				GetGameObject()->name = L"황금새";
				SetRandomYPosition(0.15f, 0.4f);
				GetTransform()->SetLossyScale({ 0.2f, 0.2f });
				break;
			case goldMole:
				m_moveFlag = 0b0001;
				m_disPoneTime = 10.0f;
				GetGameObject()->name = L"황금두더지";
				SetRandomYPosition(-0.4f, -0.1f);
				GetTransform()->SetLossyScale({ 0.12f, 0.12f });
				break;
			}
			SpriteRenderer* sprite = AddComponent<SpriteRenderer>();
			AddComponent<FadeComponent>();
			AddComponent<Animator>()->SetAnimatorController(EnemySpawner::instance->GetAnimation(GetGameObject()->name));
			sprite->SetRenderLayer(m_layer);

			auto spriteRect = EnemySpawner::instance->GetSprite(GetGameObject()->name)->GetRect();
			auto localScale = GetTransform()->GetLossyScale();
			auto collider = AddComponent<Collider2D>();

			collider->SetSize({ spriteRect.width * localScale.x , spriteRect.height * localScale.y });

			SetMovementComponents(0.15f, 0.4f);
		}

		int GetType() { return static_cast<int>(m_itemEnemyType); }

		void OnBulletDie(int attackerID) override
		{
			__super::OnBulletDie(attackerID);
			GetGameObject()->GetComponent<Animator>()->SetEnabled(false);
			GetGameObject()->GetComponent<SpriteRenderer>()->SetSprite(EnemySpawner::instance->GetSprite(GetGameObject()->name));

			auto fader = GetGameObject()->GetComponent<FadeComponent>();
			fader->Initialize();

			fader->FadeOut(0.5f, [this]() {
				EnemySpawner::instance->SetDeleteEnemy(m_layer, GetGameObject(), m_enemyType == E_EnemyType::move);
				Destroy(GetGameObject());
			});

			if (m_layer == 0b0110) // 황금두더지
			{
				// EnemySpawner::GenerateRandom(0, ItemType::Item_Count)
				ItemManager::instance->UseItem(attackerID + 1, static_cast<ItemType>(EnemySpawner::GenerateRandom(0, static_cast<int>(ItemType::Item_Count) - 1)));
			}
		}
	};
}