#include "PhysicsManager.h"
#include "RigidBody2D.h"
#include "Collider2D.h"
#include "Mathf.h"

void GOTOEngine::PhysicsManager::RefreshBodyFromPhysicsWorld2D()
{
	if (m_needRefreshBodyInPhysicsWorld)
	{
		for (auto body : m_AddPendingBody)
		{
			m_physicsWorld2D->Add(body);
		}
		m_AddPendingBody.clear();

		for (auto body : m_removePendingBody)
		{
			m_physicsWorld2D->Remove(body);
		}
		m_removePendingBody.clear();
	}
	m_needRefreshBodyInPhysicsWorld = false;
}

std::vector<Body*> GOTOEngine::PhysicsManager::OverlapBox2DFromPhysicsWorld2D(const Vec2& center, const Vec2& size)
{
	std::vector<Body*> results;

	Vec2 queryMin = center - 0.5f * size;
	Vec2 queryMax = center + 0.5f * size;

	for (Body* body : m_physicsWorld2D->bodies) // 또는 bodies 벡터에 직접 접근
	{
		// 무시할 바디 필터링 (옵션)
		//if (body->invMass == 0.0f)
		//	continue;

		Vec2 halfSize = 0.5f * body->width;
		Vec2 min = body->position - halfSize;
		Vec2 max = body->position + halfSize;

		bool overlap =
			!(max.x < queryMin.x || min.x > queryMax.x ||
				max.y < queryMin.y || min.y > queryMax.y);

		if (overlap)
		{
			results.push_back(body);
		}
	}

	return results;
}

void GOTOEngine::PhysicsManager::CheckAtiveBodyWrapper()
{
	for (auto pair : m_currentBody2Ds)
	{
		auto wrapperBody = pair.second;

		if (m_physicsWorld2D->IsValidBody(wrapperBody->GetBody())
			&& !wrapperBody->m_pOwner->IsActiveInHierarchy())
		{
			PendingRemoveBodyInWrapper(wrapperBody->GetBody());
		}

		else if (!m_physicsWorld2D->IsValidBody(wrapperBody->GetBody())
			&& wrapperBody->m_pOwner->IsActiveInHierarchy())
		{
			PendingAddBodyInWrapper(wrapperBody->GetBody());
		}
	}
}

std::vector<GOTOEngine::GameObject*> GOTOEngine::PhysicsManager::OverlapBox2D(const Vector2& center, const Vector2& size)
{
	std::vector<GameObject*> results;
	auto bodies = OverlapBox2DFromPhysicsWorld2D({ center.x, center.y }, { size.x, size.y });
	for (auto body : bodies)
	{
		auto wrapper = GetBody2DWrapper(body);
		if (wrapper && wrapper->m_pOwner)
			results.push_back(wrapper->m_pOwner);
	}
	return results;
}

void GOTOEngine::PhysicsManager::PreApplyTransform()
{
	for (auto pair : m_currentBody2Ds)
	{
		auto wrapperBody = pair.second;
		auto body = wrapperBody->GetBody();

		if (m_physicsWorld2D->IsValidBody(body)
			&& body->invMass == 0)
		{
			//if (!Object::IsValidObject(wrapperBody->GetTransform())
			//	|| wrapperBody->GetTransform()->IsDestroyed())
			//	continue;

			auto transform = wrapperBody->GetTransform();

			body->position = { transform->GetPosition().x, transform->GetPosition().y };
			body->rotation = transform->GetRotation() * Mathf::Deg2Rad;
		}
	}
}

void GOTOEngine::PhysicsManager::ApplyTransform()
{
	for (auto pair : m_currentBody2Ds)
	{
		auto wrapperBody = pair.second;
		auto body = wrapperBody->GetBody();

		if (m_physicsWorld2D->IsValidBody(body)
			&& body->invMass != 0)
		{
			auto transform = wrapperBody->GetTransform();

			transform->SetPosition({ body->position.x, body->position.y });
			transform->SetRotation(body->rotation * Mathf::Rad2Deg);
		}
	}
}

void GOTOEngine::PhysicsManager::RegisterRigidBody2D(RigidBody2D* rigidBody)
{
	auto go = rigidBody->GetGameObject();
	auto it = m_currentBody2Ds.find(go);

	if (it != m_currentBody2Ds.end())
	{
		auto body2DWrapper = m_currentBody2Ds[go];

		//이미 중복 생성되어 있음
		if (body2DWrapper->HasRigidbody())
			return;

		body2DWrapper->InitRigidBody(rigidBody);
		rigidBody->m_wrapperBody = body2DWrapper;
		return;
	}

	auto createdBody2DWrapper = new Body2DWrapper(rigidBody->GetGameObject());
	createdBody2DWrapper->InitRigidBody(rigidBody);
	createdBody2DWrapper->m_pOwner = rigidBody->GetGameObject();
	PendingAddBodyInWrapper(createdBody2DWrapper->GetBody());
	rigidBody->m_wrapperBody = createdBody2DWrapper;

	m_currentBody2Ds[go] = createdBody2DWrapper;
	m_body2DwrapperMap[createdBody2DWrapper->GetBody()] = createdBody2DWrapper;
}

void GOTOEngine::PhysicsManager::RegisterCollider2D(Collider2D* collider)
{
	auto go = collider->GetGameObject();
	auto it = m_currentBody2Ds.find(go);

	if (it != m_currentBody2Ds.end())
	{
		auto body2DWrapper = m_currentBody2Ds[go];

		//이미 중복 생성되어 있음
		if (body2DWrapper->HasCollider())
			return;

		body2DWrapper->InitCollider(collider);
		collider->m_wrapperBody = body2DWrapper;
		return;
	}

	auto createdBody2DWrapper = new Body2DWrapper(collider->GetGameObject());
	createdBody2DWrapper->InitCollider(collider);
	createdBody2DWrapper->m_pOwner = collider->GetGameObject();
	PendingAddBodyInWrapper(createdBody2DWrapper->GetBody());
	collider->m_wrapperBody = createdBody2DWrapper;

	m_currentBody2Ds[go] = createdBody2DWrapper;
	m_body2DwrapperMap[createdBody2DWrapper->GetBody()] = createdBody2DWrapper;
}

void GOTOEngine::PhysicsManager::UnRegisterRigidBody2D(RigidBody2D* rigidBody)
{
	auto go = rigidBody->GetGameObject();
	auto it = m_currentBody2Ds.find(go);

	if (it != m_currentBody2Ds.end())
	{
		auto body2DWrapper = m_currentBody2Ds[go];

		//이미 중복 파괴되어 있음
		if (!body2DWrapper->HasRigidbody())
			return;

		body2DWrapper->ExcludeRigidBody();

		//콜라이더도 없으면 아예 파괴
		if (!body2DWrapper->HasCollider())
		{
			m_currentBody2Ds.erase(it);
			m_body2DwrapperMap.erase(body2DWrapper->GetBody());
			PendingRemoveBodyInWrapper(body2DWrapper->GetBody());
			delete body2DWrapper;
		}
	}
}

void GOTOEngine::PhysicsManager::UnRegisterCollider2D(Collider2D* collider)
{
	auto go = collider->GetGameObject();
	auto it = m_currentBody2Ds.find(go);

	if (it != m_currentBody2Ds.end())
	{
		auto body2DWrapper = m_currentBody2Ds[go];

		//이미 중복 파괴되어 있음
		if (!body2DWrapper->HasCollider())
			return;

		body2DWrapper->ExcludeRigidBody();

		//강체도 없으면 아예 파괴
		if (!body2DWrapper->HasRigidbody())
		{
			m_currentBody2Ds.erase(it); 
			m_body2DwrapperMap.erase(body2DWrapper->GetBody());
			PendingRemoveBodyInWrapper(body2DWrapper->GetBody());
			delete body2DWrapper;
		}
	}
}

void GOTOEngine::PhysicsManager::Body2DWrapper::InitCollider(Collider2D* col)
{
	m_pBody->Set({50,50}, m_pBody->mass);
	m_pBody->position = { col->GetTransform()->GetPosition().x , col->GetTransform()->GetPosition().y };
	m_pCol = col;
}

void GOTOEngine::PhysicsManager::Body2DWrapper::InitRigidBody(RigidBody2D* rb)
{
	m_pBody->Set(m_pBody->width, 200.0f);
	m_pRb = rb;
}

void GOTOEngine::PhysicsManager::Body2DWrapper::ExcludeCollider()
{
	m_pBody->Set({ 0,0 }, m_pBody->mass);

	m_pCol = nullptr;
}

void GOTOEngine::PhysicsManager::Body2DWrapper::ExcludeRigidBody()
{
	m_pBody->Set(m_pBody->width, FLT_MAX);

	m_pRb = nullptr;
}
