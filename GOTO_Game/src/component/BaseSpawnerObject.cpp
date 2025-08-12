#include "BaseSpawnerObject.h"
#include "WStringHelper.h"
#include "Screen.h"

#include <filesystem>
#include <fstream>
#include <iostream>

using json = nlohmann::json;

void GOTOEngine::EnemySpawner::LoadFromFilePath(const std::wstring& filePath)
{
	std::ifstream inFile(filePath);
	if (inFile.is_open())
	{
		json jFromFile;
		inFile >> jFromFile;
		inFile.close();
		m_spawnName = STR_TO_WSTR(jFromFile["spawnName"]);

		m_isFixFlag = jFromFile.value("fixFlag", false);

		for (auto& pointInfo : jFromFile["points"])
		{
			SpawnPoint* spawnTemplate = new SpawnPoint();
			spawnTemplate->SetupFromJSON(pointInfo);
			m_points.emplace_back(spawnTemplate);
		}
		if (jFromFile.contains("moveFlags"))
		{
			for (auto& flagInfo : jFromFile["moveFlags"])
			{
				EnemyMove* moveTemplate = new EnemyMove();
				moveTemplate->SetupFromJSON(flagInfo);
				m_moveFlag.emplace_back(moveTemplate);
			}
		}
	}
}

void GOTOEngine::EnemySpawner::Dispose()
{
	for (auto point : m_points)
	{
		DestroyImmediate(point);
	}
	m_points.clear();
	for (auto flag : m_moveFlag)
	{
		DestroyImmediate(flag);
	}
	m_moveFlag.clear();
}

int GOTOEngine::EnemySpawner::GetRandomMoveFlag() const
{
	if (m_moveFlag.empty()) {
		return 0; // 컨테이너가 비어있으면 기본값 반환
	}
	int randomIndex = EnemySpawnManager::instance->GenerateRandom(0, (int)m_moveFlag.size() - 1);
	return m_moveFlag[randomIndex]->GetFlag();
}

int GOTOEngine::EnemySpawner::GetFixFlagEnemyType(const std::wstring& enemyType) const
{
	auto it = std::find_if(	m_moveFlag.begin(),
							m_moveFlag.end(),
							[enemyType](const EnemyMove* enemyMove) 
	{ return enemyMove->GetEnemyType() == enemyType; });

	if (it != m_moveFlag.end())
	{
		return (*it)->GetFlag();
	}
	else
	{
		return 0;
	}
}

int GOTOEngine::EnemySpawner::GetMoveFlag(const std::wstring& enemyType) const
{
	if (m_isFixFlag) {
		return GetFixFlagEnemyType(enemyType);
	}
	else {
		return GetRandomMoveFlag();
	}
}

void GOTOEngine::SpawnPoint::Initialize()
{
	if (m_isCrop)
	{
		const Vector2 fixedPositions[] = {
			{ Screen::GetWidth() * -0.3f, Screen::GetHeight() * -0.35f },
			{ Screen::GetWidth() * 0.3f, Screen::GetHeight() * -0.35f }
		};
		int randomIndex = EnemySpawnManager::instance->GenerateRandom(0, 1);
		m_currentPosition = fixedPositions[randomIndex];
		return;
	}

	float x = 0.0f;
	float y = 0.0f;

	if (m_isRandomX)
	{
		float minX = Screen::GetWidth() * m_randomRangeX_min.multiple + m_randomRangeX_min.offset;
		float maxX = Screen::GetWidth() * m_randomRangeX_max.multiple + m_randomRangeX_max.offset;
		x = EnemySpawnManager::instance->GenerateRandom(minX, maxX);
	}
	else
	{
		x = m_fixedX;
	}

	if (m_isRandomY)
	{
		float minY = Screen::GetHeight() * m_randomRangeY_min.multiple + m_randomRangeY_min.offset;
		float maxY = Screen::GetHeight() * m_randomRangeY_max.multiple + m_randomRangeY_max.offset;
		y = EnemySpawnManager::instance->GenerateRandom(minY, maxY);
	}
	else
	{
		y = m_fixedY;
	}

	m_currentPosition = Vector2(x, y);
}

void GOTOEngine::SpawnPoint::SetupFromJSON(const nlohmann::json& pointInfo)
{
	m_state = StringToState(pointInfo.value("state", ""));
	m_moveSpeed = pointInfo.value("speed", 1.0f);
	m_renderOrder = pointInfo.value("renderOrder", 1);

	// 위치 결정 규칙 파싱
	m_isCrop = pointInfo.value("crop", false);
	m_isRandomX = pointInfo.value("random_x", false);
	m_isRandomY = pointInfo.value("random_y", false);

	// 고정 좌표 값 파싱
	m_fixedX = Screen::GetWidth() * pointInfo.value("x", 0.0f);
	m_fixedY = Screen::GetHeight() * pointInfo.value("y", 0.0f);

	m_randomRangeX_min = { 0.0f, 0.0f };
	m_randomRangeX_max = { 0.0f, 0.0f };
	m_randomRangeY_min = { 0.0f, 0.0f };
	m_randomRangeY_max = { 0.0f, 0.0f };

	// 랜덤 범위 파싱
	CalculateCoordinate(pointInfo, "x", Screen::GetWidth(), m_randomRangeX_min, m_randomRangeX_max);
	CalculateCoordinate(pointInfo, "y", Screen::GetHeight(), m_randomRangeY_min, m_randomRangeY_max);
}

float GOTOEngine::SpawnPoint::CalculateCoordinate(const nlohmann::json& pointInfo, const std::string& axisName, float screenDimension, RangeInfo& minInfo, RangeInfo& maxInfo)
{
	std::string randomFlagKey = "random_" + axisName;
	std::string rangeKey = "random_" + axisName + "_range";

	if (pointInfo.value(randomFlagKey, false))
	{
		// 기본값 설정
		float minMultiplier = 0.0f, minOffset = 0.0f;
		float maxMultiplier = 1.0f, maxOffset = 0.0f;

		if (pointInfo.contains(rangeKey))
		{
			auto range = pointInfo[rangeKey];
			if (range.contains("min"))
			{
				minMultiplier = range["min"].value("multiple", 1.0f);
				minOffset = range["min"].value("offset", 0.0f);
			}
			if (range.contains("max"))
			{
				maxMultiplier = range["max"].value("multiple", 1.0f);
				maxOffset = range["max"].value("offset", 0.0f);
			}
		}

		float minCoord = screenDimension * minMultiplier + minOffset;
		float maxCoord = screenDimension * maxMultiplier + maxOffset;

		minInfo.multiple = minMultiplier;
		minInfo.offset = minOffset;

		maxInfo.multiple = maxMultiplier;
		maxInfo.offset = maxOffset;

		return EnemySpawnManager::instance->GenerateRandom(minCoord, maxCoord);
	}
	else
	{
		return pointInfo.value(axisName, 0.0f);
	}
}