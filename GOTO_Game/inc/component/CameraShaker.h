#pragma once
#include <ScriptBehaviour.h>
#include <TimeManager.h>
#include <Transform.h>
#include <Mathf.h>

namespace GOTOEngine
{
	class CameraShaker : public ScriptBehaviour
	{
	private:
		Vector2 m_shakePos = { 0,0 };
		float m_shakeAmount = 1.0f;
		int m_noiseSeed = 45;
		float m_shakeSpeed = 1.0f;
		float m_shakeRecovery = 1.0f;
		float m_shakeTrauma = 0.0f;
	public:
    CameraShaker()
    {
        REGISTER_BEHAVIOUR_MESSAGE(Update);
    }
		void Update()
		{
			m_shakePos = { m_shakeAmount * (Mathf::PerlinNoise(m_noiseSeed, TIME_GET_TOTALTIME() * m_shakeSpeed) * 2 - 1),
				m_shakeAmount * (Mathf::PerlinNoise(m_noiseSeed + 2, TIME_GET_TOTALTIME() * m_shakeSpeed) * 2 - 1) };
			m_shakeTrauma = Mathf::Lerp(m_shakeTrauma, 0, m_shakeRecovery * TIME_GET_DELTATIME());

			GetTransform()->SetLocalPosition(m_shakePos * m_shakeTrauma);
		}

		void ShakeCamera(float speed, float amount, float recovery)
		{
			m_shakeSpeed = speed;
			m_shakeAmount = amount;
			m_shakeRecovery = recovery;
			m_shakeTrauma = 1.0f;
		}
	};
}