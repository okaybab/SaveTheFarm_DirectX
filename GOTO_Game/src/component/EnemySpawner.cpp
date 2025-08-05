#include "EnemySpawner.h"
#include "Screen.h"

// enemy
#include "BaseEnemyObject.h"
#include "MoveEnemy.h"
#include "GimmickEnemy.h"
#include "ItemEnemy.h"

// move
#include "MoveLeftRight.h"
#include "MoveUpDown.h"
#include "MoveCircle.h"
#include "MoveParabolic.h"

using namespace GOTOEngine;

EnemySpawner* EnemySpawner::instance = nullptr;
std::mt19937 EnemySpawner::m_gen(std::random_device{}());
std::mutex EnemySpawner::m_genMutex;

void GOTOEngine::EnemySpawner::Awake()
{
	if (!instance)
	{
		instance = this;
	}
	else
	{
		Destroy(GetGameObject());
	}
}

void GOTOEngine::EnemySpawner::Update()
{
	if (INPUT_GET_KEYDOWN(KeyCode::Q)) // p1 enemy 생성 (MoveEnemy)
	{
		CreateEnemy(E_EnemyType::move, 1);
	}
	if (INPUT_GET_KEYUP(KeyCode::W)) // p1 enemy 생성 (GimmickEnemy)
	{
		CreateEnemy(E_EnemyType::gimmick, 1);
	}
	if (INPUT_GET_KEYUP(KeyCode::E)) // p1 enemy 생성 (ItemEnemy)
	{
		CreateEnemy(E_EnemyType::itemspawn, 1);
	}
	if (INPUT_GET_KEYDOWN(KeyCode::I)) // p2 enemy 생성 (MoveEnemy)
	{
		CreateEnemy(E_EnemyType::move, 2);
	}
	if (INPUT_GET_KEYDOWN(KeyCode::O)) // p2 enemy 생성 (GimmickEnemy)
	{
		CreateEnemy(E_EnemyType::gimmick, 2);
	}
	if (INPUT_GET_KEYUP(KeyCode::P)) // p2 enemy 생성 (ItemEnemy)
	{
		CreateEnemy(E_EnemyType::itemspawn, 2);
	}
}

void GOTOEngine::EnemySpawner::OnDestroy()
{
	if (instance == this)
		instance = nullptr;
}

// 플레이어에 타입 랜덤 생성
void GOTOEngine::EnemySpawner::CreateEnemy(E_EnemyType enemyType, int player)
{
	GameObject* newEnemyObject = new GameObject(L"Enemy");

	/*// 설정대로 스폰 (디버그 용)
	switch(enemyType)
	{
	case move:
		newEnemyObject->AddComponent<MoveEnemy>();
		newEnemyObject->GetComponent<MoveEnemy>()->Initialize(mole);
		break;
	case gimmick:
		newEnemyObject->AddComponent<GimmickEnemy>();
		newEnemyObject->GetComponent<GimmickEnemy>()->Initialize(squirrel);
		break;
	case itemspawn:
		newEnemyObject->AddComponent<ItemEnemy>();
		newEnemyObject->GetComponent<ItemEnemy>()->Initialize(bombCrow);
		break;
	default:
		break;
	}
	//*/

	//*// 랜덤 스폰
	switch (enemyType)
	{
	case move:
	{
		auto randomType = static_cast<E_Move_Enemy_Type>(std::rand() % E_Move_Enemy_Type::move_type_count);
		newEnemyObject->AddComponent<MoveEnemy>();
		newEnemyObject->GetComponent<MoveEnemy>()->Initialize(randomType);
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
		auto randomType = static_cast<E_Item_Enemy_Type>(std::rand() % E_Item_Enemy_Type::item_type_count);
		newEnemyObject->AddComponent<ItemEnemy>();
		newEnemyObject->GetComponent<ItemEnemy>()->Initialize(randomType);
	}
	break;
	}
	//*/

	newEnemyObject->GetComponent<BaseEnemyObject>()->SetEnemyLayer(player);
	newEnemyObject->layer = 1 << player;

	if (player == 1)
	{
		m_p1Enemy.push_back(newEnemyObject);
	}
	else if (player == 2)
	{
		m_p2Enemy.push_back(newEnemyObject);
	}
}

// 설정대로 스폰
void GOTOEngine::EnemySpawner::CreateEnemy(E_EnemyType enemyType, size_t enemy, int player)
{
	GameObject* newEnemyObject = new GameObject(L"Enemy");

	switch (enemyType)
	{
	case move:
		newEnemyObject->AddComponent<MoveEnemy>();
		newEnemyObject->GetComponent<MoveEnemy>()->Initialize(static_cast<E_Move_Enemy_Type>(enemy));
		break;
	case gimmick:
		newEnemyObject->AddComponent<GimmickEnemy>();
		newEnemyObject->GetComponent<GimmickEnemy>()->Initialize(static_cast<E_Gimmick_Enemy_Type>(enemy));
		break;
	case itemspawn:
		newEnemyObject->AddComponent<ItemEnemy>();
		newEnemyObject->GetComponent<ItemEnemy>()->Initialize(static_cast<E_Item_Enemy_Type>(enemy));
		break;
	default:
		break;
	}

	newEnemyObject->GetComponent<BaseEnemyObject>()->SetEnemyLayer(player);
	newEnemyObject->layer = 1 << player;

	if (player == 1)
	{
		m_p1Enemy.push_back(newEnemyObject);
	}
	else if (player == 2)
	{
		m_p2Enemy.push_back(newEnemyObject);
	}
}

bool GOTOEngine::EnemySpawner::SetDeleteEnemy(int _layer, GameObject* enemy)
{  
   if (_layer == 1)  
   {  
       auto it = std::find(m_p1Enemy.begin(), m_p1Enemy.end(), enemy);  
       if (it != m_p1Enemy.end())  
       {  
           m_p1Enemy.erase(it);  
           return true;  
       }  
   }  
   else if (_layer == 2)  
   {  
       auto it = std::find(m_p2Enemy.begin(), m_p2Enemy.end(), enemy);  
       if (it != m_p2Enemy.end())  
       {  
           m_p2Enemy.erase(it);  
           return true;  
       }  
   }  

   return false;  
}

void GOTOEngine::EnemySpawner::Setp1EnemyAllDestroy()
{
	for (auto& enemy : m_p1Enemy)
	{
		if (enemy)
		{
			GameObject::Destroy(enemy);
			enemy = nullptr;
		}
	}
	m_p1Enemy.clear();
}
void GOTOEngine::EnemySpawner::Setp2EnemyAllDestroy()
{
	for (auto& enemy : m_p2Enemy)
	{
		if (enemy)
		{
			GameObject::Destroy(enemy);
			enemy = nullptr;
		}
	}
	m_p2Enemy.clear();
}

float GOTOEngine::EnemySpawner::GenerateRandom(float min, float max)
{
	// 스레드 안전하게 난수 생성
	std::lock_guard<std::mutex> lock(m_genMutex);
	std::uniform_real_distribution<float> dist(min, max);

	return dist(m_gen);
}