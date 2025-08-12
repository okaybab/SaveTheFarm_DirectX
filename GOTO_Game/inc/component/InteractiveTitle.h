#pragma once
#include <ScriptBehaviour.h>
#include <IAttackAble.h>
#include <Joint.h>
#include <PhysicsManager.h>
#include <SoundManager.h>
#include <RigidBody2D.h>
#include <Mathf.h>
#include <random>
#include <SpriteRenderer.h>

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
		SpriteRenderer* m_renderer;
		AnimationCurve m_curve = { R"({
    "keyframes": [
        {
            "time": 0.0,
            "value": 1.0,
            "in_tangent": 0.0,
            "out_tangent": 0.0,
            "tangent_mode": 1
        },
        {
            "time": 0.36,
            "value": 1.0,
            "in_tangent": 0.0,
            "out_tangent": 0.0,
            "tangent_mode": 1
        },
        {
            "time": 0.43,
            "value": 0.3475,
            "in_tangent": 0,
            "out_tangent": 0,
            "tangent_mode": 1
        },
        {
            "time": 0.575,
            "value": 0.9925,
            "in_tangent": 0,
            "out_tangent": 0,
            "tangent_mode": 1
        },
        {
            "time": 0.655,
            "value": 0.3425,
            "in_tangent": 0,
            "out_tangent": 0,
            "tangent_mode": 1
        },
        {
            "time": 0.83,
            "value": 0.9875,
            "in_tangent": 0,
            "out_tangent": 0,
            "tangent_mode": 1
        },
        {
            "time": 0.93,
            "value": 0.3325,
            "in_tangent": 0.0,
            "out_tangent": 0.0,
            "tangent_mode": 1
        },
        {
            "time": 1.08,
            "value": 0.9925,
            "in_tangent": 0.0,
            "out_tangent": 0.0,
            "tangent_mode": 1
        },
        {
            "time": 1.24,
            "value": 0.0,
            "in_tangent": 0.0,
            "out_tangent": 0.0,
            "tangent_mode": 1
        }
    ]
})" };

		std::random_device rd;

		int m_frameCount = 0;

		float m_randomDir = 8.0f;

		bool m_jointDestroied = false;
		int m_hp = 10;

		bool m_isInitialized = false;

		Vec2 m_initPosition;
		float m_initRotation;

		float m_initFriciton;
		float m_initMass;
		float m_initInvMass;
		float m_initInertia;
		float m_initInvInertia;

		int m_recreateJointFrameCount;
		float m_alphaMotionTimer = 0.0f;

	public:
    InteractiveTitle()
    {
        REGISTER_BEHAVIOUR_MESSAGE(Awake);
		REGISTER_BEHAVIOUR_MESSAGE(FixedUpdate);
        REGISTER_BEHAVIOUR_MESSAGE(OnDestroy);
		REGISTER_BEHAVIOUR_MESSAGE(Update);
    }

		void Awake()
		{
			if (m_isInitialized)
				return;

			m_rb = GetComponent<RigidBody2D>();

			m_renderer = m_rb->GetTransform()->GetChild(0)->GetComponent<SpriteRenderer>();

			if (m_rb)
			{
				auto box2dBody = m_rb->GetWrapperBody()->GetBody();

				m_initPosition = box2dBody->position;
				m_initRotation = box2dBody->rotation;
				m_initFriciton = box2dBody->friction;
				m_initMass = box2dBody->mass;
				m_initInvMass = box2dBody->invMass;
				m_initInertia = box2dBody->I;
				m_initInvInertia = box2dBody->invI;
			}

			ResetBodyAndCreateJoint();

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

			if (m_jointDestroied)
			{
				m_recreateJointFrameCount++;
				if (m_recreateJointFrameCount > 165) // (0.03125f / 1.0f) => 1초당 32프레임, 32 * 5 = 165 => 5초 대기
				{
					m_recreateJointFrameCount = 0;
					ResetBodyAndCreateJoint();
					m_rb->SetPosition({ 0.0f, 800.0f });
					m_hp = 10;
				}
			}
				
		}

		void Update()
		{
			if (m_jointDestroied && m_renderer)
			{
				m_alphaMotionTimer += TIME_GET_DELTATIME();

				float value = m_curve.Evaluate(m_alphaMotionTimer - 2.0f);

				m_renderer->SetColor({ 255,255,255,static_cast<uint8_t>(255.0f * value) });
			}
			else
			{
				m_renderer->SetColor({ 255,255,255,255 });
				m_alphaMotionTimer = 0.0f;
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
			if (m_hp <= 1)
			{
				std::mt19937 gen(rd());
				std::uniform_real_distribution<float> distrib(32.0f, 45.0f);
				std::uniform_real_distribution<float> distrib2(210.0f, 320.0f);
				std::uniform_int_distribution<int> distInt1(0,1);
				std::uniform_int_distribution<int> distInt2(0,1);

				m_rb->SetMass(8900.0f);
				m_rb->GetWrapperBody()->GetBody()->angularVelocity = 5.0f * (distInt1(gen) > 0 ? 1 : -1);
				m_rb->AddForce({ distrib(gen) * (distInt1(gen) > 0 ? 1 : -1), distrib2(gen) * (distInt2(gen) > 0 ? 1 : -1) });
			}
				
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

		void ResetBodyAndCreateJoint()
		{
			m_jointDestroied = false;

			auto box2dBody = m_rb->GetWrapperBody()->GetBody();

			box2dBody->position = m_initPosition;
			box2dBody->rotation = m_initRotation;
			box2dBody->friction = m_initFriciton;
			box2dBody->mass = m_initMass;
			box2dBody->invMass = m_initInvMass;
			box2dBody->I = m_initInertia;
			box2dBody->invI = m_initInvInertia;

			m_jointbody1 = new Body();
			m_jointbody1->Set({ 1.0f,1.0f }, FLT_MAX);
			m_jointbody1->isTrigger = true;
			m_jointbody1->position.Set(350.0f, 550.0f);
			m_joint1 = new Joint();
			m_joint1->Set(m_jointbody1, m_rb->GetWrapperBody()->GetBody(), { 350.0f,540.0f });
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
		}
	};
}