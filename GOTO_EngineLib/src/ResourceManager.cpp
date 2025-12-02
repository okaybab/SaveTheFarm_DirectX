#include "ResourceManager.h"
#include "D2DFont.h"
#include "Resource.h"


void GOTOEngine::ResourceManager::StartUp()
{
	
}

void GOTOEngine::ResourceManager::ShutDown()
{
	
}

void GOTOEngine::ResourceManager::Clear()
{
	//강제로 모든 리소스를 파괴
	auto resourcesCopy = m_resources;

	for (auto& res : resourcesCopy)
	{
		if (!Object::IsValidObject(res))
			continue;

		res->m_refCount = 0;
		Object::DestroyImmediate(res);
	}
	m_resourceTable.clear();
	m_resources.clear();
}

void GOTOEngine::ResourceManager::RegisterResource(Resource* resource)
{
	m_resources.push_back(resource);
}

void GOTOEngine::ResourceManager::UnRegisterResource(Resource* resource)
{
	auto it = std::find(m_resources.begin(), m_resources.end(), resource);
	if (it != m_resources.end()) {
		*it = std::move(m_resources.back()); // 마지막 원소를 덮어씀
		m_resources.pop_back();
	}
}

void GOTOEngine::ResourceManager::DestroyUnusedResource()
{
	auto resourcesCopy = m_resources;

	for (auto& res : resourcesCopy)
	{
		if (Object::IsValidObject(res) && res->m_refCount == 0)
		{
			Object::DestroyImmediate(res);
		}
	}
}

