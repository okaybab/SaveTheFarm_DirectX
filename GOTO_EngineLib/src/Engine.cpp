#include "Engine.h"
#include "InputManager.h"
#include "TimeManager.h"
#include "SceneManager.h"
#include "ObjectDestructionManager.h"
#include "RenderManager.h"
#include "ResourceManager.h"
#include "BehaviourManager.h"
#include "PhysicsManager.h"
#include "AudioManager.h"
#ifdef _OS_WINDOWS
#include "WICHelper.h"
#include "DWriteHelper.h"
#include "WinAPIWindow.h"
#endif
using namespace GOTOEngine;

bool Engine::Initialize(int width, int height, const wchar_t* title)
{
	//윈도우 생성 (OS별 초기화)
	IWindow* window = nullptr;

#ifdef _OS_WINDOWS
	HRESULT hr;

	hr = CoInitialize(nullptr);

	if (!SUCCEEDED(hr))
		return false;

    window = new WinAPIWindow();

	//한글 설정
	std::locale::global(std::locale(".UTF-8"));
	std::wcout.imbue(std::locale());
#endif

    if (!window->Create(width, height, title))
    {
        delete window;
        return false;
    }

    m_window = window;

	//매니저 초기화
#ifdef _OS_WINDOWS
	InputManager::Get()->Startup(static_cast<HWND>(m_window->GetNativeHandle()));
	TimeManager::Get()->StartUp();
#endif
	PhysicsManager::Get()->StartUp();
	SceneManager::Get()->StartUp();
    RenderManager::Get()->StartUp(static_cast<IWindow*>(m_window));

	RenderManager::Get()->SetTargetWidth(static_cast<float>(width));
	RenderManager::Get()->SetTargetHeight(static_cast<float>(height));

	AudioManager::Get()->StartUp();
	ResourceManager::Get()->StartUp();
	BehaviourManager::Get()->StartUp();
	ObjectDestructionManager::Get()->StartUp();

	//헬퍼 초기화
#ifdef _OS_WINDOWS
	WICHelper::StartUp();
	DWriteHelper::StartUp();
#endif

	return true;
}

void Engine::Run()
{
	m_isRunning = true;
	while (m_isRunning)
	{
		m_window->PollEvents();
		if (m_window->ShouldClose())
		{
			m_isRunning = false;
			break;
		}
		ProcessFrame();
	}
}

void Engine::ProcessFrame()
{
	//코어 업데이트
	InputManager::Get()->Update();
	TimeManager::Get()->Update();
	bool changedScene = SceneManager::Get()->CheckSceneChange();
	if (changedScene)
	{
		ObjectDestructionManager::Get()->Update();
		BehaviourManager::Get()->DisableBehaviours();
		ResourceManager::Get()->DestroyUnusedResource();
		ObjectDestructionManager::Get()->Clear();
		BehaviourManager::Get()->BroadCastBehaviourMessage("OnSceneLoaded");
	}

	//Behaviour 초기화 메시지 (필요한 객체에 한해)
	//BehaviourManager::Get()->CheckAndSortBehaviours();
	//BehaviourManager::Get()->BroadCastBehaviourMessage("Awake");
	//BehaviourManager::Get()->BroadCastBehaviourMessage("OnEnable");
	//BehaviourManager::Get()->BroadCastBehaviourMessage("Start");
	BehaviourManager::Get()->InitializeBehaviours();

	//고정 업데이트
	static float accumulator = 0.0f;
	accumulator += TimeManager::Get()->GetDeltaTime();

	if (accumulator > TimeManager::Get()->GetMaximumAllowedTimestep())
		accumulator = TimeManager::Get()->GetMaximumAllowedTimestep();

	//GetTime -> 고정시간 반환으로 변경
	TimeManager::Get()->SetExecutionContext(TimeManager::ExcutionContext::Fixed);
	float fixedDelta = TimeManager::Get()->GetFixedDeltaTime();

	while (accumulator >= fixedDelta)
	{
		//실제 고정틱 실행
		accumulator -= fixedDelta;
		TimeManager::Get()->FixedUpdate();

		PhysicsManager::Get()->PreSyncPhysicsWorld();
		PhysicsManager::Get()->PreApplyTransform();
		BehaviourManager::Get()->BroadCastBehaviourMessage("FixedUpdate");
		PhysicsManager::Get()->Simulate(fixedDelta);
		PhysicsManager::Get()->ApplyTransform();
	}

	//GetTime -> 일반시간 반환으로 변경
	TimeManager::Get()->SetExecutionContext(TimeManager::ExcutionContext::Normal);

	//업데이트
	BehaviourManager::Get()->BroadCastBehaviourMessage("Update");
	AudioManager::Get()->Update();
	BehaviourManager::Get()->BroadCastBehaviourMessage("LateUpdate");

	BehaviourManager::Get()->BroadCastBehaviourMessage("OnAnimationUpdate");

	//렌더
	RenderManager::Get()->StartRender();
	RenderManager::Get()->Render();
	BehaviourManager::Get()->BroadCastBehaviourMessage("OnGUI");
	RenderManager::Get()->EndRender();

	//렌더 후 후처리
	//BehaviourManager::Get()->BroadCastBehaviourMessage("OnDisable"); 
	//BehaviourManager::Get()->BroadCastBehaviourMessage("OnDestroy");
	ObjectDestructionManager::Get()->Update();
	BehaviourManager::Get()->DisableBehaviours();
	ObjectDestructionManager::Get()->Clear();
}

void Engine::Quit()
{
	m_isRunning = false;
}

void Engine::Shutdown()
{
	InputManager::Get()->Shutdown();
	TimeManager::Get()->Shutdown();
	SceneManager::Get()->ShutDown();

	ObjectDestructionManager::Get()->Update();
	BehaviourManager::Get()->DisableBehaviours();
	ResourceManager::Get()->Clear();
	ObjectDestructionManager::Get()->Clear();

	BehaviourManager::Get()->ShutDown();
	ResourceManager::Get()->ShutDown();
	PhysicsManager::Get()->ShutDown();
	ObjectDestructionManager::Get()->ShutDown();
	RenderManager::Get()->ShutDown();
#ifdef _OS_WINDOWS
	WICHelper::ShutDown();
	DWriteHelper::ShutDown();
#endif

	//오디오 스레드 종료
	AudioManager::Get()->ShutDown();

	if (m_window)
	{
		delete m_window;
		m_window = nullptr;
	}

#ifdef _OS_WINDOWS
	CoUninitialize();
#endif
}
