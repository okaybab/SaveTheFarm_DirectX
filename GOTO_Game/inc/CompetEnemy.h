#pragma once
#include "BaseEnemyObject.h"

#include <SpriteRenderer.h>
#include <Collider2D.h>

#include "ItemManager.h"
#include "FadeComponent.h"

namespace GOTOEngine
{
	enum E_Compet_Enemy_Type
	{
		goldMole,	// 황금 두더지
		compet_type_count
	};

	class CompetEnemy : public BaseEnemyObject
	{
		E_Compet_Enemy_Type m_competEnemyType;

	public:
		void Dispose()
		{
			if (!GameManager::instance->setactive) return;
		}
		void Initialize(std::any param) override
		{
			if (param.type() == typeid(E_Compet_Enemy_Type)) m_competEnemyType = std::any_cast<E_Compet_Enemy_Type >(param);
		}
		void Awake()
		{
			__super::Awake();

			m_enemyType = E_EnemyType::itemspawn;
			m_isMoveLoop = true;

			switch (m_competEnemyType)
			{
			case goldMole:
				m_moveFlag = 0b0001;
				m_disPoneTime = 10.0f;
				GetGameObject()->name = L"황금두더지";
				SetRandomYPosition(-0.3f, -0.1f);
				GetTransform()->SetLossyScale({ 0.2f, 0.2f });
				break;
			}
			AddComponent<SpriteRenderer>()->SetRenderLayer(m_layer);
			AddComponent<FadeComponent>();
			AddComponent<Animator>()->SetAnimatorController(EnemySpawner::instance->GetAnimation(GetGameObject()->name));

			auto spriteRect = EnemySpawner::instance->GetSprite(GetGameObject()->name)->GetRect();
			auto localScale = GetTransform()->GetLossyScale();
			auto collider = AddComponent<Collider2D>();

			collider->SetSize({ spriteRect.width * localScale.x , spriteRect.height * localScale.y });

			SetMovementComponents(0.15f, 0.4f);
		}

		int GetType() { return static_cast<int>(m_competEnemyType); }

		void OnDie(int attackerID) override
		{
			GetGameObject()->GetComponent<Animator>()->SetEnabled(false);
			GetGameObject()->GetComponent<SpriteRenderer>()->SetSprite(EnemySpawner::instance->GetSprite(GetGameObject()->name));

			auto fader = GetGameObject()->GetComponent<FadeComponent>();
			fader->Initialize();
			EnemySpawner::instance->SetDeleteGoldMole();

			if(attackerID != -1) ItemManager::instance->UseItem(attackerID, static_cast<ItemType>(EnemySpawner::GenerateRandom(0, static_cast<int>(ItemType::Item_Count) - 1)));
			fader->FadeOut(0.5f, [this]() {
				Destroy(GetGameObject());
			});
		}
	};
}