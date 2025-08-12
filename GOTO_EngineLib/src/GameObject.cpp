#include "GameObject.h"
#include "Component.h"
#include "ObjectDestructionManager.h"
#include "Transform.h"
#include "Canvas.h"
#include "Graphic.h"
#include "RectTransform.h"
#include "Behaviour.h"

std::vector<GOTOEngine::GameObject*> GOTOEngine::GameObject::s_allGameObjects;

void GOTOEngine::GameObject::InitInstance()
{
	//transform은 직접 생성 후 m_components에 등록
	m_transform = new Transform();
	m_transform->m_gameObject = this;
	m_transform->SetParent(nullptr); // 부모가 없으므로 nullptr로 설정

	RegisterComponent(m_transform); // Transform을 컴포넌트로 등록

	UpdateActiveInHierarchy();

	s_allGameObjects.push_back(this); // 모든 게임 오브젝트 목록에 추가
}

void GOTOEngine::GameObject::RegisterComponent(Component* comp)
{
	m_components.push_back(comp);
}

void GOTOEngine::GameObject::UnRegisterComponent(Component* comp)
{
	auto it = std::find(m_components.begin(), m_components.end(), comp);
	if (it != m_components.end()) {
		*it = std::move(m_components.back()); // 마지막 원소를 덮어씀
		m_components.pop_back();
	}
}

void GOTOEngine::GameObject::UpdateActiveInHierarchy()
{
	if (Transform* parent = m_transform->GetParent())
	{
		m_activeInHierarchy = parent->GetGameObject()->IsActiveInHierarchy() && m_active;
	}
	else
	{
		m_activeInHierarchy = m_active; // 부모가 없으면 자기 자신만 활성화 여부를 따짐
	}

	// 자식들의 활성화 상태 갱신
	for (auto& child : m_transform->m_childs)
	{
		child->GetGameObject()->UpdateActiveInHierarchy();
	}
}

void GOTOEngine::GameObject::Dispose()
{
	Object::Dispose();

	////====== 트랜스 폼 파괴 ======////
	// 
	// 
	//트랜스폼의 차일드들도 일괄 파괴해야함...
	for (auto& child : m_transform->m_childs)
	{
		DestroyImmediate(child->GetGameObject()); //
	}
	//
	//
	////===========================////



	////====== 컴포넌트 파괴 =======////
	//
	//
	for (auto& comp : m_components)
	{
		if (!IsValidObject(comp))
			continue; // 이미 파괴된 컴포넌트는 무시
		DestroyImmediate(comp);
	}
	//
	//
	////===========================////
}

void GOTOEngine::GameObject::EnsureRectTransform()
{
	auto rectTransform = dynamic_cast<RectTransform*>(m_transform);
	if (!rectTransform)
	{
		auto parent = m_transform->GetParent();
		auto childs = m_transform->m_childs;

		if (IsValidObject(parent))
			parent->RemoveChild(m_transform);
		delete m_transform;
		rectTransform = new RectTransform();
		m_transform = rectTransform;
		m_transform->m_gameObject = this;
		m_transform->SetParent(parent, false);

		for (auto& child : childs)
		{
			child->m_parent = nullptr;
			child->SetParent(m_transform, false); // 부모를 새로 만든 RectTransform으로 설정
		}
		RegisterComponent(m_transform); // Transform을 컴포넌트로 등록
	}
}

GOTOEngine::GameObject::GameObject(std::wstring name)
	: Object(name)
	, m_tag("")
	, m_active(true)
{
	m_scene = SceneManager::Get() ? SCENE_GET_CURRENTSCENE() : nullptr;
	m_scene->RegisterGameObject(this);

	InitInstance();
}

GOTOEngine::GameObject::GameObject() 
	: m_tag("")
	, m_active(true)
{
	m_scene = SceneManager::Get() ? SCENE_GET_CURRENTSCENE() : nullptr;
	m_scene->RegisterGameObject(this);

	InitInstance();
}

GOTOEngine::GameObject::~GameObject()
{
	////====== 트랜스 폼 파괴 ======////
	// 
	// 
	//트랜스폼의 차일드들도 일괄 파괴해야함...
	for (auto& child : m_transform->m_childs)
	{
		if (!IsValidObject(child))
			continue; // 이미 파괴된 차일드는 무시
		//iterator 오염 방지 -> RemoveChild()방지;
		child->m_parent = nullptr;
	}
	m_transform->m_childs.clear(); // 자식 트랜스폼 목록 초기화

	if (IsValidObject(m_transform->m_parent))
		m_transform->m_parent->RemoveChild(m_transform);

	delete m_transform; // Transform 컴포넌트 파괴
	m_transform = nullptr;
	//
	//
	////===========================////



	////====== 컴포넌트 파괴 =======////
	//
	//
	for (auto& comp : m_components)
	{
		if (!IsValidObject(comp))
			continue; // 이미 파괴된 컴포넌트는 무시

		//iterator 오염 방지 -> UnregisterComponent()호출 방지
		comp->m_gameObject = nullptr;
	}
	m_components.clear();
	//
	//
	////===========================////

	////====== 씬 등록 해제 ========////
	if (m_scene)
	{
		m_scene->UnregisterGameObject(this);
		m_scene = nullptr;
	}
	////===========================////

	// 모든 게임 오브젝트 목록에서 제거
	auto it = std::find(s_allGameObjects.begin(), s_allGameObjects.end(), this);
	if (it != s_allGameObjects.end())
	{
		*it = std::move(s_allGameObjects.back()); // 마지막 원소를 덮어씀
		s_allGameObjects.pop_back();
	}
}

GOTOEngine::GameObject::GameObject(Scene* scene) 
	: m_scene(scene)
	, m_tag("")
	, m_active(true)
{
	m_scene->RegisterGameObject(this);
	InitInstance();
}

GOTOEngine::GameObject::GameObject(Scene* scene, std::wstring name)
	: m_scene(scene)
	, m_tag("")
	, m_active(true)
{
	m_scene->RegisterGameObject(this);
	InitInstance();
}

void GOTOEngine::GameObject::SetActive(bool active)
{
	m_active = active;

	UpdateActiveInHierarchy();
}

GOTOEngine::GameObject* GOTOEngine::GameObject::Find(const std::wstring& name)
{
	//s_allGameObjects에서 이름으로 검색
	for (auto& go : s_allGameObjects)
	{
		if (!go->IsActiveInHierarchy()
			|| go->IsDestroyed())
			continue;

		if (go->name == name)
		{
			return go;
		}
	}

	return nullptr;
}

GOTOEngine::GameObject* GOTOEngine::GameObject::FindWithTag(const std::string& name)
{
	//s_allGameObjects에서 태그로 검색
	for (auto& go : s_allGameObjects)
	{
		if (!go->IsActiveInHierarchy()
			|| go->IsDestroyed())
			continue;

		if (go->GetTag() == name)
		{
			return go;
		}
	}

	return nullptr;
}

std::vector<GOTOEngine::GameObject*> GOTOEngine::GameObject::FindGameObjectsWithTag(const std::string& name)
{
	auto GOs = std::vector<GameObject*>();

	for (auto& go : s_allGameObjects)
	{
		if (!go->IsActiveInHierarchy()
			|| go->IsDestroyed())
			continue;

		if (go->GetTag() == name)
		{
			GOs.emplace_back(go);
		}
	}

	return GOs;
}

//bool GOTOEngine::GameObject::IsActiveInHierarchy() const
//{
//	if (!m_active) return false;
//	
//	if (Transform* parent = m_transform->GetParent()) 
//		return parent->GetGameObject()->IsActiveInHierarchy();
//	
//	return true;
//}
