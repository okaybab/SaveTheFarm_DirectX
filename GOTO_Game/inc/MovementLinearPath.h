#pragma once
#include "BaseMovement.h"
#include "Transform.h"

namespace GOTOEngine
{
    class MovementLinearPath : public BaseMovement
    {
    private:
        Vector2 m_startPos;
        Vector2 m_endPos;
        float m_progress = 0.0f;

    public:
        Delegate<void> OnEndPoint;

    public:
        void Initialize(Vector2 _startPos, Vector2 _endPos, float speed)
        {
            m_startPos = _startPos;
            m_endPos = _endPos;
            m_moveSpeed = speed;
            m_role = E_Move_Role::PATH; // 역할은 PATH로 설정
        }

        Vector2 Move(float deltaTime) override
        {
            m_progress += deltaTime / m_moveSpeed;
            if (m_progress >= 1.0f)
            {
                m_progress = 1.0f;
                OnEndPoint.Invoke(); // 델리게이트 호출
            }
            // Lerp를 이용해 다음 위치 계산
            Vector2 nextPos = m_startPos + (m_endPos - m_startPos) * m_progress;

            // 현재 위치와의 차이, 즉 이동량(Displacement)을 반환
            Vector2 currentPos = GetGameObject()->GetTransform()->GetPosition();
            return nextPos - currentPos;
        }
        void OnDestroy() override
        {
            __super::OnDestroy();
            OnEndPoint.Clear();
        }
    };
}