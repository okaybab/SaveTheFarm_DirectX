#pragma once
#include <vector>
#include <string>
#include "Delegate.h"

namespace GOTOEngine
{
	class Scene
	{
	private:
		friend class GameObject;
		friend class Object;

		void RegisterGameObject(GameObject* go);
		void UnregisterGameObject(GameObject* go);
	protected:
		const std::wstring m_sceneName;
		std::vector<GameObject*> m_gameObjects;
		GameObject* CreateGameObject(std::wstring name);
		GameObject* CreateGameObject();
	public:
		Scene(const std::wstring sceneName) : m_sceneName(sceneName) {}
		virtual ~Scene() { Clear(); }

		void Clear();
		void Reset() { Clear(); Initialize(); }

		virtual void Initialize() { }
		//virtual void Exit() { Clear(); }
		const std::wstring& GetSceneName() const { return m_sceneName; }

		void PrintCurrentGameObjects();
	};
}
