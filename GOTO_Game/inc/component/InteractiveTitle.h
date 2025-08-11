#pragma once
#include <ScriptBehaviour.h>
#include <IAttackAble.h>
#include <Joint.h>
#include <PhysicsManager.h>
#include <SoundManager.h>
#include <RigidBody2D.h>
#include <Mathf.h>
#include <random>

namespace GOTOEngine
{
	class InteractiveTitle : public ScriptBehaviour, public IAttackAble
	{
	private:
		RigidBody2D* m_rb;
		Joint* m_joint1;
		Joint* m_joint2;
		Body* m_jointbody1;
		Body* m_jointbody2;

		std::random_device rd;

		int m_frameCount = 0;

		float m_randomDir = 8.0f;

		bool m_jointDestroied = false;
		int m_hp = 5;

		bool m_isInitialized = false;
	public:
    InteractiveTitle()
    {
        REGISTER_BEHAVIOUR_MESSAGE(Awake);
		REGISTER_BEHAVIOUR_MESSAGE(FixedUpdate);
        REGISTER_BEHAVIOUR_MESSAGE(OnDestroy);
    }

		void Awake()
		{
			if (m_isInitialized)
				return;

			m_rb = GetComponent<RigidBody2D>();
			m_jointbody1 = new Body();
			m_jointbody1->Set({1.0f,1.0f},FLT_MAX);
			m_jointbody1->isTrigger = true;
			m_jointbody1->position.Set(350.0f, 550.0f);
			m_joint1 = new Joint();
			m_joint1->Set(m_jointbody1,m_rb->GetWrapperBody()->GetBody(), { 350.0f,540.0f });
			m_joint1->biasFactor = 0.5f;
			m_joint1->softness = 0.15f;
			PhysicsManager::Get()->AddJoint(m_joint1);
			PhysicsManager::Get()->AddBody(m_jointbody1);
			m_jointbody2 = new Body();
			m_jointbody2->Set({ 1.0f,1.0f }, FLT_MAX);
			m_jointbody2->isTrigger = true;
			m_jointbody2->position.Set(-350.0f, 550.0f);
			m_joint2 = new Joint();
			m_joint2->Set(m_rb->GetWrapperBody()->GetBody(), m_jointbody2, { -350.0f,540.0f });
			m_joint2->biasFactor = 0.6f;
			m_joint2->softness = 0.3f;
			PhysicsManager::Get()->AddJoint(m_joint2);
			PhysicsManager::Get()->AddBody(m_jointbody2);

			m_rb->SetPosition({ 0.0f, 440.0f });

			m_isInitialized = true;
		}

		void FixedUpdate()
		{
			m_frameCount++;

			if (m_frameCount > 280 + (40.0f * (m_randomDir/8.0f)))
			{
				m_rb->AddForce({ -2850.0f * m_randomDir, 980.0f  * m_randomDir});
				if (m_frameCount > 284 + (40.0f  * (m_randomDir / 8.0f)))
				{
					std::mt19937 gen(rd());
					std::uniform_real_distribution<float> distrib(-8.0f, 8.0f);
					m_frameCount = 0;
					m_randomDir = distrib(gen);
					float sign = m_randomDir > 0.0f ? 1.0f : -1.0f;
					m_randomDir = Mathf::Max(abs(m_randomDir), 0.5f) * sign;
				}
			}

		}

		void OnDestroy()
		{
			if (m_jointDestroied)
				return;

			if (m_jointbody1)
			{
				PhysicsManager::Get()->RemoveBody(m_jointbody1);
				delete m_jointbody1;
			}
			if (m_joint1)
			{
				PhysicsManager::Get()->RemoveJoint(m_joint1);
				delete m_joint1;
			}
			if (m_jointbody2)
			{
				PhysicsManager::Get()->RemoveBody(m_jointbody2);
				delete m_jointbody2;
			}
			if (m_joint2)
			{
				PhysicsManager::Get()->RemoveJoint(m_joint2);
				delete m_joint2;
			}
		}

		void TakeDamage(int attackerID, float damage) override
		{
			if(m_hp <= 1)
				m_rb->AddForce({ -12000.0f,2500000.0f });
			else
				m_rb->AddForce({ -25000.0f,5000000.0f });

			//m_rb->AddTorque({ 500.0f });
			SoundManager::instance->PlaySFX("Hit");

			m_hp -= damage;

			if (m_hp <= 0 && !m_jointDestroied)
			{
				m_jointDestroied = true;

				if (m_jointbody1)
				{
					PhysicsManager::Get()->RemoveBody(m_jointbody1);
					delete m_jointbody1;
				}
				if (m_joint1)
				{
					PhysicsManager::Get()->RemoveJoint(m_joint1);
					delete m_joint1;
				}
				if (m_jointbody2)
				{
					PhysicsManager::Get()->RemoveBody(m_jointbody2);
					delete m_jointbody2;
				}
				if (m_joint2)
				{
					PhysicsManager::Get()->RemoveJoint(m_joint2);
					delete m_joint2;
				}
			}
		}
	};
}