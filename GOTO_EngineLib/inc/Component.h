#pragma once
#include "Object.h"
#include "GameObject.h"

namespace GOTOEngine
{
	class Transform;
	class Component : public Object
	{
	private:
		friend class GameObject;
		GameObject* m_gameObject;

	protected:
		Component() : m_gameObject(nullptr) {}
		~Component() 
		{ 
			if (IsValidObject(m_gameObject)) 
			{
				m_gameObject->UnRegisterComponent(this);
				m_gameObject = nullptr;
			}
		}

		virtual void AdditionalInitialize() {};
	public:
		GameObject* GetGameObject() { return m_gameObject; }
		Transform* GetTransform() { return m_gameObject->GetTransform(); }

		template <typename T>
		T* GetComponent()
		{
			if (IsValidObject(GetGameObject())
				&& !GetGameObject()->IsDestroyed())
			{
				return GetGameObject()->GetComponent<T>();
			}
			return nullptr;
		}

		template <typename T>
		std::vector<T*> GetComponents()
		{
			if (IsValidObject(GetGameObject())
				&& !GetGameObject()->IsDestroyed())
			{
				return GetGameObject()->GetComponents<T>();
			}
			return {};
		}

		template <typename T>
		T* AddComponent()
		{
			if (IsValidObject(GetGameObject())
				&& !GetGameObject()->IsDestroyed())
			{
				return GetGameObject()->AddComponent<T>();
			}
			return nullptr;
		}
	};
}
