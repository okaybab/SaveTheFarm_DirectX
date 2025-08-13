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

			InitializeMovement();
		}

		void Awake()
		{
			__super::Awake();

			m_gameType = EnemySpawnManager::instance->GetGameType();

			m_isMoveLoop = false;
			m_disPoneTime = 30.0f;
		
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
		}

		void InitializeMovement()
		{
			m_movementComponents.clear();

			// 0b1001: 통통 튀는 포물선 + 대각선 이동
			if ((m_moveFlag & MOVE_PARABOLIC) && (m_moveFlag & MOVE_LEFT_RIGHT))
			{
				// MovementLinearPath 컴포넌트 초기화 및 델리게이트 바인딩
				auto linearPathComp = GetComponent<MovementLinearPath>();
				if (!linearPathComp)
				{
					linearPathComp = AddComponent<MovementLinearPath>();
					linearPathComp->OnEndPoint.Add<DefenseEnemy>(this, &DefenseEnemy::OnEndEvent);
				}
				// 컴포넌트의 Initialize는 한 번만 호출
				linearPathComp->Initialize(m_StartPos, m_EndPos, m_moveSpeed);
				m_movementComponents.push_back(linearPathComp);

				// MovementParabolic 컴포넌트 초기화
				auto parabolicComp = GetComponent<MovementParabolic>();
				if (!parabolicComp)
				{
					parabolicComp = AddComponent<MovementParabolic>();
				}
				// MOVE_UP_DOWN 플래그를 사용하여 OFFSET 모드로 초기화
				// 이 Initialize 함수에서 m_moveSpeed가 적절하게 설정되어야 함
				float bounceHeight = 100.0f;
				parabolicComp->testInitialize(bounceHeight);
				m_movementComponents.push_back(parabolicComp);
			}
			// 0b1000: 긴 포물선 (단일 컴포넌트로 처리)
			else if (m_moveFlag & MOVE_PARABOLIC)
			{
				if (auto comp = GetComponent<MovementParabolic>())
				{
					comp->Initialize(GetGameObject()->GetTransform()->GetPosition(), m_StartPos, m_EndPos, m_moveSpeed);
					m_movementComponents.push_back(comp);
				}
				else
				{
					comp = AddComponent<MovementParabolic>();
					comp->OnEndPoint.Add<DefenseEnemy>(this, &DefenseEnemy::OnEndEvent);
					comp->Initialize(GetGameObject()->GetTransform()->GetPosition(), m_StartPos, m_EndPos, m_moveSpeed);
					m_movementComponents.push_back(comp);
				}
			}
			// 0b0011: 지그재그 (MovementLeftRight + MovementUpDown)
			else if ((m_moveFlag & MOVE_LEFT_RIGHT) && (m_moveFlag & MOVE_UP_DOWN))
			{
				// 1. 중심축 이동을 위한 MovementLinearPath 컴포넌트 추가
				if (auto linearComp = GetComponent<MovementLinearPath>())
				{
					linearComp->Initialize(m_StartPos, m_EndPos, m_moveSpeed);
					// 이동이 완료되면 OnEndEvent를 호출하도록 델리게이트 바인딩
					// linearComp->OnEndPoint.Add<DefenseEnemy>(this, &DefenseEnemy::OnEndEvent);
					m_movementComponents.push_back(linearComp);
				}
				else
				{
					linearComp = AddComponent<MovementLinearPath>();
					linearComp->Initialize(m_StartPos, m_EndPos, m_moveSpeed);
					linearComp->OnEndPoint.Add<DefenseEnemy>(this, &DefenseEnemy::OnEndEvent);
					m_movementComponents.push_back(linearComp);
				}

				float wobbleWidth = 50.0f;     // 좌우 흔들림 폭
				float wobbleSpeed = 4.0f;      // 좌우 흔들림 속도
				float verticalBob = 15.0f;     // 상하 흔들림 폭
				float verticalBobSpeed = 3.0f; // 상하 흔들림 속도


				// 2. 흔들림(오프셋)을 위한 MovementLeftRight 컴포넌트 추가
				// MovementLeftRight::CalculateOffsetDirection(m_StartPos, m_EndPos)를 사용해 흔들림 방향을 설정
				if (auto lrComp = GetComponent<MovementLeftRight>())
				{
					lrComp->testInitialize(wobbleWidth, wobbleSpeed);
					lrComp->CalculateOffsetDirection(m_StartPos, m_EndPos);
					m_movementComponents.push_back(lrComp);
				}
				else
				{
					lrComp = AddComponent<MovementLeftRight>();
					lrComp->testInitialize(wobbleWidth, wobbleSpeed);
					lrComp->CalculateOffsetDirection(m_StartPos, m_EndPos);
					m_movementComponents.push_back(lrComp);
				}

				// MovementUpDown 컴포넌트도 필요하다면 동일하게 추가
				// 현재 코드에선 MovementUpDown이 상하 오프셋 역할
				//*//
				if (auto udComp = GetComponent<MovementUpDown>())
				{
					udComp->testInitialize(verticalBob, verticalBobSpeed);
					m_movementComponents.push_back(udComp);
				}
				else
				{
					udComp = AddComponent<MovementUpDown>();
					udComp->testInitialize(verticalBob, verticalBobSpeed);
					m_movementComponents.push_back(udComp);
				}
				//*/
			}
			else if (m_moveFlag & MOVE_LEFT_RIGHT) // 0b0001
			{
				auto linearPathComp = GetComponent<MovementLinearPath>();
				if (!linearPathComp)
				{
					linearPathComp = AddComponent<MovementLinearPath>();
					linearPathComp->OnEndPoint.Add<DefenseEnemy>(this, &DefenseEnemy::OnEndEvent);
				}
				// 컴포넌트의 Initialize는 한 번만 호출
				linearPathComp->Initialize(m_StartPos, m_EndPos, m_moveSpeed);
				m_movementComponents.push_back(linearPathComp);
			}
		}

		void OnEndEvent()
		{
			if (m_currentPoint >= m_points.size() - 1)
			{
				return;
			}

			m_currentPoint++;

			SetCurrentPoint();

			if (m_isCrop) m_moveFlag = 0b0000;
			else m_moveFlag = m_spawner->GetMoveFlag(GetGameObject()->name);

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