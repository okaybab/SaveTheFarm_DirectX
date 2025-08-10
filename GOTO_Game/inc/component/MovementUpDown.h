#pragma once
#include "BaseMovement.h"
#include <TimeManager.h>
#include "Transform.h"

#include <math.h>

// enemy의 상하 movement 입니다.

namespace GOTOEngine
{
    class MovementUpDown : public BaseMovement
    {
    private:
        float m_distance = 5.0f;
        float m_frequency = 1.0f; // 진동 수

		float m_maxY;
		float m_minY;

        bool isWave;

    public:
        void Initialize(float _min, float _max)
        {
            m_maxY = _max;
            m_minY = _min;
        }
        void Initialize(int moveFlag, Vector2 initialPos, float speed) override
        {
            __super::Initialize(moveFlag, initialPos, speed);

            m_moveSpeed = 140.0f * speed;

        }
        void Awake() override
        {
            __super::Awake();
       
            m_role = E_Move_Role::PATH;

            if (m_flag & MOVE_LEFT_RIGHT && m_flag & MOVE_WAVE)
            {
                // 0b0011 지그재그에서 flag wave가 켜져있으면 물결 무늬
                isWave = true;
                m_distance = 1.0f;
            }
        }
        Vector2 Move(float deltaTime) override
        {
            if (m_isLoop)
            {
				Vector2 currentPos = GetGameObject()->GetTransform()->GetPosition();
				if ((currentPos.y > m_maxY && GetDirection() > 0) ||
					(currentPos.y < m_minY && GetDirection() < 0))
				{
					FlipDirection();
				}

                if (isWave)
                {
                    float totalTime = TimeManager::Get()->GetTime();
                    float offsetY = sin(totalTime * m_frequency) * m_distance;
                    return Vector2(0, offsetY);
                }
                return Vector2(0, m_moveSpeed * m_flipDirection * deltaTime);
            }
            else
            {
                float totalTime = TimeManager::Get()->GetTime();
                float deltaY = sin(deltaTime * m_moveSpeed) * m_distance;
                return Vector2(0, deltaY);
            }
        }

    };
}
