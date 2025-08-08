#pragma once
#include <ScriptBehaviour.h>
#include <IAttackAble.h>
#include <Joint.h>
#include <PhysicsManager.h>
#include <RigidBody2D.h>
#include <Mathf.h>

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
	public:
		InteractiveTitle()
		{
			REGISTER_BEHAVIOUR_MESSAGE(Awake);
			REGISTER_BEHAVIOUR_MESSAGE(OnDestroy);
			REGISTER_BEHAVIOUR_MESSAGE(FixedUpdate);
		}

		void FixedUpdate()
		{
			m_rb->AddForce({ 0.0f,20.0f });
		}

		void Awake()
		{
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
		}

		void OnDestroy()
		{
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
		}

		void TakeDamage(int attackerID, float damage) override
		{
			m_rb->AddForce({ -25000.0f,5000000.0f });
		}
	};
}