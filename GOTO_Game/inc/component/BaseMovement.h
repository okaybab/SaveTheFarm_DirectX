#pragma once
#include <ScriptBehaviour.h>

namespace GOTOEngine
{
    enum E_Enemy_Move_Type
    {
        NONE = 0,					// 아무 움직임 없음 (0000) 
        MOVE_LEFT_RIGHT = 1 << 0,	// 좌우 이동 (0001)         PATH
        MOVE_UP_DOWN = 1 << 1,		// 상하 이동 (0010)         PATH
        MOVE_CIRCULAR = 1 << 2,		// 원형 이동 (0100)         OFFSET
        MOVE_PARABOLIC = 1 << 3,	// 포물선 이동 (1000)       PATH, OFFSET
        MOVE_WAVE = 1 << 4          // 물결 이동 (10000)        OFFSET
        // 이동 추가
    };

    // 0011 지그재그 (각진)
    // 1000 곡선 (주기가 긴)
    // 1001 포물선 (주기가 짧은)
    // 10011 지그재그 (물결)
    //
    // 0100 원형은 자유자재로 붙이기 가능
    // 1011 == 1000 곡선 (주기가 긴)

    enum class E_Move_Role
    {
        PATH,                       // 중심축을 움직이는 역할
        OFFSET                      // 중심축으로부터의 오프셋을 계산하는 역할
    };

    class BaseMovement : public ScriptBehaviour
    {
    protected:
        float m_moveSpeed = 10.0f;
        E_Move_Role m_role = E_Move_Role::PATH;
        int m_flag = E_Enemy_Move_Type::NONE;

        // 루프 상태
        bool m_isLoop = true;
        int m_flipDirection = 1;    // 방향 (1: 정방향, -1: 역방향)
        float m_minBounds = 0.0f;   // 최소 경계
        float m_maxBounds = 0.0f;   // 최대 경계

        Vector2 m_initializePos;

    public:
    BaseMovement()
    {
        REGISTER_BEHAVIOUR_MESSAGE(Awake);
        REGISTER_BEHAVIOUR_MESSAGE(OnDestroy);
    }
        virtual void Awake() {}
        virtual void OnDestroy() {}

        virtual void Initialize(int moveFlag, Vector2 initialPos, float speed)
        { 
            m_flag = moveFlag; 
            m_initializePos = initialPos;
        }

        // Get
        int GetDirection() const { return m_flipDirection; }
        E_Move_Role GetRole() const { return m_role; }
        bool IsLoop() const { return m_isLoop; }

        // Set
        void SetLoopMode(float min, float max)
        {
            m_isLoop = true;
            m_minBounds = min;
            m_maxBounds = max;
        }
        virtual void FlipDirection()
        {
            m_flipDirection *= -1;
        }

        // 이벤트
        virtual Vector2 Move(float deltaTime) = 0;
    };
}