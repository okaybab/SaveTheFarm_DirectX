#pragma once
#include <ScriptBehaviour.h>

namespace GOTOEngine
{
	class Camera;
	class DefenseModeCameraManager : public ScriptBehaviour
	{
	private:
		Camera* m_commonCam = nullptr;
		Camera* m_p1Cam = nullptr;
		Camera* m_p2Cam = nullptr;
	public:
		DefenseModeCameraManager()
		{
			REGISTER_BEHAVIOUR_MESSAGE(Awake);
			REGISTER_BEHAVIOUR_MESSAGE(Update);
			REGISTER_BEHAVIOUR_MESSAGE(OnDestroy);
		}

		static DefenseModeCameraManager* instance;

		void Awake();
		void Update();
		void OnDestroy();
	};
}