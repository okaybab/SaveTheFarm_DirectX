#pragma once
#include "BaseEnemyObject.h"

#include <SpriteRenderer.h>
#include <Collider2D.h>
#include "Mathf.h"

#include "BaseSpawnerObject.h"

using ParameterMap = std::map<std::string, std::any>;

namespace GOTOEngine
{
	class DefenseEnemy : public BaseEnemyObject
	{
		E_Game_Type m_gameType;
		E_Defense_Fly_Type m_enemyType;
		E_Defense_Gimmick_Type m_gimmickType;
		E_Defense_Enemy_Type m_dEnemyType;
		bool m_isGimmick;
		bool m_isCrop;

		std::vector<SpawnPoint*> m_points;
		int m_currentPoint = 0;

		Vector2 m_StartPos;
		Vector2 m_EndPos;

		int m_renderOrder;
		float t = 0;
		float cropTime = 3.0f;

	public:
		void Initialize(std::any param) override
		{
			if (const auto pMap = std::any_cast<ParameterMap>(&param)) {
				const ParameterMap& params = *pMap;
				auto itEnemyType = params.find("EnemyType");
				if (itEnemyType != params.end()) {
					if (const auto pValue = std::any_cast<E_Defense_Fly_Type>(&itEnemyType->second)) { m_enemyType = *pValue; }
				}
				auto itGimmickType = params.find("GimmickType");
				if (itGimmickType != params.end()) {
					if (const auto pValue = std::any_cast<E_Defense_Gimmick_Type>(&itGimmickType->second)) { m_gimmickType = *pValue; }
				}
				// 타입에 맞는 이름 생성 params["EnemyName"] 가 없으면 랜덤으로 설정
				CreateEnemyWithRandomName(params, GetGameObject());
			}
			std::wstring type = CreateCombinedString(m_enemyType, m_gimmickType);
			m_spawner = EnemySpawnManager::instance->GetSpawner(type);
			if (m_spawner)
			{
				m_points = m_spawner->GetPoints();
				m_spawner->Initialize();
				SetCurrentPoint();
			}
		}

		void SetCurrentPoint()
		{
			std::any data = m_points[m_currentPoint]->GetSpawnPointData();

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
				auto itBool = params.find("crop");
				if (itBool != params.end()) {
					if (const auto pValue = std::any_cast<bool>(&itBool->second)) { m_isCrop = *pValue; }
				}
			}

			m_moveFlag = m_spawner->GetMoveFlag(GetGameObject()->name);

			if (m_currentPoint < m_points.size() - 1)
			{
				m_EndPos = m_points[m_currentPoint + 1]->GetPosition();
			}
			else
			{
				m_EndPos = m_points[m_points.size() - 1]->GetPosition();
			}
		}

		void Awake()
		{
			__super::Awake();

			m_gameType = EnemySpawnManager::instance->GetGameType();

			m_isMoveLoop = false;
			m_disPoneTime = 30.0f;
			
			GetTransform()->SetPosition(m_StartPos);
			m_currentPathPosition = m_StartPos;

			SetScaleByEnemyType(GetGameObject(), m_dEnemyType);

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

			if (m_currentPathPosition.x > m_EndPos.x) SetFlipXSprite();

			InitializeMovement();
			
		}

		void InitializeMovement()
		{
			m_movementComponents.clear();

			if (m_moveFlag & MOVE_PARABOLIC) // 0b1000 (곡선)
			{
				if (auto comp = GetComponent<MovementParabolic>())
				{
					comp->SetEnabled(m_moveFlag & MOVE_PARABOLIC);
					comp->Initialize(GetGameObject()->GetTransform()->GetPosition(), m_StartPos, m_EndPos, m_moveSpeed);
					m_movementComponents.push_back(comp);
				}
				else
				{
					comp = AddComponent<MovementParabolic>();
					comp->OnEndPoint.Add<DefenseEnemy>(this, &DefenseEnemy::OnEndEvent);
					comp->Initialize(Screen::GetWidth() * -0.25f - 420.0f, Screen::GetWidth() * 0.25f + 420.0f);
					comp->Initialize(GetGameObject()->GetTransform()->GetPosition(), m_StartPos, m_EndPos, m_moveSpeed);
					m_movementComponents.push_back(comp);
				}
			}

			if (m_moveFlag & MOVE_LEFT_RIGHT) // 물결
			{
				if (auto comp = GetComponent<MovementLeftRight>())
				{
					comp->testInitialize(m_moveSpeed);
					comp->CalculateOffsetDirection(m_StartPos, m_EndPos);
					m_movementComponents.push_back(comp);
				}
				else
				{
					comp = AddComponent<MovementLeftRight>();
					comp->OnEndPoint.Add<DefenseEnemy>(this, &DefenseEnemy::OnEndEvent);
					comp->Initialize(Screen::GetWidth() * -0.25f - 420.0f, Screen::GetWidth() * 0.25f + 420.0f);
					comp->testInitialize(m_moveSpeed);
					comp->CalculateOffsetDirection(m_StartPos, m_EndPos);
					m_movementComponents.push_back(comp);
				}
			}
		}

		void OnEndEvent()
		{
			if (m_currentPoint < m_points.size() - 1)
			{
				m_currentPoint++;
			}
			else
			{
				return;
			}
			
			t = 0.0f;

			SetCurrentPoint();

			if (m_isCrop) m_moveFlag = 0b0000;
			else m_moveFlag = m_spawner->GetMoveFlag(GetGameObject()->name);

			m_StartPos = GetGameObject()->GetTransform()->GetPosition();
			m_currentPathPosition = m_StartPos;

			SetFlipXSprite(m_currentPathPosition.x > m_EndPos.x);

			InitializeMovement();
			SetState(m_animState);
		}

		void Update() override
		{
			__super::Update();

			if (m_isCrop)
			{
				cropTime -= TIME_GET_DELTATIME();

				if (cropTime <= 0.0f)
				{
					m_isCrop = false;
					m_moveFlag = m_spawner->GetMoveFlag(GetGameObject()->name);
					InitializeMovement();
				}
			}
			else
			{
				if (m_moveFlag & MOVE_LEFT_RIGHT && m_moveFlag & MOVE_UP_DOWN)
				{
					t += TIME_GET_DELTATIME() * m_moveSpeed;
					m_currentPathPosition.x = Mathf::Lerp(m_StartPos.x, m_EndPos.x, t);
					m_currentPathPosition.y = Mathf::Lerp(m_StartPos.y, m_EndPos.y, t);

					if ((m_currentPathPosition - m_EndPos).Magnitude() < 1.0f)
					{
						OnEndEvent();
					}
				}
			}
			
		}

		int GetType() { return static_cast<int>(m_enemyType); }
		void SetDefenseEnemyType(E_Defense_Enemy_Type type) { m_dEnemyType = type; }
		void OnDie(int attackerID, bool isGimmick = true) override
		{
			__super::OnDie(attackerID);
			EnemySpawnManager::instance->SetDeleteEnemy(m_layer, GetGameObject(), isGimmick);
		}
		void OnDispone() override
		{
			__super::OnDispone();
			SetState(E_Enemy_Anim_State::DISPONE);
			EnemySpawnManager::instance->SetDeleteEnemy(m_layer, GetGameObject());
		}
		void OnGimmick()
		{

		}
	};
}