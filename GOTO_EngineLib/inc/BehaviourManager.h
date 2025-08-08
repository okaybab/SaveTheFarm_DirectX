#pragma once
#include <Singleton.h>
#include <unordered_set>
#include <string>
#include <queue>
#include <algorithm>
#include "Behaviour.h"

namespace GOTOEngine
{
	class BehaviourManager : public Singleton<BehaviourManager>
	{
	private:
		friend class Engine;
		friend class Behaviour;

		bool m_needSort = false; // Behaviour 정렬이 필요한지 여부
		std::vector<Behaviour*> m_activeBehaviours; // 활성화된 Behaviour를 저장하는 벡터
		std::vector<Behaviour*> m_inactiveBehaviours; // 비활성화된 Behaviour를 저장하는 벡터
		std::unordered_set<Behaviour*> m_firstCallBehaviours;
	
		// Behaviour를 등록하는 함수
		void RegisterBehaviour(Behaviour* behaviour);
	
		// Behaviour를 제거하는 함수
		void UnregisterBehaviour(Behaviour* behaviour);
	
		// ExcutionOrder에 따라 Behaviour를 정렬하는 함수
		void SortBehaviours();
	
		void InitializeBehaviours();
		
		// 비활성화된 Behaviour를 감지하는 함수
		void DisableBehaviours();
	
		void BroadCastBehaviourMessage(const std::string& messageName);
	
		// 매개변수 있는 브로드캐스트
		template<typename... Args>
		void BroadCastBehaviourMessage(const std::string& messageName, Args&&... args)
		{
			for (auto& behaviour : m_activeBehaviours)
			{
				behaviour->CallMessage(messageName, std::forward<Args>(args)...);
			}
		}

		template<typename... Args>
		void AllBroadCastBehaviourMessage(const std::string& messageName, Args&&... args)
		{
			for (auto& behaviour : m_activeBehaviours)
			{
				behaviour->CallMessage(messageName, std::forward<Args>(args)...);
			}
			for (auto& behaviour : m_inactiveBehaviours)
			{
				behaviour->CallMessage(messageName, std::forward<Args>(args)...);
			}
		}
	
		void CheckAndSortBehaviours();
		void StartUp();
		void ShutDown();
	};
}