#pragma once
#include "BaseMovement.h"
#include <TimeManager.h>
#include "Screen.h"
#include "Transform.h"
#include "Delegate.h"

#include <math.h>

// enemy의 좌우 movement 입니다.

namespace GOTOEngine
{
    class MovementLeftRight : public BaseMovement
    {
    private:
        float m_distance = 1.0f;
		float m_maxX;
		float m_minX;

    public:
        Delegate<void> OnFlipDirection;

    public:
        void Initialize(float _min, float _max)
        {
            m_maxX = _max;
            m_minX = _min;
        }
        void Initialize(int moveFlag, Vector2 initialPos, float speed) override
        {
            __super::Initialize(moveFlag, initialPos, speed);

            m_moveSpeed = 80.0f * speed;

        }
        void Awake() override
        {
            __super::Awake();

            m_role = E_Move_Role::PATH;
        }
        void OnDestroy() override
        {
            __super::OnDestroy();
            OnFlipDirection.Clear();
        }
        Vector2 Move(float deltaTime) override
        {
            if (m_isLoop)
            {
                Vector2 currentPos = GetGameObject()->GetTransform()->GetPosition();
                if ((currentPos.x > m_maxX && GetDirection() > 0) ||
                    (currentPos.x < m_minX && GetDirection() < 0))
                {
                    FlipDirection();
                    OnFlipDirection.Invoke();
                }
                return Vector2(m_moveSpeed * m_flipDirection * deltaTime, 0);
            }
            else
            {
                float totalTime = TimeManager::Get()->GetTime();
                float deltaX = sin(deltaTime * m_moveSpeed) * m_distance;
                return Vector2(deltaX, 0);
            }
            
        }

    };
}
