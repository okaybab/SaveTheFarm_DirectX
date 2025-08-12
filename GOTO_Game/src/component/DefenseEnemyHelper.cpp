#include "BaseSpawnerObject.h"

#include "DefenseEnemy.h"

namespace GOTOEngine
{
	void SetScaleByEnemyType(GameObject* enemyObject, E_Defense_Enemy_Type type)
	{
		Vector2 scale = { 0.6f, 0.6f };

		switch (type)
		{
		case E_Defense_Enemy_Type::d_mole: // 두더지
			scale = { 0.15f, 0.15f };
			break;
		case E_Defense_Enemy_Type::d_crow: // 까마귀
			scale = { 0.6f, 0.6f };
			break;
		case E_Defense_Enemy_Type::d_rabbit: // 토끼
			scale = { 0.24f, 0.24f };
			break;
		case E_Defense_Enemy_Type::d_squirrel: // 다람쥐
			scale = { 0.3f, 0.3f };
			break;
		case E_Defense_Enemy_Type::d_thiefMole: // 도둑두더지
			scale = { 0.15f, 0.15f };
			break;
		case E_Defense_Enemy_Type::d_iceCrow: // 얼음새
			scale = { 0.3f, 0.3f };
			break;
		case E_Defense_Enemy_Type::d_bombCrow: // 폭탄새
			scale = { 0.3f, 0.3f };
			break;
		case E_Defense_Enemy_Type::d_mushCrow: // 버섯새
			scale = { 0.3f, 0.3f };
			break;
		default:
			break;
		}

		if (enemyObject && enemyObject->GetTransform())
		{
			enemyObject->GetTransform()->SetLossyScale(scale);
		}
	}


	std::wstring TypetoString(E_Defense_Enemy_Type type)
	{
		switch (type)
		{
		case E_Defense_Enemy_Type::d_mole:      return L"두더지";
		case E_Defense_Enemy_Type::d_crow:      return L"까마귀";
		case E_Defense_Enemy_Type::d_rabbit:    return L"토끼";
		case E_Defense_Enemy_Type::d_squirrel:  return L"다람쥐";
		case E_Defense_Enemy_Type::d_thiefMole: return L"도둑두더지";
		case E_Defense_Enemy_Type::d_iceCrow:   return L"얼음새";
		case E_Defense_Enemy_Type::d_bombCrow:  return L"폭탄새";
		case E_Defense_Enemy_Type::d_mushCrow:  return L"버섯새";
		default:                                return L"";
		}
	}

	E_Defense_Enemy_Type GetDefenseEnemyType(const std::wstring& enemyName)
	{
		if (enemyName == L"두더지") return E_Defense_Enemy_Type::d_mole;
		if (enemyName == L"까마귀") return E_Defense_Enemy_Type::d_crow;
		if (enemyName == L"토끼") return E_Defense_Enemy_Type::d_rabbit;
		if (enemyName == L"다람쥐") return E_Defense_Enemy_Type::d_squirrel;
		if (enemyName == L"도둑두더지") return E_Defense_Enemy_Type::d_thiefMole;
		if (enemyName == L"얼음새") return E_Defense_Enemy_Type::d_iceCrow;
		if (enemyName == L"폭탄새") return E_Defense_Enemy_Type::d_bombCrow;
		if (enemyName == L"버섯새") return E_Defense_Enemy_Type::d_mushCrow;

		return defense_type_count;
	}

	std::wstring GetDefenseEnemyTypeString(E_Defense_Fly_Type type)
	{
		static const std::map<E_Defense_Fly_Type, std::wstring> typeMap = {
			{E_Defense_Fly_Type::fly, L"공중"},
			{E_Defense_Fly_Type::ground1, L"지상1"},
			{E_Defense_Fly_Type::ground2, L"지상2"}
		};

		auto it = typeMap.find(type);
		if (it != typeMap.end())
		{
			return it->second;
		}
		return L"";
	}

	std::wstring GetDefenseGimmickTypeString(E_Defense_Gimmick_Type type)
	{
		static const std::map<E_Defense_Gimmick_Type, std::wstring> typeMap = {
			{E_Defense_Gimmick_Type::defense_nomal, L""},
			{E_Defense_Gimmick_Type::defense_gimmick, L"기믹"}
		};

		auto it = typeMap.find(type);
		if (it != typeMap.end())
		{
			return it->second;
		}
		return L"";
	}
	std::wstring CreateCombinedString(E_Defense_Fly_Type enemyType, E_Defense_Gimmick_Type gimmickType)
	{
		return (GetDefenseEnemyTypeString(enemyType) + GetDefenseGimmickTypeString(gimmickType));
	}

	std::wstring GetRandomNameFromVector(const std::vector<std::wstring>& names)
	{
		if (names.empty())
		{
			return L"";
		}

		// EnemySpawnManager의 난수 생성기(m_gen)와 뮤텍스를 활용
		// T = int, min = 0, max = names.size() - 1
		int randomIndex = static_cast<int>(EnemySpawnManager::instance->GenerateRandom(0, (int)names.size() - 1));

		return names[randomIndex];
	}

	void CreateEnemyWithRandomName(const ParameterMap& params, GameObject* newEnemyObject)
	{
		// E_Defense_Fly_Type과 E_Defense_Gimmick_Type 값 가져오기
		E_Defense_Fly_Type enemyType = static_cast<E_Defense_Fly_Type>(std::any_cast<E_Defense_Fly_Type>(params.at("EnemyType")));
		E_Defense_Gimmick_Type gimmickType = static_cast<E_Defense_Gimmick_Type>(std::any_cast<E_Defense_Gimmick_Type>(params.at("GimmickType")));

		std::wstring enemyName = L"";

		// EnemyName 키가 있는지 확인하고, 있으면 값을 가져옵니다.
		auto itEnemyName = params.find("EnemyName");
		if (itEnemyName != params.end())
		{
			if (const auto pValue = std::any_cast<std::wstring>(&itEnemyName->second))
			{
				enemyName = *pValue;
			}
		}

		// EnemyName이 없거나 비어 있으면 타입에 따라 랜덤 이름을 할당합니다.
		if (enemyName.empty())
		{
			// 타입에 따라 이름 목록을 설정
			std::vector<std::wstring> possibleNames;

			if (enemyType == E_Defense_Fly_Type::fly)
			{
				if (gimmickType == E_Defense_Gimmick_Type::defense_nomal)
				{
					// 공중 (까마귀)
					possibleNames.push_back(L"까마귀");
				}
				else // defense_gimmick
				{
					// 공중기믹 (얼음새, 폭탄새, 버섯새)
					possibleNames.push_back(L"얼음새");
					possibleNames.push_back(L"폭탄새");
					possibleNames.push_back(L"버섯새");
				}
			}
			else // 지상 (ground1, ground2)
			{
				if (gimmickType == E_Defense_Gimmick_Type::defense_nomal)
				{
					// 지상 (두더지)
					possibleNames.push_back(L"두더지");
				}
				else // defense_gimmick
				{
					// 지상기믹 (토끼, 다람쥐, 도둑두더지)
					possibleNames.push_back(L"토끼");
					possibleNames.push_back(L"다람쥐");
					possibleNames.push_back(L"도둑두더지");
				}
			}

			// 가능한 이름 목록에서 랜덤으로 하나 선택
			enemyName = GetRandomNameFromVector(possibleNames);
		}

		// 최종적으로 결정된 이름으로 GameObject의 이름 설정
		if (!enemyName.empty())
		{
			newEnemyObject->name = enemyName;

			E_Defense_Enemy_Type dEnemyType = GetDefenseEnemyType(enemyName);
			if (auto enemyComp = newEnemyObject->GetComponent<DefenseEnemy>())
			{
				enemyComp->SetDefenseEnemyType(dEnemyType);
			}
		}
	}
}