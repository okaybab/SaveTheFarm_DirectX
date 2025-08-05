#pragma once
#include "BaseMovement.h"
#include <TimeManager.h>

#include <math.h>

// enemy에 쓰이는 movement 입니다.

namespace GOTOEngine
{
    class MoveCircle : public BaseMovement
    {
    private:
        float m_radius = 5.0f;

    public:
        void Awake() override
        {
            __super::Awake();
            m_moveSpeed = 30.0f;            // 원을 도는 속도
            m_role = E_Move_Role::OFFSET;
        }

        Vector2 Move(float deltaTime) override
        {
            float totalTime = TimeManager::Get()->GetTime();
            float angle = totalTime * m_moveSpeed;

            return Vector2(cos(angle) * m_radius, sin(angle) * m_radius);
        }
    };
}