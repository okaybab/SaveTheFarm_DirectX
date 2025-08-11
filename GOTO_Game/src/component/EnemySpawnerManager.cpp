#include "EnemySpawnManager.h"
#include "Screen.h"

// manager
#include "GameManager.h"

// enemy
#include "BaseEnemyObject.h"
#include "MoveEnemy.h"
#include "GimmickEnemy.h"
#include "ItemEnemy.h"
#include "CompetEnemy.h"

// move
#include "MovementLeftRight.h"
#include "MovementUpDown.h"
#include "MovementCircle.h"
#include "MovementParabolic.h"

// spawner
#include "BaseSpawnerObject.h"

using namespace GOTOEngine;

EnemySpawnManager* EnemySpawnManager::instance = nullptr;
std::mt19937 EnemySpawnManager::m_gen(std::random_device{}());
std::mutex EnemySpawnManager::m_genMutex;

void GOTOEngine::EnemySpawnManager::Awake()
{
	if (!instance)
	{
		instance = this;
		//경로 지정 및 리스트에 추가
		std::vector<std::pair<std::wstring, std::wstring>> animList = {
			{L"두더지", L"../Resources/Animation/controller/MoleAnimator_AnimController.json"},
			{L"까마귀", L"../Resources/Animation/controller/CrowAnimator_AnimController.json"},

			{L"토끼", L"../Resources/Animation/controller/RabbitAnimator_AnimController.json"},
			{L"다람쥐", L"../Resources/Animation/controller/SquirrelAnimator_AnimController.json"},
			{L"도둑두더지", L"../Resources/Animation/controller/ThiefMoleAnimator_AnimController.json"},

			{L"얼음새", L"../Resources/Animation/controller/IceCrowAnimator_AnimController.json"},
			{L"폭탄새", L"../Resources/Animation/controller/BombCrowAnimator_AnimController.json"},
			{L"황금새", L"../Resources/Animation/controller/GoldCrowAnimator_AnimController.json"},
			{L"황금두더지", L"../Resources/Animation/controller/GoldMoleAnimator_AnimController.json"},

			{L"Gimmick3", L"../Resources/Animation/controller/Gimmick3_Animator_AnimController.json"}
		};
		for (const auto& [key, path] : animList)
		{
			auto animControl = Resource::Load<AnimatorController>(path.c_str());
			animControl->IncreaseRefCount();
			m_animControllers[key] = animControl;
		}

		std::vector<std::pair<std::wstring, std::wstring>> spriteList = {
			{L"두더지", L"../Resources/artResource/Sprint/Mole_die.png"},
			{L"까마귀", L"../Resources/artResource/Sprint/Crow_die.png"},

			{L"토끼", L"../Resources/artResource/Sprint/Rabbit_die.png"},
			{L"다람쥐", L"../Resources/artResource/Sprint/Squirrel_die.png"},
			{L"도둑두더지", L"../Resources/artResource/Sprint/ThiefMole_die.png"},
			
			{L"얼음새", L"../Resources/artResource/Sprint/IceCrow_die.png"},
			{L"폭탄새", L"../Resources/artResource/Sprint/BomCrow_die.png"},
			{L"황금새", L"../Resources/artResource/Sprint/GoldCrow_die.png"},
			{L"황금두더지", L"../Resources/artResource/Sprint/GoldMole_die.png"},
		};
		for (const auto& [key, path] : spriteList)
		{
			auto sprite = Resource::Load<Sprite>(path.c_str());
			sprite->IncreaseRefCount();
			m_sprites[key] = sprite;
		}

		std::vector<std::pair<std::wstring, std::wstring>> spawnList = {
			{L"지상", L"../Resources/EnemySpawner/SpawnPoint_Fly.json"}
		};
		for (const auto& [key, path] : spawnList)
		{
			auto spawner = Resource::Load<EnemySpawner>(path.c_str());
			spawner->IncreaseRefCount();
			m_spawners[key] = spawner;
		}

		DontDestroyOnLoad(GetGameObject());
	}
	else
	{
		Destroy(GetGameObject());
	}

}
void GOTOEngine::EnemySpawnManager::OnDestroy()
{
	if (instance == this)
		instance = nullptr;
	for (auto& it : m_animControllers) {
		it.second->DecreaseRefCount();
	}
	for (auto& it : m_sprites) {
		it.second->DecreaseRefCount();
	}
	for (auto& it : m_spawners) {
		it.second->DecreaseRefCount();
	}
}

void GOTOEngine::EnemySpawnManager::Update()
{
	if(m_GameType == GAME1)
	{
		if (INPUT_GET_KEYDOWN(KeyCode::Q)) // p1 enemy 생성 (MoveEnemy)
		{
			CreateEnemy(E_EnemyType::move, static_cast<std::uint32_t>(EPlayerOwner::PLAYER_1));
		}
		if (INPUT_GET_KEYUP(KeyCode::W)) // p1 enemy 생성 (GimmickEnemy)
		{
			CreateEnemy(E_EnemyType::gimmick, static_cast<std::uint32_t>(EPlayerOwner::PLAYER_1));
		}
		if (INPUT_GET_KEYUP(KeyCode::E)) // p1 enemy 생성 (ItemEnemy)
		{
			CreateEnemy(E_EnemyType::itemspawn, static_cast<std::uint32_t>(EPlayerOwner::PLAYER_1));
		}
		if (INPUT_GET_KEYDOWN(KeyCode::I)) // p2 enemy 생성 (MoveEnemy)
		{
			CreateEnemy(E_EnemyType::move, static_cast<std::uint32_t>(EPlayerOwner::PLAYER_2));
		}
		if (INPUT_GET_KEYDOWN(KeyCode::O)) // p2 enemy 생성 (GimmickEnemy)
		{
			CreateEnemy(E_EnemyType::gimmick, static_cast<std::uint32_t>(EPlayerOwner::PLAYER_2));
		}
		if (INPUT_GET_KEYUP(KeyCode::P)) // p2 enemy 생성 (ItemEnemy)
		{
			CreateEnemy(E_EnemyType::itemspawn, static_cast<std::uint32_t>(EPlayerOwner::PLAYER_2));
		}
	}
	else if (m_GameType == GAME2)
	{
		if (INPUT_GET_KEYDOWN(KeyCode::Q))
		{
			CreateDefenseFlyEnemey();
		}
	}

}

void GOTOEngine::EnemySpawnManager::CreateDefenseFlyEnemey()
{
	auto spawner = GetSpawner(L"지상");
	GameObject* newEnemyObject = new GameObject(L"지상");
	auto comp = newEnemyObject->AddComponent<MoveEnemy>();
	comp->SetupSpawner(spawner, static_cast<E_Move_Enemy_Type>(crow_1));

}

// 플레이어에 타입 랜덤 생성
void GOTOEngine::EnemySpawnManager::CreateEnemy(E_EnemyType enemyType, std::uint32_t player, bool isGimmick)
{
	if (GameManager::instance == nullptr) return;

	/*// 설정대로 스폰 (디버그 용)
	GameObject* newEnemyObject = (enemyType == competition)? nullptr : new GameObject(L"Enemy");
	switch(enemyType)
	{
	case move:
	{
		ParameterMap params;
		params["EnemyType"] = static_cast<E_Move_Enemy_Type>(crow_1);
		params["isGimmick"] = isGimmick;

		newEnemyObject->AddComponent<MoveEnemy>();
		newEnemyObject->GetComponent<MoveEnemy>()->Initialize(params);
	}
	break;
	case gimmick:
	{
		newEnemyObject->AddComponent<GimmickEnemy>();
		newEnemyObject->GetComponent<GimmickEnemy>()->Initialize(thiefmole);
	}
	break;
	case itemspawn:
	{
		newEnemyObject->AddComponent<ItemEnemy>();
		newEnemyObject->GetComponent<ItemEnemy>()->Initialize(goldCrow);
	}
	break;
	case competition:
	{
		CreateGoleMole();
	}
	break;
	}
	if (!newEnemyObject) return;
	//*/

	//*// 랜덤 스폰
	GameObject* newEnemyObject = new GameObject(L"Enemy");
	switch (enemyType)
	{
	case move:
	{
		ParameterMap params;
		params["EnemyType"] = static_cast<E_Move_Enemy_Type>(std::rand() % E_Move_Enemy_Type::move_type_count);
		params["isGimmick"] = isGimmick;

		newEnemyObject->AddComponent<MoveEnemy>();
		newEnemyObject->GetComponent<MoveEnemy>()->Initialize(params);
	}
	break;
	case gimmick:
	{
		auto randomType = static_cast<E_Gimmick_Enemy_Type>(std::rand() % E_Gimmick_Enemy_Type::gimmick_type_count);
		newEnemyObject->AddComponent<GimmickEnemy>();
		newEnemyObject->GetComponent<GimmickEnemy>()->Initialize(randomType);
	}
	break;
	case itemspawn:
	{
		auto randomType = static_cast<E_Item_Enemy_Type>(std::rand() % (E_Item_Enemy_Type::item_type_count));
		newEnemyObject->AddComponent<ItemEnemy>();
		newEnemyObject->GetComponent<ItemEnemy>()->Initialize(randomType);
	}
	break;
	}
	//*/

	newEnemyObject->GetComponent<BaseEnemyObject>()->SetEnemyLayer(player);
	newEnemyObject->layer = player;

	if (player & static_cast<std::uint32_t>(EPlayerOwner::PLAYER_1))
	{
		m_p1Enemy.push_back(newEnemyObject);
	}
	if (player & static_cast<std::uint32_t>(EPlayerOwner::PLAYER_2))
	{
		m_p2Enemy.push_back(newEnemyObject);
	}
}

// 설정대로 스폰
void GOTOEngine::EnemySpawnManager::CreateEnemy(E_EnemyType enemyType, int detailType, std::uint32_t player, bool isGimmick)
{
	if (GameManager::instance == nullptr) return;

	GameObject* newEnemyObject = new GameObject(L"Enemy");

	switch (enemyType)
	{
	case move:
	{
		ParameterMap params;
		params["EnemyType"] = static_cast<E_Move_Enemy_Type>(detailType);
		params["isGimmick"] = isGimmick;

		newEnemyObject->AddComponent<MoveEnemy>();
		newEnemyObject->GetComponent<MoveEnemy>()->Initialize(params);
	}
	break;
	case gimmick:
	{
		newEnemyObject->AddComponent<GimmickEnemy>();
		newEnemyObject->GetComponent<GimmickEnemy>()->Initialize(static_cast<E_Gimmick_Enemy_Type>(detailType));
	}
	break;
	case itemspawn:
	{
		newEnemyObject->AddComponent<ItemEnemy>();
		newEnemyObject->GetComponent<ItemEnemy>()->Initialize(static_cast<E_Item_Enemy_Type>(detailType));
	}
	break;
	}

	newEnemyObject->GetComponent<BaseEnemyObject>()->SetEnemyLayer(player);
	newEnemyObject->layer = player;

	if (player & static_cast<std::uint32_t>(EPlayerOwner::PLAYER_1))
	{
		m_p1Enemy.push_back(newEnemyObject);
	}
	if (player & static_cast<std::uint32_t>(EPlayerOwner::PLAYER_2))
	{
		m_p2Enemy.push_back(newEnemyObject);
	}
}

void GOTOEngine::EnemySpawnManager::CreateGoleMole()
{
	if (GameManager::instance == nullptr) return;

	if (m_goldMole)
	{ 
		GameObject::Destroy(m_goldMole); 
		m_goldMole = nullptr;
	}

	GameObject* newEnemyObject = new GameObject(L"Enemy");

	newEnemyObject->AddComponent<CompetEnemy>();
	newEnemyObject->GetComponent<CompetEnemy>()->Initialize(goldMole);
	
	newEnemyObject->GetComponent<BaseEnemyObject>()->SetEnemyLayer((1 << 1) | (1 << 2));
	newEnemyObject->layer = (1 << 1) | (1 << 2);
	
	m_goldMole = newEnemyObject;
}

void GOTOEngine::EnemySpawnManager::SetDeleteEnemy(std::uint32_t player, GameObject* enemy, bool _isPlayerAttack)
{  
   if (player & static_cast<std::uint32_t>(EPlayerOwner::PLAYER_1))
   {  
	   auto it = std::find(m_p1Enemy.begin(), m_p1Enemy.end(), enemy);
       if (it != m_p1Enemy.end())  
       {
		   m_p1Enemy.erase(it);
		   if (_isPlayerAttack)
		   {
			   E_EnemyType enemyType = static_cast<E_EnemyType>(enemy->GetComponent<BaseEnemyObject>()->BaseEnemyObject::GetType());
			   int detailType = enemy->GetComponent<BaseEnemyObject>()->GetType();
			   CreateEnemy(enemyType, detailType, static_cast<std::uint32_t>(EPlayerOwner::PLAYER_2), true);
		   }
       }  
   }  
   if (player & static_cast<std::uint32_t>(EPlayerOwner::PLAYER_2))
   {  
       auto it = std::find(m_p2Enemy.begin(), m_p2Enemy.end(), enemy);  
       if (it != m_p2Enemy.end())  
       {  
		   m_p2Enemy.erase(it);
		   if (_isPlayerAttack)
		   {
			   E_EnemyType enemyType = static_cast<E_EnemyType>(enemy->GetComponent<BaseEnemyObject>()->BaseEnemyObject::GetType());
			   int detailType = enemy->GetComponent<BaseEnemyObject>()->GetType();
			   CreateEnemy(enemyType, detailType, static_cast<std::uint32_t>(EPlayerOwner::PLAYER_1), true);
		   }
       }  
   }  
}

void GOTOEngine::EnemySpawnManager::DestroyGoldMole()
{
	if (m_goldMole)
	{
		GameObject::Destroy(m_goldMole);
		m_goldMole = nullptr;
	}
}

void GOTOEngine::EnemySpawnManager::Setp1EnemyAllDestroy()
{
	for (GameObject* enemy : m_p1Enemy)
	{
		if (enemy != nullptr)
		{
			GameObject::Destroy(enemy);
		}
	}
	m_p1Enemy.clear();
}
void GOTOEngine::EnemySpawnManager::Setp2EnemyAllDestroy()
{
	for (GameObject* enemy : m_p2Enemy)
	{
		if (enemy)
		{
			GameObject::Destroy(enemy);
		}
	}
	m_p2Enemy.clear();
}



template <typename T>
float GOTOEngine::EnemySpawnManager::GenerateRandom(T min, T max)
{
	// 스레드 안전하게 난수 생성
	std::lock_guard<std::mutex> lock(m_genMutex);

	// C++17의 if constexpr: 컴파일 타임에 T의 타입을 확인합니다.
	if constexpr (std::is_integral_v<T>)
	{
		// 1. 만약 T가 int, long, short 등 정수 타입이라면...
		std::uniform_int_distribution<T> dist(min, max);
		return dist(m_gen);
	}
	else if constexpr (std::is_floating_point_v<T>)
	{
		// 2. 만약 T가 float, double 등 실수 타입이라면...
		std::uniform_real_distribution<T> dist(min, max);
		return dist(m_gen);
	}
}