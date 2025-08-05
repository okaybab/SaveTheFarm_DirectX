#pragma once
#include <ScriptBehaviour.h>
#include <InputManager.h>
#include <GameObject.h>
#include "ObjectPool.h"

#include <vector>
#include <random>
#include <mutex>
#include <queue>

namespace GOTOEngine
{	
	enum E_EnemyType;


	class EnemySpawner : public ScriptBehaviour
	{
	public:
    EnemySpawner()
    {
        REGISTER_BEHAVIOUR_MESSAGE(Awake);
        REGISTER_BEHAVIOUR_MESSAGE(OnDestroy);
        REGISTER_BEHAVIOUR_MESSAGE(Update);
    }

		static EnemySpawner* instance;
		static std::mt19937 m_gen;
		static std::mutex m_genMutex;

		std::vector<GameObject*> m_p1Enemy;
		std::vector<GameObject*> m_p2Enemy;



	public:
		virtual ~EnemySpawner() = default;

		void Awake();
		void Update();
		void OnDestroy();

		void CreateEnemy(E_EnemyType enemyType, int player);
		void CreateEnemy(E_EnemyType enemyType, size_t enemy, int player);

		std::vector<GameObject*>* Getp1Enemy() { return &m_p1Enemy; }
		std::vector<GameObject*>* Getp2Enemy() { return &m_p2Enemy; }

		// Get
		static float GenerateRandom(float min, float max);

		// Set
		bool SetDeleteEnemy(int _layer, GameObject* enemy);

		void Setp1EnemyAllDestroy();
		void Setp2EnemyAllDestroy();
	};
}

