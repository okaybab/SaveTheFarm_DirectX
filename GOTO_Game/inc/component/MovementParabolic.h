#pragma once
#include "BaseMovement.h"
#include <TimeManager.h>
#include "Screen.h"
#include "Transform.h"

#include <math.h>
#include <utility>

// enemy에 쓰이는 movement 입니다.

namespace GOTOEngine
{
    class MovementParabolic : public BaseMovement
    {
    private:
        float m_height = 50.0f;         // 포물선의 높이각도
        float m_maxX;
        float m_minX;

        // PATH 일 때, 이전 프레임 위치 기억변수
        Vector2 m_startPos;
        Vector2 m_endPos;
        float m_progress = 0.0f;        // 포물선 진행도 (0.0 ~ 1.0)

        bool m_flipXY;

    public:
        Delegate<void> OnFlipDirection;
        Delegate<void> OnEndPoint;

    public:
        void Initialize(float _min, float _max)
        {
            m_maxX = _max;
            m_minX = _min;
        }
        void Initialize(int moveFlag, Vector2 initialPos, float speed) override
        {
            __super::Initialize(moveFlag, initialPos, speed);

            m_moveSpeed = 2.0f * speed;

            if (m_role == E_Move_Role::PATH)
            {
                m_startPos = Vector2(m_minX * m_flipDirection, initialPos.y);
                m_endPos = Vector2(m_maxX * m_flipDirection, initialPos.y);
            }

            const int MASK = MOVE_LEFT_RIGHT | MOVE_UP_DOWN;
            int relevant_bits = m_flag & MASK; // XOR

            if (relevant_bits != 0 && relevant_bits != MASK) // 1001 or 1010
            {
                m_role = E_Move_Role::OFFSET;
                m_flipXY = m_flag & MOVE_UP_DOWN;
            }
            else // 1000 or 1011
            {
                m_role = E_Move_Role::PATH;
            }

        }
        void Initialize(Vector2 initialPos, Vector2 _startPos, Vector2 _endPos, float speed)
        {
            m_progress = 0.0f;

            m_initializePos = initialPos;
            m_role = E_Move_Role::PATH;

            m_startPos = _startPos;
            m_endPos = _endPos;
            m_height = 200.0f;
            m_moveSpeed = 2.0f * speed;
        }

        void Awake() override
        {
            __super::Awake();
            
        }
        void OnDestroy() override
        {
            __super::OnDestroy();
            OnFlipDirection.Clear();
            OnEndPoint.Clear();
        }
        Vector2 Move(float deltaTime) override
        {
            switch (m_role)
            {
            case E_Move_Role::PATH:
            {
                // 진행도 progress
                m_progress += deltaTime / m_moveSpeed;
                if (m_progress > 1.0f)
                {
                    if (m_isLoop)
                    {
                        m_progress = 0.0f;
                        FlipDirection();
                        OnFlipDirection.Invoke();
                        std::swap(m_startPos, m_endPos);
                    }
                    else m_progress = 1.0f;
                    OnEndPoint.Invoke();
                }

                float linearX = m_startPos.x + (m_endPos.x - m_startPos.x) * m_progress;
                float linearY = m_startPos.y + (m_endPos.y - m_startPos.y) * m_progress;

                float parabolicOffset = -4 * m_height * (m_progress - m_progress * m_progress);

                Vector2 targetPos = Vector2(linearX, linearY + parabolicOffset);
                return targetPos - GetGameObject()->GetTransform()->GetPosition();
            }
            break;
            case  E_Move_Role::OFFSET:
            {
                m_progress += deltaTime / m_moveSpeed;
                if (m_progress > 1.0f)
                {
                    m_progress = 0.0f;
                }
                float offset = 4 * m_height * (m_progress - m_progress * m_progress);
                if (m_flipXY)
                {
                    // X축 오프셋 벡터 반환
                    return Vector2(offset, 0);
                }
                else
                {
                    // Y축 오프셋 벡터 반환
                    return Vector2(0, offset);
                }
            }
            break;
            }
            return Vector2::Zero();
        }

    };
}