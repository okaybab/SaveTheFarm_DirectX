#pragma once
#include <ScriptBehaviour.h>
#include <InputManager.h>
#include <Transform.h>
#include <TimeManager.h>
#include <Camera.h>
#include <Animator.h>

#include "CrosshairCollide.h"

namespace GOTOEngine
{
	class CrosshairMove : public ScriptBehaviour
	{
	private:
		CrosshairCollide* m_collider = nullptr; // Collider2D 컴포넌트
		const float slowSpeedFactor = 0.6f; // 슬로우 모드 속도 감소 비율
		Vector2 m_vel = { 0,0 };
	public:
    CrosshairMove()
    {
        SetExecutionOrder(5);
        REGISTER_BEHAVIOUR_MESSAGE(Awake);
        REGISTER_BEHAVIOUR_MESSAGE(OnEnable);
        REGISTER_BEHAVIOUR_MESSAGE(Update);
    }

		Vector2 clampOffset = { 0,0 };

		const float defalutMoveSpeed = 650.0f;
		float moveSpeed = 650.0f;
		int id = 0;
		Rect clampRect = { 0, 0, 1.0f, 1.0f }; // 화면의 크기에 맞춰 조정

		float timeSpeed = 1.0f;

		static bool reverseInput1;
		static bool reverseInput2;
		
		Animator* gimmickAnimator;
		SpriteRenderer* gimmickAnimSprite;

		Camera* cam;

		void Awake()
		{
			m_collider = GetComponent<CrosshairCollide>();
		}

		void OnEnable()
		{
			m_vel = { 0,0 };
		}

		void Update()
		{
			float hInput, vInput;

			hInput = 0;
			vInput = 0;
			
			if (id == 0)
			{
				hInput = (INPUT_GET_KEY(KeyCode::G) ? 1.0f : 0.0f) + (INPUT_GET_KEY(KeyCode::D) ? -1.0f : 0.0f);
				vInput = (INPUT_GET_KEY(KeyCode::R) ? 1.0f : 0.0f) + (INPUT_GET_KEY(KeyCode::F) ? -1.0f : 0.0f);
			}
			else
			{
				hInput = (INPUT_GET_KEY(KeyCode::RightArrow) ? 1.0f : 0.0f) + (INPUT_GET_KEY(KeyCode::LeftArrow) ? -1.0f : 0.0f);
				vInput = (INPUT_GET_KEY(KeyCode::UpArrow) ? 1.0f : 0.0f) + (INPUT_GET_KEY(KeyCode::DownArrow) ? -1.0f : 0.0f);
			}

			if (INPUT_GAMEPAD_IS_CONNECTED(id))
			{
				hInput += INPUT_GET_GAMEPAD_AXIS(id, GamepadAxis::LeftStickX);
				vInput += INPUT_GET_GAMEPAD_AXIS(id, GamepadAxis::LeftStickY);
			}

			if (id == 0 && reverseInput1)
			{
				hInput = -hInput;
				vInput = -vInput;
				if(!gimmickAnimSprite->GetEnabled())
				{
					gimmickAnimSprite->SetEnabled(true);
					gimmickAnimator->SetEnabled(true);
				}
			}
			else if(id == 0)
			{
				if (gimmickAnimSprite->GetEnabled())
				{
					gimmickAnimSprite->SetEnabled(false);
					gimmickAnimator->SetEnabled(false);
				}
			}

			if (id == 1 && reverseInput2)
			{
				hInput = -hInput;
				vInput = -vInput;
				if (!gimmickAnimSprite->GetEnabled())
				{
					gimmickAnimSprite->SetEnabled(true);
					gimmickAnimator->SetEnabled(true);
				}
			}
			else if(id == 1)
			{
				if (gimmickAnimSprite->GetEnabled())
				{
					gimmickAnimSprite->SetEnabled(false);
					gimmickAnimator->SetEnabled(false);
				}
			}

			auto currentSpeedFactor = 1.0f;
			if (m_collider && m_collider->GetCollideObjects().size() != 0)
			{
				currentSpeedFactor = slowSpeedFactor; // 슬로우 모드 적용
			}

	
			auto moveInput = Vector2::ClampMagnitude(Vector2{ hInput, vInput }, 1.0f);
			m_vel = Vector2::Lerp(m_vel, moveInput.Normalized() * moveInput.Magnitude() * moveSpeed * currentSpeedFactor, 12.0f * TIME_GET_DELTATIME());

			GetTransform()->SetLocalPosition(GetTransform()->GetPosition() + (m_vel * TIME_GET_DELTATIME()));

			Vector2 posMin = cam->ViewportToWorldPoint({ 0,0 });
			Vector2 posMax = cam->ViewportToWorldPoint({ 1.0f, 1.0f });

			auto currentPos = GetTransform()->GetPosition();

			GetTransform()->SetPosition({ Mathf::Clamp(currentPos.x,posMin.x + clampOffset.x,posMax.x + clampOffset.x), Mathf::Clamp(currentPos.y,posMin.y + clampOffset.y,posMax.y + clampOffset.y) });
		}
	};
}