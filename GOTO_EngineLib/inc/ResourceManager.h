#pragma once
#include <Singleton.h>
#include <vector>
#include <unordered_map>
#include <string>
#include <Color.h>
#include <wincodec.h>
#include <typeindex>
#include "Object.h"


namespace GOTOEngine
{
	class ResourceManager : public Singleton<ResourceManager>
	{
	private:
		friend class Engine;
		friend class Resource;

		std::vector<Resource*> m_resources;
		std::unordered_map < std::type_index, std::unordered_map <std::wstring, Resource*> > m_resourceTable; // map < typename, map < filePath, Resource* > >

		void StartUp();
		void ShutDown();
		void Clear();

		void RegisterResource(Resource* resource);
		void UnRegisterResource(Resource* resource);

		void DestroyUnusedResource();

		template <typename T>
		T* Load(std::wstring filePath)
		{
			//T는 자신을 상속받는 클래스만
			static_assert(std::is_base_of<Resource, T>::value, "T must be derived from Resource");

			std::type_index typeIdx = typeid(T); // 요청된 타입 T의 type_index

			//리소스 매니저에서 리소스테이블 내부 검색
			//--- 1. 타입 검색
			auto typeMapIt = m_resourceTable.find(typeIdx);
			if (typeMapIt != m_resourceTable.end())
			{
				//--- 2. 파일경로 검색
				auto& filePathMap = typeMapIt->second; 
				auto resourceIt = filePathMap.find(filePath);

				if (resourceIt != filePathMap.end())
				{
					//--- 3. 리소스 검색: 파일 경로로 리소스가 있는 경우
					// 댕글러 포인터 확인
					if (Object::IsValidObject(resourceIt->second) 
						&& !resourceIt->second->IsDestroyed())
					{
						// 캐스팅이 안전한지 확인 (dynamic_cast 사용, static_cast도 가능한지 나중에 체크하기) 
						T* existingResource = dynamic_cast<T*>(resourceIt->second);
						if (existingResource) {
							//existingResource->IncreaseRefCount();
							return existingResource;
						}
					}
					// 유효하지 않은 리소스로 판단, 리소스 테이블에서 원소 삭제
					filePathMap.erase(resourceIt);
				}
			}

			//없는 경우
			T* newResource = new T();
			newResource->m_filePath = filePath; // 파일 경로 갱신
			newResource->LoadFromFilePath(filePath); // 파일에서 데이터 로드

			if (!newResource->IsValidData())
			{
				Object::DestroyImmediate(newResource);
				return nullptr;
			}

			// ResourceManager에 새로운 리소스 등록
			m_resourceTable[typeIdx].insert_or_assign(filePath, newResource);

			return newResource;
		}
	};
}
