#pragma once
#include "Singleton.h"
#include "box2d-lite/World.h"
#include "box2d-lite/Body.h"
#include "GameObject.h"
#include "Transform.h"
#include "RenderManager.h"
#include <unordered_map>

namespace GOTOEngine
{
	class PhysicsManager : public Singleton<PhysicsManager>
	{
	private:
		friend class Collider2D;
		friend class RigidBody2D;
		friend class Engine;
		class Body2DWrapper
		{
		private:
			friend class Collider2D;
			friend class RigidBody2D;
			friend class PhysicsManager;

			Body* m_pBody;		

			GameObject* m_pOwner;
			Collider2D* m_pCol;
			RigidBody2D* m_pRb;
		public:
			Body2DWrapper(GameObject* owner)
				: m_pBody(new Body())
				, m_pOwner(owner)
				, m_pCol(nullptr)
				, m_pRb(nullptr)
			{
				m_pBody->Set({ 0,0 }, FLT_MAX);
			}

			~Body2DWrapper()
			{
				if (m_pBody)
				{
					delete m_pBody;
				}
			}

			Body* GetBody() { return m_pBody; }
			
			Transform* GetTransform() { return m_pOwner->GetTransform(); }

			bool HasCollider() { return m_pCol; }
			bool HasRigidbody() { return m_pRb; }

			bool IsInPhysicsWorld() { return PhysicsManager::Get()->m_physicsWorld2D->IsValidBody(m_pBody); }

			void InitCollider(Collider2D* col);
			void InitRigidBody(RigidBody2D* rb);

			void ExcludeCollider();
			void ExcludeRigidBody();
		};

		

		int m_phyiscsIteration = 10;
		Vector2 m_gravity = { 0.0f, -50.0f };
		World* m_physicsWorld2D;

		bool m_needRefreshBodyInPhysicsWorld = false;    //RigidBody 등록 및 해제(파괴) 시 적용
		
		void RegisterRigidBody2D(RigidBody2D* rigidBody);
		void RegisterCollider2D(Collider2D* collider);

		void UnRegisterRigidBody2D(RigidBody2D* rigidBody);
		void UnRegisterCollider2D(Collider2D* collider);

		void RefreshBodyFromPhysicsWorld2D();

		void PhysicsWorld2DRefreshDirtySet() { m_needRefreshBodyInPhysicsWorld = true; }

		void PendingAddBodyInWrapper(Body* body2DWrapper)
		{
			PhysicsWorld2DRefreshDirtySet();
			m_AddPendingBody.push_back(body2DWrapper);
		}

		void PendingRemoveBodyInWrapper(Body* body2DWrapper)
		{
			PhysicsWorld2DRefreshDirtySet();
			m_removePendingBody.push_back(body2DWrapper);
		}

		std::vector<Body*> OverlapBox2DFromPhysicsWorld2D(const Vec2& center, const Vec2& size);

		void CheckAtiveBodyWrapper();

		std::unordered_map<GameObject*, Body2DWrapper*> m_currentBody2Ds;
		std::unordered_map<Body*, Body2DWrapper*> m_body2DwrapperMap;

		std::vector<Body*> m_AddPendingBody;
		std::vector<Body*> m_removePendingBody;
	public:
		void StartUp()
		{
			m_physicsWorld2D = new World({ m_gravity.x, m_gravity.y }, m_phyiscsIteration);
		}

		std::vector<GameObject*> OverlapBox2D(const Vector2& center, const Vector2& size);

		Body2DWrapper* GetBody2DWrapper(GameObject* go)
		{
			auto it = m_currentBody2Ds.find(go);

			if (it != m_currentBody2Ds.end())
				return m_currentBody2Ds[go];

			return nullptr;
		}

		Body2DWrapper* GetBody2DWrapper(Body* body)
		{
			auto it = m_body2DwrapperMap.find(body);
			if (it != m_body2DwrapperMap.end())
				return it->second;
			return nullptr;
		}

		const std::unordered_map<GameObject*, Body2DWrapper*>& GetBody2DWrappers() const
		{
			return m_currentBody2Ds;
		}

		void Simulate(float deltaTime)
		{
			if (m_physicsWorld2D)
				m_physicsWorld2D->Step(deltaTime);
		}

		void SetGravity(const Vector2& gravity)
		{
			m_gravity = gravity;
			m_physicsWorld2D->gravity = { m_gravity.x,m_gravity.y };
		}

		const Vector2& GetGravity() const { return m_gravity; }

		void PreApplyTransform();
		void ApplyTransform();

		void PreSyncPhysicsWorld()
		{
			CheckAtiveBodyWrapper();
			RefreshBodyFromPhysicsWorld2D();
		}

		void AddJoint(Joint* joint)
		{
			m_physicsWorld2D->Add(joint);
		}

		void RemoveJoint(Joint* joint)
		{
			m_physicsWorld2D->Remove(joint);
		}

		void AddBody(Body* body)
		{
			m_physicsWorld2D->Add(body);
		}

		void RemoveBody(Body* body)
		{
			m_physicsWorld2D->Remove(body);
		}

		void ShutDown()
		{
			if (m_physicsWorld2D)
				delete m_physicsWorld2D;

			m_physicsWorld2D = nullptr;
		}
	};
}

#define PHYSICS_OVERLAP_BOX2D(center, size) GOTOEngine::PhysicsManager::Get()->OverlapBox2D(center, size)