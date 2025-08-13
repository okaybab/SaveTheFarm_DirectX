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
        float m_offsetTime = 0.0f;
        float m_length = 100.0f;
        float m_frequency = 5.0f;

        Vector2 m_offsetDirection;

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
            m_role = E_Move_Role::PATH;
        }
        void testInitialize(float length, float frequency)
        {
            m_offsetTime = 0.0f;
            m_length = length;
            m_frequency = frequency;
            m_role = E_Move_Role::OFFSET;
        }

        void CalculateOffsetDirection(const Vector2& start, const Vector2& end)
        {
            Vector2 pathDirection = end - start;
            m_offsetDirection = Vector2(-pathDirection.y, pathDirection.x);
            m_offsetDirection.Normalize(); // 벡터의 길이를 1로 만듭니다.
        }

        void Awake() override
        {
            __super::Awake();
        }
        void OnDestroy() override
        {
            __super::OnDestroy();
            OnFlipDirection.Clear();
        }
        Vector2 Move(float deltaTime) override
        {
            if (m_role == E_Move_Role::OFFSET)
            {
                // 시간을 누적
                m_offsetTime += deltaTime;

                // sin 함수를 사용해 오프셋의 크기를 계산
                float offsetMagnitude = sin(m_offsetTime * m_frequency) * m_length;

                // 방향 벡터에 크기를 곱하여 최종 오프셋을 반환
                return m_offsetDirection * offsetMagnitude;
            }
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
