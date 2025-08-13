#include "ObjectDestructionManager.h"

void GOTOEngine::ObjectDestructionManager::ScheduleDestroy(Object* obj, float delay)
{
	//유효한 오브젝트인지 확인
	if (!Object::IsValidObject(obj)
		|| obj->IsDestroyed())
		return;

	//Transform의 경우엔 Destroy불가능
	if (dynamic_cast<Transform*>(obj))
	{
		return;
	}

	float currentTime = TIME_GET_TOTALTIME();
	float destroyTime = currentTime + delay;

	auto it = m_destroySchedule.find(obj);

	if (m_destroySchedule.end() != it)
	{
		if (it->second->delayedTime > destroyTime)
			it->second->delayedTime = destroyTime;
		return;
	}

	// 예약 파괴 정보 생성
	ObjectDestroyInfo* destroyInfo = new ObjectDestroyInfo(obj, destroyTime);
	m_destroySchedule[obj] = destroyInfo;
}

void GOTOEngine::ObjectDestructionManager::ImmediateDestroy(Object* obj)
{
	//유효한 오브젝트인지 확인
	if (!Object::IsValidObject(obj)
		|| obj->IsDestroyed())
		return;

	//Transform의 경우엔 Destroy불가능
	if (dynamic_cast<Transform*>(obj))
	{
		return;
	}

	//예약된 오브젝트의 경우 파괴예약을 취소하고 즉시 파괴
	auto it = m_destroySchedule.find(obj);
	if (it != m_destroySchedule.end())
	{
		// 예약정보 훼손
		delete it->second;
		m_destroySchedule[obj] = nullptr;
	}
	
	//지금부터 사실상 파괴되었음을 선언
	obj->MarkDestory();
	obj->Dispose();
	m_pendingDeleteObjects.push_back(obj); // 즉시 파괴할 오브젝트 목록에 추가
}

void GOTOEngine::ObjectDestructionManager::Update()
{
	// 현재 시간 기준으로 파괴예약된 오브젝트들 중 파괴시간이 지난 오브젝트들을 직접파괴
	float currentTime = TIME_GET_TOTALTIME();

	// 파괴 시간이 지나지 않은 오브젝트들은 그대로 두기
	for (auto it = m_destroySchedule.begin(); it != m_destroySchedule.end(); )
	{
		ObjectDestroyInfo* destroyInfo = it->second;
		if (destroyInfo && destroyInfo->delayedTime <= currentTime)
		{
			// 예약 파괴 시간이 지난 오브젝트를 파괴
			Object* obj = destroyInfo->obj;

			//지금부터 사실상 파괴되었음을 선언
			obj->MarkDestory();
			obj->Dispose();
			m_pendingDeleteObjects.push_back(obj); // 즉시 파괴할 오브젝트 목록에 추가
			delete destroyInfo; // 예약 정보도 삭제
			it = m_destroySchedule.erase(it); // 맵에서 제거
		}
		else
		{
			++it; // 다음 요소로 이동
		}
	}

	// 즉시파괴로 비정상적으로 사라진 파괴예약정보에 대한 제거
	// 남은 지연파괴 정보는 그대로 보존 -> 다음 프레임에 다시 확인
	for (auto it = m_destroySchedule.begin(); it != m_destroySchedule.end(); )
	{
		ObjectDestroyInfo* destroyInfo = it->second;
		if (!destroyInfo)
		{
			it = m_destroySchedule.erase(it); // 맵에서 제거
		}
		else
		{
			++it; // 다음 요소로 이동
		}
	}
}

void GOTOEngine::ObjectDestructionManager::Clear()
{
	//_CrtCheckMemory();
#ifdef _DEBUG_DESTRUCTION
	bool debugStart = false;
	if(!m_pendingDeleteObjects.empty())
	{
		debugStart = true;
		std::cout << std::endl;
		std::cout << " || Object Clear Start || - pending delete object count : " << m_pendingDeleteObjects.size() << std::endl;
	}
#endif // _DEBUG_DESTRUCTION

	for (auto& obj : m_pendingDeleteObjects)
	{
#ifdef _DEBUG_DESTRUCTION
		std::cout << "<==================================>" << std::endl;
		std::cout << " Instance ID - " << obj->GetInstanceID() << std::endl;
		std::cout << " Adress - " << obj << std::endl;
		std::cout << " Type - " << typeid(*obj).name() << std::endl;
		std::cout << "<==================================>" << std::endl;
#endif // _DEBUG_DESTRUCTION
		delete obj; // 실제 파괴
	}
	m_pendingDeleteObjects.clear(); // 파괴된 오브젝트 목록 초기화

#ifdef _DEBUG_DESTRUCTION
	if (debugStart)
	{
		std::cout << " || Object Clear End ||" << std::endl;
		std::cout << std::endl;
	}
#endif // _DEBUG_DESTRUCTION
}

void GOTOEngine::ObjectDestructionManager::ShutDown()
{
	// 모든 예약 파괴된 오브젝트들 파괴
	for (auto& pair : m_destroySchedule)
	{
		//안전한 파괴예약만 일괄 파괴
		ObjectDestroyInfo* destroyInfo = pair.second;
		if (destroyInfo)
		{
			if (!Object::IsValidObject(destroyInfo->obj) 
				|| destroyInfo->obj->IsDestroyed())
				continue; // 유효하지 않은 오브젝트는 무시

			destroyInfo->obj->MarkDestory();
			destroyInfo->obj->Dispose();
			m_pendingDeleteObjects.push_back(destroyInfo->obj);
			delete destroyInfo; // 예약 정보도 삭제
		}
	}
	m_destroySchedule.clear();
	Clear();
}
