#pragma once
#include "Component.h"
#include "PhysicsManager.h"
#include "Vector2.h"

namespace GOTOEngine
{
	class RigidBody2D : public Component
	{
	private:
		friend class PhysicsManager;
		friend class PhysicsManager::Body2DWrapper;
		PhysicsManager::Body2DWrapper* m_wrapperBody;

		void AdditionalInitialize() override;

		~RigidBody2D();
	public:
		RigidBody2D();

		void AddForce(Vector2 force);
		void AddTorque(float torque);
		PhysicsManager::Body2DWrapper* GetWrapperBody() { return m_wrapperBody; };

		Vector2 GetPosition();
		void SetPosition(Vector2 pos);
		float GetRotation();
		void SetRotation(float rot);

		float GetMass();
		void SetMass(float mass);
	};
}