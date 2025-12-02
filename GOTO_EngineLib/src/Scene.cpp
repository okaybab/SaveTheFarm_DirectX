#include "Scene.h"
#include "GameObject.h"
#include "Transform.h"
#include "WStringHelper.h"

using namespace GOTOEngine;

void Scene::RegisterGameObject(GameObject* go)
{
	m_gameObjects.push_back(go);
}

//순서를 보장하지 않는 제거방식 -> 하지만 가장빠름
void Scene::UnregisterGameObject(GameObject* go)
{
	auto it = std::find(m_gameObjects.begin(), m_gameObjects.end(), go);
	if (it != m_gameObjects.end()) {
		*it = std::move(m_gameObjects.back()); // 마지막 원소를 덮어씀
		m_gameObjects.pop_back();
	}
}

GameObject* GOTOEngine::Scene::CreateGameObject(std::wstring name)
{
	auto go = new GameObject(this, name);
	return go;
}

GameObject* GOTOEngine::Scene::CreateGameObject()
{
	auto go = new GameObject(this);
	return go;
}

void Scene::Clear()
{
	//iterator 오염 방지 -> UnregisterGameObject()호출 방지
	for (auto& go : m_gameObjects)
	{
		go->m_scene = nullptr;
	}
	
	//실제 파괴
	for (auto& go : m_gameObjects)
	{
		Object::DestroyImmediate(go);
	}

#ifdef _DEBUG_DESTRUCTION
	std::cout << "Clearing Scene Name : " << WStringHelper::wstring_to_string(m_sceneName) << std::endl;
	std::cout << " - cleared obj : " << m_gameObjects.size() << std::endl;
	for (auto& sceneGameObj : m_gameObjects)
	{
		std::cout << "  =>  ID [ " << sceneGameObj->GetInstanceID() << " ] " << std::endl;
		std::cout << "  =>  Name [ " << WStringHelper::wstring_to_string(sceneGameObj->name) << " ] " << std::endl;

		for (auto& comp : sceneGameObj->GetAllComponents())
		{
			std::cout << "      =>>  Comp [ " << WStringHelper::wstring_to_string(comp->name) << " | " << typeid(*comp).name() << " ] " << std::endl;
		}
	}
#endif //_DEBUG_DESTRUCTION
	m_gameObjects.clear();
}

void GOTOEngine::Scene::PrintCurrentGameObjects()
{
	std::cout << "Print Current Objects's Scene Name : " << WStringHelper::wstring_to_string(m_sceneName) << std::endl;
	std::cout << " - current objects : " << m_gameObjects.size() << std::endl;
	for (auto& sceneGameObj : m_gameObjects)
	{
		std::cout << "  =>  ID [ " << sceneGameObj->GetInstanceID() << " ] " << std::endl;
		std::cout << "  =>  Name [ " << WStringHelper::wstring_to_string(sceneGameObj->name) << " ] " << std::endl;

		for (auto& comp : sceneGameObj->GetAllComponents())
		{
			std::cout << "      =>>  Comp [ " << WStringHelper::wstring_to_string(comp->name) << " | " << typeid(*comp).name() << " ] " << std::endl;
		}
	}
}
