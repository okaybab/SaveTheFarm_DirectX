#pragma once
#include <ScriptBehaviour.h>
#include <GameObject.h>
#include <Delegate.h>
#include <InputManager.h>
#include <PhysicsManager.h>
#include <TimeManager.h>
#include <Collider2D.h>
#include <SpriteRenderer.h>
#include "ICollideAble.h"

namespace GOTOEngine
{
	class CrosshairCollide : public ScriptBehaviour
	{
	private:
		Collider2D* m_collider = nullptr;
		std::vector<GameObject*> m_collideObjs;

		Sprite* m_crosshair1Sprite = nullptr;
		Sprite* m_crosshairEmpty1Sprite = nullptr;
		Sprite* m_crosshair2Sprite = nullptr;
		Sprite* m_crosshairEmpty2Sprite = nullptr;

	public:
    CrosshairCollide()
    {
        SetExecutionOrder(2);
        REGISTER_BEHAVIOUR_MESSAGE(Awake);
        REGISTER_BEHAVIOUR_MESSAGE(FixedUpdate);
        REGISTER_BEHAVIOUR_MESSAGE(OnDestroy);
    }


		SpriteRenderer* spriteRenderer = nullptr;

		const std::vector<GameObject*>& GetCollideObjects() const { return m_collideObjs; }

		int id = 0;

		void Awake()
		{
			m_collider = GetComponent<Collider2D>();
	
			m_crosshair1Sprite = Resource::Load<Sprite>(L"../Resources/Demo/Crosshair.png");
			m_crosshair1Sprite->IncreaseRefCount();
			m_crosshair2Sprite = Resource::Load<Sprite>(L"../Resources/Demo/Crosshair2.png");
			m_crosshair2Sprite->IncreaseRefCount(); 
			m_crosshairEmpty1Sprite = Resource::Load<Sprite>(L"../Resources/Demo/Crosshair_empty.png");
			m_crosshairEmpty1Sprite->IncreaseRefCount();
			m_crosshairEmpty2Sprite = Resource::Load<Sprite>(L"../Resources/Demo/Crosshair_empty2.png");
			m_crosshairEmpty2Sprite->IncreaseRefCount();

			if(!spriteRenderer)
				spriteRenderer = GetComponent<SpriteRenderer>();
		}

		void OnDestroy()
		{
			m_crosshair1Sprite->DecreaseRefCount();
			m_crosshair2Sprite->DecreaseRefCount();
			m_crosshairEmpty1Sprite->DecreaseRefCount();
			m_crosshairEmpty2Sprite->DecreaseRefCount();
		}

		void FixedUpdate()
		{
			if (!IsValidObject(m_collider))
				return;

			m_collideObjs.clear();

			const Vector2 position = GetTransform()->GetPosition();
			const Vector2 size = m_collider->GetSize();
			auto objects = PHYSICS_OVERLAP_BOX2D(position, size);

			const int targetLayerMask = 1 << (id + 1);
			GameObject* self = GetGameObject();

			for (auto* obj : objects)
			{
				if (obj == self)
					continue;

				if ((obj->layer & targetLayerMask) == 0)
					continue;

				m_collideObjs.push_back(obj);

				for (auto* comp : obj->GetAllComponents() )
				{
					if (auto* collideAble = dynamic_cast<ICollideAble*>(comp))
					{
						collideAble->OnCollide(self);
					}
				}
			}

			if (m_collideObjs.size() != 0)
			{
				spriteRenderer->SetSprite(
					id == 0 ? m_crosshair1Sprite : m_crosshair2Sprite);
			}
			else
			{
				spriteRenderer->SetSprite(
					id == 0 ? m_crosshairEmpty1Sprite : m_crosshairEmpty2Sprite);
			}
		}
	};
}