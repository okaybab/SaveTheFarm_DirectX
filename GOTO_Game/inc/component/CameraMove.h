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

	public:
    CameraMove()
    {
        SetExecutionOrder(10);
        REGISTER_BEHAVIOUR_MESSAGE(Awake);
        REGISTER_BEHAVIOUR_MESSAGE(Update);
    }
		float moveSpeed = 245.0f;
		bool isSub = false;

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
			float vInput = 0.0f;

			if (isSub)
			{
				hInput = (INPUT_GET_KEY(KeyCode::RightArrow) ? 1.0f : 0.0f) + (INPUT_GET_KEY(KeyCode::LeftArrow) ? -1.0f : 0.0f);
				vInput = (INPUT_GET_KEY(KeyCode::UpArrow) ? 1.0f : 0.0f) + (INPUT_GET_KEY(KeyCode::DownArrow) ? -1.0f : 0.0f);
			}
			else
			{
				hInput = (INPUT_GET_KEY(KeyCode::D) ? 1.0f : 0.0f) + (INPUT_GET_KEY(KeyCode::A) ? -1.0f : 0.0f);
				vInput = (INPUT_GET_KEY(KeyCode::W) ? 1.0f : 0.0f) + (INPUT_GET_KEY(KeyCode::S) ? -1.0f : 0.0f);
			}

			GetTransform()->SetLocalPosition(GetTransform()->GetLocalPosition() + (Vector2{ hInput, vInput }.Normalized() * moveSpeed * m_sizefactor * TIME_GET_DELTATIME()));
		}
	};
}