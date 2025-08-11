#pragma once
#include <ScriptBehaviour.h>
#include <InputManager.h>
#include <GameObject.h>
#include <Animator.h>

#include <vector>
#include <random>
#include <mutex>
#include <type_traits>
#include <queue>

namespace GOTOEngine
{	
	//static_cast<std::uint32_t>(EPlayerOwner::PLAYER_1)
	enum class EPlayerOwner : std::uint32_t
	{
		NONE = 0,
		PLAYER_1 = 1 << 1,
		PLAYER_2 = 1 << 2,
	};
	enum E_Game_Type
	{
		TITLE,
		GAME1,	// 경쟁모드
		GAME2	// 협동모드
	};

	enum E_EnemyType;
	class EnemySpawnManager : public ScriptBehaviour
	{
	public:
    EnemySpawnManager()
    {
        REGISTER_BEHAVIOUR_MESSAGE(Awake);
        REGISTER_BEHAVIOUR_MESSAGE(OnDestroy);
        REGISTER_BEHAVIOUR_MESSAGE(Update);
    }
		static EnemySpawnManager* instance;
		E_Game_Type m_GameType;

		static std::mt19937 m_gen;
		static std::mutex m_genMutex;

		std::vector<GameObject*> m_p1Enemy;
		std::vector<GameObject*> m_p2Enemy;
		GameObject* m_goldMole;

		std::unordered_map<std::wstring, AnimatorController*> m_animControllers;
		std::unordered_map<std::wstring, Sprite*> m_sprites;



	public:
		virtual ~EnemySpawnManager() = default;

		void Awake();
		void Update();
		void OnDestroy();

		// Get
		template <typename T>
		static float GenerateRandom(T min, T max);
		std::vector<GameObject*>* Getp1Enemy() { return &m_p1Enemy; }
		std::vector<GameObject*>* Getp2Enemy() { return &m_p2Enemy; }

        AnimatorController* GetAnimation(const std::wstring& name) const {
           auto it = m_animControllers.find(name);  
           return (it != m_animControllers.end()) ? it->second : nullptr;  
        }
		Sprite* GetSprite(const std::wstring& name) const {
			auto it = m_sprites.find(name);
			return (it != m_sprites.end()) ? it->second : nullptr;
		}

		// Set
		void SetEGameType(E_Game_Type type) { m_GameType = type; }
		void SetDeleteEnemy(std::uint32_t player, GameObject* enemy, bool _isPlayerAttack = false);
		void SetDeleteGoldMole() { m_goldMole = nullptr; }
		void DestroyGoldMole();

		//bool CreateEnemy()
		void CreateEnemy(E_EnemyType enemyType, std::uint32_t player, bool isGimmick = false);
		void CreateEnemy(E_EnemyType enemyType, int detailType, std::uint32_t player, bool isGimmick = false);
		void CreateGoleMole();

		void Setp1EnemyAllDestroy();
		void Setp2EnemyAllDestroy();

	};
}

