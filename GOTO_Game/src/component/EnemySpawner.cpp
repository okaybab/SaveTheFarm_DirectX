#include "EnemySpawner.h"
#include "Screen.h"

#include "GameManager.h"

// enemy
#include "BaseEnemyObject.h"
#include "MoveEnemy.h"
#include "GimmickEnemy.h"
#include "ItemEnemy.h"

// move
#include "MovementLeftRight.h"
#include "MovementUpDown.h"
#include "MovementCircle.h"
#include "MovementParabolic.h"

using namespace GOTOEngine;

EnemySpawner* EnemySpawner::instance = nullptr;
std::mt19937 EnemySpawner::m_gen(std::random_device{}());
std::mutex EnemySpawner::m_genMutex;

void GOTOEngine::EnemySpawner::Awake()
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
			{L"황금새", L"../Resources/Animation/controller/GoldCrowAnimator_AnimController.json"}
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
		};
		for (const auto& [key, path] : spriteList)
		{
			auto sprite = Resource::Load<Sprite>(path.c_str());
			sprite->IncreaseRefCount();
			m_sprites[key] = sprite;
		}

		DontDestroyOnLoad(GetGameObject());
	}
	else
	{
		Destroy(GetGameObject());
	}

}
void GOTOEngine::EnemySpawner::OnDestroy()
{
	if (instance == this)
		instance = nullptr;
	for (auto& it : m_animControllers) {
		it.second->DecreaseRefCount();
	}
	for (auto& it : m_sprites) {
		it.second->DecreaseRefCount();
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
// 플레이어에 타입 랜덤 생성
void GOTOEngine::EnemySpawner::CreateEnemy(E_EnemyType enemyType, int player)
{
	if (GameManager::instance == nullptr) return;

	GameObject* newEnemyObject = new GameObject(L"Enemy");

	/*// 설정대로 스폰 (디버그 용)
	switch(enemyType)
	{
	case move:
		newEnemyObject->AddComponent<MoveEnemy>();
		newEnemyObject->GetComponent<MoveEnemy>()->Initialize(crow_1);
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
void GOTOEngine::EnemySpawner::CreateEnemy(E_EnemyType enemyType, int detailType, int player)
{
	if (GameManager::instance == nullptr) return;

	GameObject* newEnemyObject = new GameObject(L"Enemy");

	switch (enemyType)
	{
	case move:
		newEnemyObject->AddComponent<MoveEnemy>();
		newEnemyObject->GetComponent<MoveEnemy>()->Initialize(static_cast<E_Move_Enemy_Type>(detailType));
		break;
	case gimmick:
		newEnemyObject->AddComponent<GimmickEnemy>();
		newEnemyObject->GetComponent<GimmickEnemy>()->Initialize(static_cast<E_Gimmick_Enemy_Type>(detailType));
		break;
	case itemspawn:
		newEnemyObject->AddComponent<ItemEnemy>();
		newEnemyObject->GetComponent<ItemEnemy>()->Initialize(static_cast<E_Item_Enemy_Type>(detailType));
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

void GOTOEngine::EnemySpawner::SetDeleteEnemy(int _layer, GameObject* enemy, bool _isPlayerAttack)
{  
   if (_layer == 1)  
   {  
	   auto it = std::find(m_p1Enemy.begin(), m_p1Enemy.end(), enemy);
       if (it != m_p1Enemy.end())  
       {
		   m_p1Enemy.erase(it);
		   if (_isPlayerAttack)
		   {
			   E_EnemyType enemyType = static_cast<E_EnemyType>(enemy->GetComponent<BaseEnemyObject>()->BaseEnemyObject::GetType());
			   int detailType = enemy->GetComponent<BaseEnemyObject>()->GetType();
			   CreateEnemy(enemyType, detailType, 2);
		   }
       }  
   }  
   else if (_layer == 2)  
   {  
       auto it = std::find(m_p2Enemy.begin(), m_p2Enemy.end(), enemy);  
       if (it != m_p2Enemy.end())  
       {  
		   m_p2Enemy.erase(it);
		   if (_isPlayerAttack)
		   {
			   E_EnemyType enemyType = static_cast<E_EnemyType>(enemy->GetComponent<BaseEnemyObject>()->BaseEnemyObject::GetType());
			   int detailType = enemy->GetComponent<BaseEnemyObject>()->GetType();
			   CreateEnemy(enemyType, detailType, 1);
		   }
       }  
   }  
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