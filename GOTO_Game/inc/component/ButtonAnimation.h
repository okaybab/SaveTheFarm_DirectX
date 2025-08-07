#pragma once
#include <ScriptBehaviour.h>
#include <TimeManager.h>
#include <Delegate.h>
#include <Transform.h>
#include <random>
#include <Mathf.h>

namespace GOTOEngine
{
	class ButtonAnimation : public ScriptBehaviour
	{
	private:
		float m_angularVelocity = 0.0f;   // ω: 각속도
		float m_angle = 0.0f;

		float m_sourceScale = 1.0f;
		float m_scale = 1.0f;            // 현재 값 (예: 스케일)
		float m_scaleVelocity = 0.0f;         // 현재 속도
	public:
    ButtonAnimation()
    {
        REGISTER_BEHAVIOUR_MESSAGE(Awake);
        REGISTER_BEHAVIOUR_MESSAGE(Update);
    }
		float torqueStrength = 200.0f;    // 스프링 강도
		float damping = 3.0f;           // 감쇠
		float inertia = 0.16f;            // 관성 (질량 역할)

		float target = 1.0f;     // 목표 값 (원래 크기)
		float strength = 500.0f;  // 스프링 강도 (k)
		float scaleDamping = 8.0f;    // 감쇠 계수 (d)


		void Awake()
		{
			m_sourceScale = Mathf::Min(GetTransform()->GetLossyScale().x,GetTransform()->GetLossyScale().y);
		}

		void ApplyTorque(float torque)
		{
			m_angularVelocity += torque / inertia;
		}

		void PlayRotateAnimation()
		{
			std::random_device rd;
			std::mt19937 gen(rd());
			std::uniform_real_distribution<float> dist(20.0f, 35.0f);

			std::mt19937 gen2(std::random_device{}());
			std::bernoulli_distribution dist2(0.5); // 50% 확률로 true

			bool isMinus = dist2(gen); // true 또는 false

			ApplyTorque(dist(gen) * (isMinus ? -1 : 1));
		}

		void PlayScaleAnimaiton()
		{
			ApplyScaleForce(2.0f);
		}

		void ApplyScaleForce(float force)
		{
			m_scaleVelocity += force;
		}

		void Update()
		{
			float dt = Mathf::Min(TIME_GET_FIXED_DELTATIME(), TIME_GET_DELTATIME());

			// 스프링 힘 계산 (후크 법칙)
			float torque = -torqueStrength * m_angle - damping * m_angularVelocity;

			// 각속도와 각도 업데이트
			m_angularVelocity += torque / inertia * dt;
			m_angle += m_angularVelocity * dt;

			// 스케일 탄성 업데이트
			float force = -strength * (m_scale - target) - scaleDamping * m_scaleVelocity;
			float accel = force; // 질량 m = 1 가정
			m_scaleVelocity += accel * dt;
			m_scale += m_scaleVelocity * dt;

			// 실제 회전에 적용
			GetTransform()->SetLocalRotation(m_angle);
			GetTransform()->SetLocalScale(Vector2(m_sourceScale * m_scale, m_sourceScale * m_scale));
		}
	};
}