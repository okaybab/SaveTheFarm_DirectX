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

		std::vector<SpawnPoint*> m_points;
		int currentPoint = 0;

		Vector2 m_StartPos;
		Vector2 m_EndPos;

		int m_renderOrder;
		float t = 0;

		E_Game_Type m_gameType;

	public:
		void Initialize(std::any param) override
		{
			if(EnemySpawnManager::instance->GetGameType() == E_Game_Type::GAME1)
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
			else if(EnemySpawnManager::instance->GetGameType() == E_Game_Type::GAME2)
			{
				if (const auto pMap = std::any_cast<ParameterMap>(&param)) {
					const ParameterMap& params = *pMap;
					auto itEnemyType = params.find("EnemyType");
					if (itEnemyType != params.end()) {
						if (const auto pValue = std::any_cast<E_Move_Enemy_Type>(&itEnemyType->second)) { m_moveEnemyType = *pValue; }
					}
				}
			}
		}

		void SetupSpawner(EnemySpawner* spawner, std::any param) override
		{
			if (EnemySpawnManager::instance->GetGameType() != E_Game_Type::GAME2)  return;

			__super::SetupSpawner(spawner, param);

			if (const auto pMap = std::any_cast<ParameterMap>(&param)) {
				const ParameterMap& params = *pMap;
				auto itEnemyType = params.find("EnemyType");
				if (itEnemyType != params.end()) {
					if (const auto pValue = std::any_cast<E_Move_Enemy_Type>(&itEnemyType->second)) { m_moveEnemyType = *pValue; }
				}
			}
			if (spawner)
			{
				m_points = spawner->GetPoints();

				
				SetCurrentPoint();

			}

		}

		void SetCurrentPoint()
		{
			std::any data = m_points[currentPoint]->GetSpawnPointData();
		
			if (const auto pMap = std::any_cast<PointData>(&data)) {
				const ParameterMap& params = *pMap;
				auto itAnimState = params.find("animState");
				if (itAnimState != params.end()) {
					if (const auto pValue = std::any_cast<E_Enemy_Anim_State>(&itAnimState->second)) { m_animState = *pValue; }
				}
				auto itFloat = params.find("moveSpeed");
				if (itFloat != params.end()) {
					if (const auto pValue = std::any_cast<float>(&itFloat->second)) { m_moveSpeed = *pValue; }
				}
				auto itVector2 = params.find("position");
				if (itVector2 != params.end()) {
					if (const auto pValue = std::any_cast<Vector2>(&itVector2->second)) { m_StartPos = *pValue; }
				}
				auto itInt = params.find("renderOrder");
				if (itInt != params.end()) {
					if (const auto pValue = std::any_cast<int>(&itInt->second)) { m_renderOrder = *pValue; }
				}
			}

			if (currentPoint + 1 != m_points.size())
			{
				m_EndPos = m_points[currentPoint + 1]->GetPosition();
			}

		}

		void Awake()
		{
			__super::Awake();

			m_gameType = EnemySpawnManager::instance->GetGameType();

			if (EnemySpawnManager::instance->GetGameType() == E_Game_Type::GAME1)
			{
				m_enemyType = E_EnemyType::move;
				m_isMoveLoop = true;

				switch (m_moveEnemyType)
				{
				case mole:
					m_moveFlag = 0b0000;
					m_disPoneTime = 8.0f;
					GetGameObject()->name = L"두더지";
					SetRandomYPosition(-0.3f, -0.1f);
					GetTransform()->SetLossyScale({ 0.12f, 0.12f });
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
					if (m_animState == DIE || m_animState == ESCAPE)
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
			else // E_Game_Type::GAME2
			{
				//std::cout << "Move Awake" << std::endl;
				
				/*/
				m_enemyType = E_EnemyType::move;
				m_isMoveLoop = false;
				m_moveFlag = m_spawner->GetRandomMoveFlag();
				m_moveSpeed *= 1.3f;
				GetGameObject()->name = L"까마귀";

				GetTransform()->SetPosition(m_StartPos);
				m_currentPathPosition = m_StartPos;
				GetTransform()->SetLossyScale({ 0.3f, 0.3f });

				AddComponent<SpriteRenderer>()->SetRenderLayer(1);
				AddComponent<Animator>()->SetAnimatorController(EnemySpawnManager::instance->GetAnimation(GetGameObject()->name));
				auto controller = GetComponent<Animator>()->GetRuntimeAnimatorController();
				controller->SetOnAnimationEnd([this, controller]() {
					if (m_animState == DIE || m_animState == ESCAPE)
					{
						controller->SetOnAnimationEnd(nullptr);
						GameObject::Destroy(GetGameObject());
					}
				});
				auto spriteRect = EnemySpawnManager::instance->GetSprite(GetGameObject()->name)->GetRect();
				auto localScale = GetTransform()->GetLossyScale();
				auto collider = AddComponent<Collider2D>();

				collider->SetSize({ spriteRect.width * localScale.x , spriteRect.height * localScale.y });

				SetUpMovementComponents();
				//*/


			}
		}

		void SetUpMovementComponents()
		{
			auto comp = AddComponent<MovementParabolic>();
			//comp->OnFlipDirection.Add(this, &BaseEnemyObject::SetFlipXSprite);
			comp->Initialize(Screen::GetWidth() * -0.25f - 420.0f, Screen::GetWidth() * 0.25f + 420.0f);

			m_movementComponents = GetGameObject()->GetComponents<BaseMovement>();

			for (auto comp : m_movementComponents)
			{
				comp->Initialize(m_moveFlag, GetGameObject()->GetTransform()->GetPosition(), m_moveSpeed);
			}
		}

		/*/
		void Update() override
		{
			__super::Update();
			
			t += TIME_GET_DELTATIME() * m_moveSpeed;

			// m_StartPos와 m_EndPos 사이의 t 비율만큼의 위치를 계산합니다.
			m_currentPathPosition.x = Mathf::Lerp(m_StartPos.x, m_EndPos.x, t);
			m_currentPathPosition.y = Mathf::Lerp(m_StartPos.y, m_EndPos.y, t);
				
			__super::Update();
			
			
		}
		//*/

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