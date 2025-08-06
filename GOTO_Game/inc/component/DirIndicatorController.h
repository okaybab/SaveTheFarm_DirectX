#pragma once
#include <ScriptBehaviour.h>
#include <vector>

namespace GOTOEngine
{
	class DirIndicator;
	class Sprite;
	class Camera;
	class DirIndicatorController : public ScriptBehaviour
	{
	private:
		std::vector<DirIndicator*> m_indicators;
		
		void LoadIndicator();
	public:
    DirIndicatorController()
    {
        SetExecutionOrder(150);
        REGISTER_BEHAVIOUR_MESSAGE(Awake);
        REGISTER_BEHAVIOUR_MESSAGE(LateUpdate);
        REGISTER_BEHAVIOUR_MESSAGE(OnDestroy);
    }

		Camera* cam = nullptr;
		int capacity = 20;
		int id = -1;

		void Awake();
		void LateUpdate();
		void OnDestroy();

	};
}