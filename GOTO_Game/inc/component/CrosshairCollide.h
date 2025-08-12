#pragma once
#include <ScriptBehaviour.h>
#include <GameObject.h>
#include <Delegate.h>
#include <InputManager.h>
#include <PhysicsManager.h>
#include <TimeManager.h>
#include <Collider2D.h>
#include <SpriteRenderer.h>
#include <Camera.h>

#include <Screen.h>

#include "ICollideAble.h"

namespace GOTOEngine
{
	enum class CrosshairCollideMode
	{
		LocalScreen,
		CrossScreen,
	};

	class CrosshairCollide : public ScriptBehaviour
	{
	private:
		Collider2D* m_collider = nullptr;
		std::vector<GameObject*> m_collideObjs;

		CrosshairCollideMode m_mode = CrosshairCollideMode::LocalScreen;

		Sprite* m_crosshair1Sprite = nullptr;
		Sprite* m_crosshairEmpty1Sprite = nullptr;
		Sprite* m_crosshair2Sprite = nullptr;
		Sprite* m_crosshairEmpty2Sprite = nullptr;

		float m_middlePosX = 0.0f;  //스크린 가운데 지점 X좌표

	public:
		CrosshairCollide()
		{
			SetExecutionOrder(2);
			REGISTER_BEHAVIOUR_MESSAGE(Awake);
			REGISTER_BEHAVIOUR_MESSAGE(FixedUpdate);
			REGISTER_BEHAVIOUR_MESSAGE(OnDestroy);
			REGISTER_BEHAVIOUR_MESSAGE(OnSceneLoaded);
		}

		Camera* p1Cam = nullptr;
		Camera* p2Cam = nullptr;
		Camera* commonCam = nullptr;

		SpriteRenderer* spriteRenderer = nullptr;

		const std::vector<GameObject*>& GetCollideObjects() const { return m_collideObjs; }

		int id = 0;

		Vector2 GetColSize() { return m_collider->GetSize(); }

		void SetColSize(const Vector2& size) { m_collider->SetSize(size); }

		void OnSceneLoaded()
		{
			m_middlePosX = 0.0f;
		}

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

			if (!spriteRenderer)
				spriteRenderer = GetComponent<SpriteRenderer>();
		}

		void OnDestroy()
		{
			if (IsValidObject(m_crosshair1Sprite))
				m_crosshair1Sprite->DecreaseRefCount();
			if (IsValidObject(m_crosshair2Sprite))
				m_crosshair2Sprite->DecreaseRefCount();
			if (IsValidObject(m_crosshairEmpty1Sprite))
				m_crosshairEmpty1Sprite->DecreaseRefCount();
			if (IsValidObject(m_crosshairEmpty2Sprite))
				m_crosshairEmpty2Sprite->DecreaseRefCount();
		}

		void FixedUpdate()
		{
			if (!IsValidObject(m_collider))
				return;

			m_collideObjs.clear();

			switch (m_mode)
			{
			case CrosshairCollideMode::LocalScreen:
				LocalScreenModeUpdate();
				break;
			case CrosshairCollideMode::CrossScreen:
				CrossScreenModeUpdate();
				break;
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

		void LocalScreenModeUpdate()
		{
			CollectOverlapInfo(GetTransform()->GetPosition(), m_collider->GetSize(), 1 << (id + 1));
		}

		void CrossScreenModeUpdate()
		{
			if (!IsValidObject(commonCam))
				return;

			Vector2 colScreenPos = commonCam->WorldToScreenPoint(GetTransform()->GetPosition());
			float collideHalfSizeX = m_collider->GetSize().x * 0.5f;

			if (m_middlePosX == 0.0f)
			{
				m_middlePosX = Screen::GetWidth() * 0.5f;
			}

			//p1 cam 영역 확인
			bool isColInP1Cam = false;
			Vector2 inP1CamPos = { 0,0 };

			if (IsValidObject(p1Cam))
			{
				inP1CamPos = { colScreenPos.x - 480.0f + p1Cam->GetTransform()->GetPosition().x,colScreenPos.y - 540.0f + p1Cam->GetTransform()->GetPosition().y};
				isColInP1Cam = (colScreenPos.x - collideHalfSizeX) < m_middlePosX;
				if(id == 1)
					std::cout << inP1CamPos.x << std::endl;
			}

			//p1 cam 영역 콜라이드
			if (isColInP1Cam)
			{
				CollectOverlapInfo(inP1CamPos, m_collider->GetSize(), 1 << (id + 1));
			}

			//p2 cam 영역 확인
			bool isColInP2Cam = false;
			Vector2 inP2CamPos = { 0,0 };

			if (IsValidObject(p2Cam))
			{
				inP2CamPos = { colScreenPos.x - 1440.0f + p2Cam->GetTransform()->GetPosition().x,colScreenPos.y - 540.0f + p2Cam->GetTransform()->GetPosition().y };
				isColInP2Cam = (colScreenPos.x + collideHalfSizeX) >= m_middlePosX;
			}

			//p2 cam 영역 콜라이드
			if (isColInP2Cam)
			{
				CollectOverlapInfo(inP2CamPos, m_collider->GetSize(), 1 << (id + 1));
			}
		}

		void ChangeCollideMode(CrosshairCollideMode mode)
		{
			m_mode = mode;
		}

		void CollectOverlapInfo(const Vector2& position, const Vector2& size, int targetLayerMask)
		{
			auto objects = PHYSICS_OVERLAP_BOX2D(position, size);

			GameObject* self = GetGameObject();

			for (auto* obj : objects)
			{
				if (obj == self)
					continue;

				if ((obj->layer & targetLayerMask) == 0)
					continue;

				m_collideObjs.push_back(obj);

				for (auto* comp : obj->GetAllComponents())
				{
					if (auto* collideAble = dynamic_cast<ICollideAble*>(comp))
					{
						collideAble->OnCollide(self);
					}
				}
			}
		}
	};
}