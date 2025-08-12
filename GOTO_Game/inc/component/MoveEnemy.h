#pragma once
#include "BaseEnemyObject.h"

#include <SpriteRenderer.h>
#include <Collider2D.h>
#include "Mathf.h"

#include "BaseSpawnerObject.h"

using ParameterMap = std::map<std::string, std::any>;

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
		bool m_isGimmick;

	public:
		void Initialize(std::any param) override
		{
			if (const auto pMap = std::any_cast<ParameterMap>(&param)) {
				const ParameterMap& params = *pMap;
				auto itEnemyType = params.find("EnemyType");
				if (itEnemyType != params.end()) {
					if (const auto pValue = std::any_cast<E_Move_Enemy_Type>(&itEnemyType->second)) { m_moveEnemyType = *pValue; }
				}
				auto itBool = params.find("isGimmick");
				if (itBool != params.end()) {
					if (const auto pValue = std::any_cast<bool>(&itBool->second)) { m_isGimmick = *pValue; }
				}
			}
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
				m_disPoneTime = 8.0f;
				GetGameObject()->name = L"두더지";
				SetRandomYPosition(-0.3f, -0.1f);
				GetTransform()->SetLossyScale({ 0.15f, 0.15f });
				break;
			case crow_1:
				m_moveFlag = 0b0001;
				m_disPoneTime = 10.0f;
				m_moveSpeed = 1.3f;
				GetGameObject()->name = L"까마귀";
				SetRandomYPosition(0.15f, 0.4f);
				GetTransform()->SetLossyScale({ 0.6f, 0.6f });
				break;
			case crow_2:
				m_moveFlag = 0b0010;
				m_disPoneTime = 10.0f;
				m_moveSpeed = 1.3f;
				GetGameObject()->name = L"까마귀";
				SetRandomYPosition(0.15f, 0.4f);
				GetTransform()->SetLossyScale({ 0.6f, 0.6f });
				break;
			}
			AddComponent<SpriteRenderer>()->SetRenderLayer(m_layer);
			AddComponent<Animator>()->SetAnimatorController(EnemySpawnManager::instance->GetAnimation(GetGameObject()->name));

			auto controller = GetComponent<Animator>()->GetRuntimeAnimatorController();
			controller->SetOnAnimationEnd([this, controller]() {
				if (m_animState == DIE || m_animState == DISPONE)
				{
					controller->SetOnAnimationEnd(nullptr);
					GameObject::Destroy(GetGameObject());
				}
			});

			auto spriteRect = EnemySpawnManager::instance->GetSprite(GetGameObject()->name)->GetRect();
			auto localScale = GetTransform()->GetLossyScale();
			auto collider = AddComponent<Collider2D>();

			collider->SetSize({ spriteRect.width * localScale.x , spriteRect.height * localScale.y });

			SetMovementComponents(0.15f, 0.4f);

			if (m_isGimmick) OnGimmick();
		}

		int GetType() { return static_cast<int>(m_moveEnemyType); }
		void OnDie(int attackerID, bool isGimmick = true) override
		{
			__super::OnDie(attackerID);
			EnemySpawnManager::instance->SetDeleteEnemy(m_layer, GetGameObject(), isGimmick);
		}
		void OnDispone() override
		{
			__super::OnDispone();
			EnemySpawnManager::instance->SetDeleteEnemy(m_layer, GetGameObject());
			if(EnemySpawnManager::instance->GetGameType() == E_Game_Type::GAME1)
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
		}
		void OnGimmick()
		{
			auto gimmickEffect = new GameObject;
			gimmickEffect->layer = m_layer;
			gimmickEffect->AddComponent<SpriteRenderer>()->SetRenderLayer(m_layer);
			gimmickEffect->GetComponent<SpriteRenderer>()->SetRenderOrder(GetComponent<SpriteRenderer>()->GetRenderOrder() - 1);
			gimmickEffect->AddComponent<Animator>()->SetAnimatorController(EnemySpawnManager::instance->GetAnimation(L"Gimmick3"));
			gimmickEffect->GetTransform()->SetParent(this->GetTransform(), false);
			if (m_moveEnemyType == mole) gimmickEffect->GetTransform()->SetLossyScale(GetTransform()->GetLocalScale() * 2.3f);
			else gimmickEffect->GetTransform()->SetLossyScale(GetTransform()->GetLocalScale() * 1.5f);

			auto controller = gimmickEffect->GetComponent<Animator>()->GetRuntimeAnimatorController();
			controller->SetOnAnimationEnd([this, gimmickEffect]() {
				GameObject::Destroy(gimmickEffect);
			});
		}
	};
}