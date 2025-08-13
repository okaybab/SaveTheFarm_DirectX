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

        // 이전 프레임의 이상적인 위치를 기억할 변수 추가
        Vector2 m_previousIdealPos;

    public:
        Delegate<void> OnEndPoint;

    public:
        void Initialize(Vector2 _startPos, Vector2 _endPos, float speed)
        {
            m_startPos = _startPos;
            m_endPos = _endPos;
            m_moveSpeed = speed;
            m_role = E_Move_Role::PATH;

            // 변수 초기화
            m_progress = 0.0f;
            m_previousIdealPos = _startPos;
        }

        // 수정된 Move 함수
        Vector2 Move(float deltaTime) override
        {
            m_progress += deltaTime / m_moveSpeed;
            if (m_progress >= 1.0f)
            {
                // 루프나 종료 처리가 필요하다면 여기에 로직 추가
                m_progress = 1.0f; // 우선 1.0으로 고정
                OnEndPoint.Invoke();
            }

            // 이번 프레임의 이상적인 위치 계산
            Vector2 nextIdealPos = m_startPos + (m_endPos - m_startPos) * m_progress;

            // '이전 프레임의 이상적인 위치'와 '이번 프레임의 이상적인 위치'의 차이를 계산
            Vector2 displacement = nextIdealPos - m_previousIdealPos;

            // 다음 프레임을 위해 현재 위치를 저장
            m_previousIdealPos = nextIdealPos;

            // 순수한 경로의 이동량만 반환
            return displacement;
        }

        void OnDestroy() override
        {
            __super::OnDestroy();
            OnEndPoint.Clear();
        }
    };
}