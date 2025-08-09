#include "RigidBody2D.h"
#include "PhysicsManager.h"
#include "box2d-lite/Body.h"

GOTOEngine::RigidBody2D::RigidBody2D() : m_wrapperBody(nullptr)
{
	
}

void GOTOEngine::RigidBody2D::AdditionalInitialize()
{
	PhysicsManager::Get()->RegisterRigidBody2D(this);
}

GOTOEngine::RigidBody2D::~RigidBody2D()
{
	PhysicsManager::Get()->UnRegisterRigigdBody2D(this);
}

void GOTOEngine::RigidBody2D::AddForce(Vector2 force)
{
	if(GetWrapperBody())
		m_wrapperBody->GetBody()->AddForce({ force.x, force.y });
}

void GOTOEngine::RigidBody2D::AddTorque(float torque)
{
	if (GetWrapperBody())
		m_wrapperBody->GetBody()->AddTorque(torque);
}

GOTOEngine::Vector2 GOTOEngine::RigidBody2D::GetPosition()
{
	if (GetWrapperBody() && GetWrapperBody()->m_pBody)
		return Vector2{ m_wrapperBody->GetBody()->position.x,m_wrapperBody->GetBody()->position.y };

	else
		return { 0,0 };
}

void GOTOEngine::RigidBody2D::SetPosition(Vector2 pos)
{
	if (GetWrapperBody() && GetWrapperBody()->m_pBody)
	{
		m_wrapperBody->GetBody()->position = { pos.x, pos.y };
		if (m_wrapperBody->GetBody()->invMass != 0.0f)
			GetTransform()->SetPosition(pos);
	}
}


float GOTOEngine::RigidBody2D::GetRotation()
{
	if (GetWrapperBody() && GetWrapperBody()->m_pBody)
		return m_wrapperBody->GetBody()->rotation * Mathf::Rad2Deg;
	else
		return 0;
}

void GOTOEngine::RigidBody2D::SetRotation(float rot)
{
	if (GetWrapperBody() && GetWrapperBody()->m_pBody)
	{
		m_wrapperBody->GetBody()->rotation = rot * Mathf::Deg2Rad;
		if (m_wrapperBody->GetBody()->invMass != 0.0f)
			GetTransform()->SetRotation(rot);
	}
}
float GOTOEngine::RigidBody2D::GetMass()
{
	if (GetWrapperBody() && GetWrapperBody()->m_pBody)
		return m_wrapperBody->GetBody()->mass;
	else
		return 0.0f;
}

void GOTOEngine::RigidBody2D::SetMass(float mass)
{
	if (GetWrapperBody() && GetWrapperBody()->m_pBody)
	{
		if (mass != FLT_MAX)
			m_wrapperBody->GetBody()->mass = mass;
		else
		{
			auto body = m_wrapperBody->GetBody();
			body->invMass = 0.0f;
			body->I = FLT_MAX;
			body->invI = 0.0f;
		}
	}
		
}


