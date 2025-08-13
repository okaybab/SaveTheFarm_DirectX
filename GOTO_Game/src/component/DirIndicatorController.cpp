#include "DirIndicatorController.h"
#include "DirIndicatorPrefab.h"
#include "EnemySpawnManager.h"
#include "DirIndicator.h"
#include <Camera.h>
#include <Transform.h>

void GOTOEngine::DirIndicatorController::LoadIndicator()
{
	auto indicatorGO = DirIndicatorPrefab::CreateIndicator(id);
	indicatorGO->GetTransform()->SetParent(GetTransform());

	auto indicator = indicatorGO->GetComponent<DirIndicator>();
	if (IsValidObject(indicator))
		m_indicators.push_back(indicator);
	else
		DestroyImmediate(indicatorGO);
}

void GOTOEngine::DirIndicatorController::Awake()
{
	for (size_t i = 0; i < capacity; i++)
	{
		LoadIndicator();
	}
}

void GOTOEngine::DirIndicatorController::LateUpdate()
{
	if (!IsValidObject(cam) 
		|| cam->IsDestroyed())
		return;

	if (IsDestroyed())
		return;

	for (auto& indicator : m_indicators)
	{
		indicator->GetGameObject()->SetActive(false);
	}

	std::vector<GameObject*>* enemys;
	if (id == 0)
	{
		enemys = EnemySpawnManager::instance->Getp1Enemy();
	}
	else
	{
		enemys = EnemySpawnManager::instance->Getp2Enemy();
	}

	auto camPosMin = cam->ViewportToWorldPoint({ 0.0f,0.0f });
	auto camPosMax = cam->ViewportToWorldPoint({ 1.0f,1.0f });

	int currentCount = 0;
	for (auto& enemy : *enemys)
	{
		if (!IsValidObject(enemy) || enemy->IsDestroyed()) continue;

		auto enemyTransform = enemy->GetTransform();

		auto enemyPos = enemyTransform->GetPosition();

		if (enemyPos.x < camPosMin.x || enemyPos.x > camPosMax.x ||
			enemyPos.y < camPosMin.y || enemyPos.y > camPosMax.y)
		{
			if (currentCount >= m_indicators.size())
			{
				LoadIndicator();
			}
			auto& currentIndicator = m_indicators[currentCount++];
			currentIndicator->GetGameObject()->SetActive(true);
			currentIndicator->target = enemyTransform;
			currentIndicator->currentCam = cam;

			//강제 실행 - 아님 렌더러가 그냥 그려버림
			currentIndicator->LateUpdate();
		}
	}
}

void GOTOEngine::DirIndicatorController::OnDestroy()
{
	if (m_indicators.size() > 0)
	{
		for (auto& indicator : m_indicators)
		{
			if(IsValidObject(indicator)
				&& !indicator->IsDestroyed())
				DestroyImmediate(indicator->GetGameObject());
		}
	}
}
