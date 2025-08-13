#pragma once
#include <ScriptBehaviour.h>
#include <TimeManager.h>
#include "IAttackAble.h"
#include "ICollideAble.h"
#include <Delegate.h>
#include "SoundManager.h"
#include <SpriteRenderer.h>

namespace GOTOEngine
{
	class CrosshairInteractButton : public ScriptBehaviour, public IAttackAble, public ICollideAble
	{
	private:
		bool m_lastColliding = false; // 마지막 충돌 상태
		bool m_isColliding = false; // 현재 충돌 상태
	public:
    CrosshairInteractButton()
    {
        SetExecutionOrder(2);
        REGISTER_BEHAVIOUR_MESSAGE(FixedUpdate);
        REGISTER_BEHAVIOUR_MESSAGE(OnDestroy);
    }
		Transform* parentButton = nullptr; // 부모 버튼 Transform
		float interactedTime = 0.0f;
		bool isInteracted = false;
		bool isSingleInteract = false;

		SpriteRenderer* renderer;

		Sprite* defButtonSprite;
		Sprite* p1ClickButtonSprite;
		Sprite* p2ClickButtonSprite;

		Delegate<void> onEnter;
		Delegate<void> onExit;
		Delegate<void> onInteract;

		void TakeDamage(int id,float dmg) override
		{
			isInteracted = true;
			interactedTime = TIME_GET_TOTALTIME();
			onInteract.Invoke();
			SoundManager::instance->PlaySFX("Hit");
		}

		void OnCollide(GameObject* obj) override
		{
			m_isColliding = true;
		}

		void OnDestroy()
		{
			if (IsValidObject(defButtonSprite)
				&& !defButtonSprite->IsDestroyed())
			{
				defButtonSprite->DecreaseRefCount();
			}
			if (IsValidObject(p1ClickButtonSprite)
				&& !p1ClickButtonSprite->IsDestroyed())
			{
				p1ClickButtonSprite->DecreaseRefCount();
			}
			if (IsValidObject(p2ClickButtonSprite)
				&& !p2ClickButtonSprite->IsDestroyed())
			{
				p2ClickButtonSprite->DecreaseRefCount();
			}
		}

		void FixedUpdate()
		{
			if (m_lastColliding != m_isColliding)
			{
				if (m_isColliding)
				{
					onEnter.Invoke();
				}
				else
				{
					onExit.Invoke();
				}
			}

			m_lastColliding = m_isColliding;
			m_isColliding = false;
		}

		void ChangeToDefaultSprite()
		{
			if (IsValidObject(defButtonSprite))
			{
				renderer->SetSprite(defButtonSprite);
			}
		}

		void ChangeToP1ClickSprite()
		{
			if (IsValidObject(p1ClickButtonSprite))
			{
				renderer->SetSprite(p1ClickButtonSprite);
			}
		}

		void ChangeToP2ClickSprite()
		{
			if (IsValidObject(p2ClickButtonSprite))
			{
				renderer->SetSprite(p2ClickButtonSprite);
			}
		}
	};
}