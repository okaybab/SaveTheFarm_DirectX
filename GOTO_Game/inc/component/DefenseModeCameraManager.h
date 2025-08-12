#pragma once
#include <ScriptBehaviour.h>
#include <AnimationCurve.h>

namespace GOTOEngine
{
	class Camera;
	class CrosshairCollide;
	class DefenseModeCameraManager : public ScriptBehaviour
	{
	private:
		Camera* m_commonCam = nullptr;
		Camera* m_backgroundCam = nullptr;
		Camera* m_p1Cam = nullptr;
		Camera* m_p2Cam = nullptr;
		GameObject* m_p1GO = nullptr;
		GameObject* m_p2GO = nullptr;
		CrosshairCollide* m_p1CrossCol = nullptr;
		CrosshairCollide* m_p2CrossCol = nullptr;

		std::vector<CrosshairCollide*> m_p1SubCrossCol;
		std::vector<CrosshairCollide*> m_p2SubCrossCol;

		float m_wobbleStrength = 0.0f;

		static AnimationCurve s_openMotionCurve;
		static AnimationCurve s_wobbleMotionXCurve;
		static AnimationCurve s_wobbleMotionYCurve;

		bool m_isOpen = false;

		float m_motionTimer = 0.0f;
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
	
		void Open();
		void Close();
	};
}