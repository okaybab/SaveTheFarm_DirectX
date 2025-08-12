#pragma once
#include "Resource.h"

#include <json.hpp>
#include <vector>
#include <unordered_map>
#include <cstdlib>
#include <any>

#include "EnemySpawnManager.h"
#include "BaseEnemyObject.h"

namespace GOTOEngine
{
	struct RangeInfo
	{
		float multiple = 0.0f;
		float offset = 0.0f;
	};

	using PointData = std::map<std::string, std::any>;

	class SpawnPoint : public Object
	{
	private:
		friend class BaseEnemyObject;

		E_Enemy_Anim_State m_state;
		float m_moveSpeed;
		int m_renderOrder;

		// 현재 위치를 저장할 변수
		Vector2 m_currentPosition;

		float m_fixedX;
		float m_fixedY;

		// 고정을 정하는 변수
		bool m_isCrop;
		bool m_isRandomX;
		bool m_isRandomY;

		RangeInfo m_randomRangeX_min;
		RangeInfo m_randomRangeX_max;
		RangeInfo m_randomRangeY_min;
		RangeInfo m_randomRangeY_max;

	public:
		void Initialize();
		void SetupFromJSON(const nlohmann::json& pointInfo);
		float CalculateCoordinate(const nlohmann::json& pointInfo, const std::string& axisName, float screenDimension, RangeInfo& minInfo, RangeInfo& maxInfo);
		Vector2 GetPosition() { return m_currentPosition; }
		PointData GetSpawnPointData() const
		{
			PointData params;
			params["animState"] = static_cast<E_Enemy_Anim_State>(m_state);
			params["moveSpeed"] = m_moveSpeed;
			params["position"] = m_currentPosition;
			params["renderOrder"] = m_renderOrder;
			params["crop"] = m_isCrop;
			return params;
		}
	};

	class EnemyMove : public Object
	{
	private:

		std::wstring enemyType;
		int moveFlag;

	public:
		EnemyMove() : moveFlag(0) {}
		void SetupFromJSON(const nlohmann::json& flagInfo)
		{ 
			moveFlag = flagInfo.value("value", 0);
			std::string enemyTypeStr = flagInfo.value("enemyType", "");
			size_t strLength = enemyTypeStr.length();
			if (strLength > 0)
			{
				std::vector<wchar_t> wstrBuffer(strLength + 1);
				size_t convertedChars = 0;
				mbstowcs_s(&convertedChars, &wstrBuffer[0], wstrBuffer.size(), enemyTypeStr.c_str(), _TRUNCATE);
				enemyType = &wstrBuffer[0];
			}
			else
			{
				enemyType = L"";
			}
		}
		int GetFlag() const { return moveFlag; }
		std::wstring GetEnemyType() const { return enemyType; }
	};

	class EnemySpawner : public Resource
	{
	private:
		friend class ResourceManager;
		friend class EnemySpawnManager;
		
		void LoadFromFilePath(const std::wstring& filePath) override;
		bool IsValidData() override { return true; }
		std::vector<SpawnPoint*> m_points;
		std::vector<EnemyMove*> m_moveFlag;
		std::wstring m_spawnName;
		bool m_isFixFlag;
		void Dispose() override;
	public:
		const std::vector<SpawnPoint*>& GetPoints() { return m_points; }
		const std::vector<EnemyMove*>& GetFlags() { return m_moveFlag; }
		int GetRandomMoveFlag() const;
		int GetFixFlagEnemyType(const std::wstring& enemyType) const;
		int GetMoveFlag(const std::wstring& enemyType) const;
		void Initialize() { for (auto move : m_points) move->Initialize(); }
	};

}//return m_moveFlag[EnemySpawnManager::instance->GenerateRandom(0, (int)m_moveFlag.size() - 1)]->GetFlag(); 
