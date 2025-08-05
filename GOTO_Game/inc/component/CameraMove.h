#pragma once
#include <ScriptBehaviour.h>
#include <InputManager.h>
#include <TimeManager.h>
#include <Transform.h>
#include <Camera.h>

namespace GOTOEngine
{
	class CameraMove : public ScriptBehaviour
	{
	private:
		float m_sizefactor = 1.0f;
		Vector2 m_moveVec = { 0,0 };
	public:
    CameraMove()
    {
        SetExecutionOrder(10);
        REGISTER_BEHAVIOUR_MESSAGE(Awake);
        REGISTER_BEHAVIOUR_MESSAGE(Update);
    }
		float moveSpeed = 620.0f;
		float maxMoveLength = 420;
		int id;

		void Awake()
		{
			auto camera = GetGameObject()->GetComponent<Camera>();
			if (camera)
			{
				m_sizefactor = camera->GetSize();
			}
		}

		void Update()
		{
			float hInput = 0.0f;

			if (id == 0)
			{
				hInput = (INPUT_GET_KEY(KeyCode::S) ? 1.0f : 0.0f) + (INPUT_GET_KEY(KeyCode::A) ? -1.0f : 0.0f);
			}
			else
			{
				hInput = (INPUT_GET_KEY(KeyCode::Slash) ? 1.0f : 0.0f) + (INPUT_GET_KEY(KeyCode::Period) ? -1.0f : 0.0f);
			}


			if (INPUT_GAMEPAD_IS_CONNECTED(id))
			{
				hInput += INPUT_GET_GAMEPAD_AXIS(id, GamepadAxis::RightStickX);
			}


			auto moveInput = Vector2::ClampMagnitude(Vector2{ hInput, 0.0f }, 1.0f) * moveSpeed * m_sizefactor;


			float smoothness = std::pow(0.1f, TIME_GET_DELTATIME());

			float lerpFactor = 1.0f - std::exp(-6.5f * TIME_GET_DELTATIME());

			m_moveVec = Vector2::Lerp(m_moveVec, moveInput, lerpFactor);

			GetTransform()->SetLocalPosition(GetTransform()->GetLocalPosition() + (m_moveVec * TIME_GET_DELTATIME()));
			
			auto currentPos = GetTransform()->GetLocalPosition();

			GetTransform()->SetLocalPosition({ Mathf::Clamp(currentPos.x,-maxMoveLength,maxMoveLength), 0.0f });
		}
	};
}