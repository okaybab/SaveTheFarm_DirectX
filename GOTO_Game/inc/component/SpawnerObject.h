#pragma once
#include "Resource.h"

#include "EnemySpawnManager.h"
#include "BaseEnemyObject.h"
#include "BaseMovement.h"

#include <json.hpp>
#include <vector>
#include <unordered_map>

namespace GOTOEngine
{
	struct RangeInfo
	{
		float multiple = 0.0f;
		float offset = 0.0f;
	};

	class SpawnPoint : public Object
	{
	private:
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
		SpawnPoint() = default;
		void Initialize();
		Vector2 GetPosition() const { return m_currentPosition; }
		void SetupFromJSON(const nlohmann::json& pointInfo);
		float CalculateCoordinate(const nlohmann::json& pointInfo, const std::string& axisName, float screenDimension);
	};

	class EnemyMove : public Object
	{
	private:
		friend class BaseMovement;
		
		int moveFlag;
	public:
		EnemyMove() : moveFlag(0) {}
		void SetupFromJSON(const nlohmann::json& flagInfo);
		int GetFlag() const { return moveFlag; }
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
		
	public:
		void Dispose() override;
		EnemySpawner() = default;
	};

}



