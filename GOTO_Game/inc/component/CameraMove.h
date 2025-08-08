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
		Transform* m_playerTransform;
	public:
    CameraMove()
    {
        SetExecutionOrder(10);
        REGISTER_BEHAVIOUR_MESSAGE(Awake);
        REGISTER_BEHAVIOUR_MESSAGE(OnSceneLoaded);
        REGISTER_BEHAVIOUR_MESSAGE(Update);
    }
		float moveSpeed = 620.0f;
		float maxMoveLength = 420;
		float minMoveLength = -420;
		int id;

		Vector2 GetMoveVector() { return m_moveVec; }

		void OnSceneLoaded()
		{
			GameObject* player = nullptr;

			if (id == 0)
			{
				player = GameObject::Find(L"Player1");
			}
			else
			{
				player = GameObject::Find(L"Player2");
			}

			if (player == nullptr)
			{
				m_playerTransform = nullptr;
				return;
			}

			m_playerTransform = player->GetTransform();
		}

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

			float lerpFactor = 1.0f - std::exp(-6.5f * TIME_GET_DELTATIME());

			auto lastPosX = GetTransform()->GetLocalPosition().x;

			m_moveVec = Vector2::Lerp(m_moveVec, moveInput, lerpFactor);

			GetTransform()->SetLocalPosition(GetTransform()->GetLocalPosition() + (m_moveVec * TIME_GET_DELTATIME()));
			
			auto currentPosX = GetTransform()->GetLocalPosition().x;

			auto clampedPosX = Mathf::Clamp(currentPosX,minMoveLength,maxMoveLength);


			GetTransform()->SetLocalPosition({ clampedPosX, 0.0f });

			if (m_playerTransform)
			{
				m_playerTransform->SetPosition(m_playerTransform->GetPosition() + (Vector2{1,0} *(clampedPosX - lastPosX)));
			}
		}
	};
}